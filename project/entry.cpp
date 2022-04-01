#include <atomic>
#include <chrono>
#include <iostream>
#include <random>

#include "simple_process/simple_process.hpp"

int main( )
{
	simple_process process{ "ac_client.exe" };

	process.process_assembler.clear( );
	process.process_assembler.add( 0x90, 0xC3 );

	process.write_bytes( process.base_address( ) + 0x1000 + 0xC426F );
	process.execute_function( );

	std::cin.get( );

	return 0;
}
