#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <string_view>

#include "simple_process/simple_process.hpp"

int main( )
{
#ifdef _WIN32
	simple_process process{ "simple_library_test.exe" };

	while ( !GetAsyncKeyState( VK_END ) ) {
		std::string cin_buffer{ };

		std::getline( std::cin, cin_buffer );

		process.process_assembler.clear( );
		process.process_assembler.add_char( cin_buffer.data( ) );

		auto allocated_text = process.allocate_bytes( );

		process.process_assembler.clear( );
		process.process_assembler.add_byte( 0xB8 // mov eax, print_function
		);
		process.process_assembler.add_pointer( process.base_address( ) + 0x1000 + 0x82 );
		process.process_assembler.add_byte( 0x56, // push esi
		                                    0x68  // push allocated_text
		);
		process.process_assembler.add_pointer( allocated_text );
		process.process_assembler.add_byte( 0xFF, 0xD0,       // call eax
		                                    0x83, 0xC4, 0x08, // add esp, 8
		                                    0xC3              // ret
		);

		//	process.write_bytes( process.base_address( ) + 0x1000 + 0xC426F );
		process.execute_function( );
		process.free_bytes( allocated_text );
	}
#elif linux

#endif

	return 0;
}
