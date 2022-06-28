#ifndef LIBRARY_TESTING_SIMPLE_ASSEMBLER_HPP
#define LIBRARY_TESTING_SIMPLE_ASSEMBLER_HPP

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "../simple_vector/simple_vector.hpp"

struct simple_instruction {
protected:
	std::byte opcode{ };

public:
	explicit simple_instruction( std::byte opcode ) : opcode( opcode ) { }

	virtual void get_bytes( std::byte* bytes ) = 0;
};

namespace simple_instructions
{
	struct simple_mov : public simple_instruction {
		std::byte operand_one{ };
		std::byte operand_two{ };

		//-----------------------------------------------------------------------------
		// @PURPOSE : Initialize with a source and destination.
		// @INPUT   :
		//				source: What you want to move.
		//				destination: Where you want to move to.
		//-----------------------------------------------------------------------------
		simple_mov( std::byte source, std::byte destination )
			: simple_instruction( static_cast< std::byte >( 88 ) ), operand_one( destination ), operand_two( source )
		{
		}

		void get_bytes( std::byte* bytes ) override
		{
			bytes[ 0 ] = opcode;
			bytes[ 1 ] = operand_one;
			bytes[ 2 ] = operand_two;
		}
	};
} // namespace simple_instructions

struct simple_assembler {
public:
	simple_vector< std::byte > assembler_instructions{ };

	//-----------------------------------------------------------------------------
	// @PURPOSE : Default initializer.
	// @INPUT   : No arguments.
	//-----------------------------------------------------------------------------
	simple_assembler( ) = default;

	//-----------------------------------------------------------------------------
	// @PURPOSE : Initialize with a buffer.
	// @INPUT   :
	//				assembler_bytes: buffer to initialize with.
	//				assembler_size: size of the buffer.
	//-----------------------------------------------------------------------------
	simple_assembler( std::byte* assembler_bytes, std::size_t assembler_members ) : assembler_instructions( assembler_bytes, assembler_members )
	{
		assert( assembler_bytes != nullptr );
		assert( assembler_members > 0 );
	}

	//-----------------------------------------------------------------------------
	// @PURPOSE : Adds an instruction.
	// @INPUT   :
	//				instruction: Instruction to add.
	//-----------------------------------------------------------------------------
	//	template< typename INSTRUCTION = simple_instructions::simple_mov >
	//	void add( INSTRUCTION instruction )
	//	{
	//		auto allocated_instruction = new INSTRUCTION( instruction );
	//
	//		assembler_instructions.insert( allocated_instruction );
	//	}

	//-----------------------------------------------------------------------------
	// @PURPOSE : Adds an instruction byte(s).
	// @INPUT   :
	//				instruction_byte: Instruction byte(s) to add.
	//-----------------------------------------------------------------------------
	template< typename... BYTES >
	void add_byte( BYTES... instruction_bytes )
	{
		( assembler_instructions.insert( static_cast< std::byte >( instruction_bytes ) ), ... );
	}

	//-----------------------------------------------------------------------------
	// @PURPOSE : Adds a pointer to byte list.
	// @INPUT   :
	//				pointer: Pointer to add to the list.
	//-----------------------------------------------------------------------------
	template< typename... POINTERS >
	void add_pointer( POINTERS... pointer )
	{
		std::byte instruction_bytes_array[ sizeof( std::uintptr_t ) ]{ };

		auto write_instruction = [ & ]( std::uintptr_t pointer ) {
			std::memcpy( instruction_bytes_array, &pointer, sizeof( std::uintptr_t ) );

			for ( auto& iterator : instruction_bytes_array ) {
				assembler_instructions.insert( iterator );
			}
		};

		( write_instruction( pointer ), ... );
	}

	//-----------------------------------------------------------------------------
	// @PURPOSE : Adds a char array to byte list.
	// @INPUT   :
	//				char_array: Char array to add to the list.
	//-----------------------------------------------------------------------------
	template< typename... CHARS >
	void add_char( CHARS... char_array )
	{
		auto write_instruction = [ & ]( unsigned char* char_array ) {
			for ( int iterator = 0; char_array[ iterator ] != '\0'; iterator++ ) {
				assembler_instructions.insert( std::byte{ char_array[ iterator ] } );
			}
		};

		( write_instruction( reinterpret_cast< unsigned char* >( char_array ) ), ... );
	}

	//-----------------------------------------------------------------------------
	// @PURPOSE : Erases all current bytes.
	// @INPUT   : No arguments.
	//-----------------------------------------------------------------------------
	void clear( )
	{
		for ( auto iterator = assembler_instructions.begin( ); iterator != assembler_instructions.end( ); ) {
			iterator = iterator.erase( );
		}
	}
};

#endif // LIBRARY_TESTING_SIMPLE_ASSEMBLER_HPP
