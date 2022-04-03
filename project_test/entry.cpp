#include <iostream>

void __cdecl print( const char* text )
{
	std::cout << text << std::endl;
}

int main( )
{
	print( "Hello World!" );

	std::cin.get( );

	return 0;
}