#ifndef LIBRARY_TESTING_SIMPLE_PROCESS_HPP
#define LIBRARY_TESTING_SIMPLE_PROCESS_HPP

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string_view>

#include <Windows.h>

#include <TlHelp32.h>

#include "../simple_assembler/simple_assembler.hpp"

typedef struct client_id {
	PVOID unique_process;
	PVOID unique_thread;
} client_id;
typedef HANDLE handle;
typedef HMODULE hmodule;
typedef long( __stdcall* RtlCreateUserThread )( handle, PSECURITY_DESCRIPTOR, bool, unsigned long, unsigned long, unsigned long, void*, void*,
                                                handle*, client_id* );
typedef long( __stdcall* NtAllocateVirtualMemory )( handle, void**, unsigned long, unsigned long*, unsigned long, unsigned long );
typedef long( __stdcall* NtFreeVirtualMemory )( handle, void**, unsigned long*, unsigned long );
typedef long( __stdcall* NtWriteVirtualMemory )( handle, void*, void*, unsigned long, unsigned long* );
typedef long( __stdcall* NtProtectVirtualMemory )( handle, void**, unsigned long*, unsigned long, unsigned long* );

struct simple_process {
protected:
	handle process_handle{ };
	std::string_view process_name{ };
	std::size_t process_base_address{ };

	//-----------------------------------------------------------------------------
	// @PURPOSE : Finds windows handle.
	// @INPUT   : No arguments.
	//-----------------------------------------------------------------------------
	handle find_process_handle( )
	{
		handle temp_process_handle{ };

		PROCESSENTRY32 process_entry{ };
		process_entry.dwSize = sizeof( PROCESSENTRY32 );

		handle snapshot{ };
		snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );

		if ( snapshot == INVALID_HANDLE_VALUE ) {
			return INVALID_HANDLE_VALUE;
		}

		if ( !Process32First( snapshot, &process_entry ) ) {
			CloseHandle( snapshot );
			return INVALID_HANDLE_VALUE;
		}

		do {
			if ( !strcmp( process_entry.szExeFile, process_name.data( ) ) ) {
				temp_process_handle = OpenProcess( PROCESS_ALL_ACCESS, FALSE, process_entry.th32ProcessID );
				break;
			}
		} while ( Process32Next( snapshot, &process_entry ) );

		CloseHandle( snapshot );

		if ( temp_process_handle == INVALID_HANDLE_VALUE ) {
			return INVALID_HANDLE_VALUE;
		}

		return temp_process_handle;
	}

	//-----------------------------------------------------------------------------
	// @PURPOSE : Finds process base address.
	// @INPUT   : No arguments.
	//-----------------------------------------------------------------------------
	std::size_t find_process_base_address( )
	{
		MODULEENTRY32 module_entry{ };
		module_entry.dwSize = sizeof( MODULEENTRY32 );

		handle snapshot{ };
		snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, GetProcessId( process_handle ) );

		if ( snapshot == INVALID_HANDLE_VALUE ) {
			return { };
		}

		if ( !Module32First( snapshot, &module_entry ) ) {
			CloseHandle( snapshot );
			return { };
		}

		do {
			if ( !strcmp( module_entry.szModule, process_name.data( ) ) ) {
				CloseHandle( snapshot );
				return reinterpret_cast< std::size_t >( module_entry.modBaseAddr );
			}
		} while ( Module32Next( snapshot, &module_entry ) );

		CloseHandle( snapshot );

		return { };
	}

public:
	simple_assembler process_assembler{ };

	//-----------------------------------------------------------------------------
	// @PURPOSE : Default initializer, not allowed.
	// @INPUT   : No arguments.
	//-----------------------------------------------------------------------------
	simple_process( )
	{
		assert( false );
	}

	//-----------------------------------------------------------------------------
	// @PURPOSE : Initialize with the process name.
	// @INPUT   :
	//				process_name: The process name.
	//-----------------------------------------------------------------------------
	explicit simple_process( std::string_view process_name ) : process_name( process_name )
	{
		process_handle       = find_process_handle( );
		process_base_address = find_process_base_address( );

		assert( process_handle != nullptr );
		assert( process_handle != INVALID_HANDLE_VALUE );

		// assert( process_base_address != std::byte{ } );
	}

	//-----------------------------------------------------------------------------
	// @PURPOSE : Writes your own bytes to a function.
	// @INPUT   :
	//				address: The address of the function.
	//-----------------------------------------------------------------------------
	void write_bytes( std::uintptr_t function_address )
	{
		auto buffer_function_address = function_address;
		auto bytes_to_be_allocated   = process_assembler.assembler_instructions.size( );
		auto protect_bytes           = bytes_to_be_allocated;

		hmodule nt_dll = LoadLibrary( "ntdll.dll" );

		auto nt_protect_virtual_memory = reinterpret_cast< NtProtectVirtualMemory >( GetProcAddress( nt_dll, "NtProtectVirtualMemory" ) );
		auto nt_write_virtual_memory   = reinterpret_cast< NtWriteVirtualMemory >( GetProcAddress( nt_dll, "NtWriteVirtualMemory" ) );

		std::size_t old_protection_flags{ };

		nt_protect_virtual_memory( process_handle, reinterpret_cast< void** >( &buffer_function_address ),
		                           reinterpret_cast< unsigned long* >( &protect_bytes ), PAGE_EXECUTE_READWRITE,
		                           reinterpret_cast< unsigned long* >( &old_protection_flags ) );

		if ( nt_write_virtual_memory( process_handle, reinterpret_cast< void* >( function_address ), process_assembler.assembler_instructions.data( ),
		                              bytes_to_be_allocated, nullptr ) != 0 ) {
			return;
		}

		nt_protect_virtual_memory( process_handle, reinterpret_cast< void** >( &buffer_function_address ),
		                           reinterpret_cast< unsigned long* >( &protect_bytes ), old_protection_flags,
		                           reinterpret_cast< unsigned long* >( &old_protection_flags ) );

		FreeLibrary( nt_dll );
	}

	//-----------------------------------------------------------------------------
	// @PURPOSE : Allocates a function to be ran with run_function.
	// @INPUT   :
	//				function_bytes: The bytes to be allocated.
	//-----------------------------------------------------------------------------
	std::uintptr_t allocate_function( )
	{
		auto bytes_to_be_allocated = process_assembler.assembler_instructions.size( );
		auto bytes_allocated       = bytes_to_be_allocated;

		hmodule nt_dll = LoadLibrary( "ntdll.dll" );

		auto nt_allocate_virtual_memory = reinterpret_cast< NtAllocateVirtualMemory >( GetProcAddress( nt_dll, "NtAllocateVirtualMemory" ) );
		auto nt_free_virtual_memory     = reinterpret_cast< NtFreeVirtualMemory >( GetProcAddress( nt_dll, "NtFreeVirtualMemory" ) );
		auto nt_write_virtual_memory    = reinterpret_cast< NtWriteVirtualMemory >( GetProcAddress( nt_dll, "NtWriteVirtualMemory" ) );

		void* allocated_address{ };

		if ( nt_allocate_virtual_memory( process_handle, &allocated_address, 0, reinterpret_cast< unsigned long* >( &bytes_allocated ),
		                                 MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE ) != 0 ) {
			return { };
		}

		if ( nt_write_virtual_memory( process_handle, allocated_address, process_assembler.assembler_instructions.data( ), bytes_to_be_allocated,
		                              nullptr ) != 0 ) {
			nt_free_virtual_memory( process_handle, &allocated_address, nullptr, MEM_RELEASE );

			return { };
		}

		FreeLibrary( nt_dll );

		return reinterpret_cast< std::uintptr_t >( allocated_address );
	}

	//-----------------------------------------------------------------------------
	// @PURPOSE : Creates a thread on process using RtlCreateUserThread.
	// @INPUT   :
	//				address: The address of the function.
	//-----------------------------------------------------------------------------
	void run_function( std::uintptr_t address )
	{
		hmodule nt_dll = LoadLibrary( "ntdll.dll" );

		auto rtl_create_user_thread = reinterpret_cast< RtlCreateUserThread >( GetProcAddress( nt_dll, "RtlCreateUserThread" ) );

		handle thread_handle{ };
		client_id thread_id{ };

		auto status = rtl_create_user_thread( process_handle, nullptr, FALSE, 0, 0, 0, reinterpret_cast< LPTHREAD_START_ROUTINE >( address ), nullptr,
		                                      &thread_handle, &thread_id );

		if ( status != 0 ) {
			assert( false );
		};

		WaitForSingleObject( thread_handle, INFINITE );

		CloseHandle( thread_handle );

		FreeLibrary( nt_dll );
	}

	//-----------------------------------------------------------------------------
	// @PURPOSE : Free's a function.
	// @INPUT   :
	//				address: The address of the function.
	//-----------------------------------------------------------------------------
	void free_function( std::uintptr_t address )
	{
		hmodule nt_dll = LoadLibrary( "ntdll.dll" );

		auto nt_free_virtual_memory = reinterpret_cast< NtFreeVirtualMemory >( GetProcAddress( nt_dll, "NtFreeVirtualMemory" ) );

		void* allocated_address{ };

		nt_free_virtual_memory( process_handle, &allocated_address, nullptr, MEM_RELEASE );

		FreeLibrary( nt_dll );
	}

	//-----------------------------------------------------------------------------
	// @PURPOSE : Creates and runs a function
	// @INPUT   : No arguments.
	//-----------------------------------------------------------------------------
	void execute_function( )
	{
		std::uintptr_t function_address = allocate_function( );

		if ( !function_address ) {
			return;
		}

		run_function( function_address );
		free_function( function_address );
	}

	//-----------------------------------------------------------------------------
	// @PURPOSE : Gets the process's base address.
	// @INPUT   : No arguments.
	//-----------------------------------------------------------------------------
	[[nodiscard]] std::uintptr_t base_address( ) const
	{
		return process_base_address;
	}
};

#endif // LIBRARY_TESTING_SIMPLE_PROCESS_HPP
