#ifndef LIBRARY_TESTING_SIMPLE_VECTOR_HPP
#define LIBRARY_TESTING_SIMPLE_VECTOR_HPP

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

template< class STRUCT >
struct simple_vector_iterator {
protected:
	std::size_t* iterator_members{ };
	std::size_t iterator_location{ };
	std::size_t iterator_size{ };
	void** iterator_memory{ };

public:
	//-----------------------------------------------------------------------------
	// @PURPOSE : Default initializer, not allowed.
	// @INPUT   : No arguments.
	//-----------------------------------------------------------------------------
	simple_vector_iterator( )
	{
		assert( false );
	}

	//-----------------------------------------------------------------------------
	// @PURPOSE : Initialize iterator with memory.
	// @INPUT   :
	//				vector_memory: Location of vectors allocated_memory.
	//				vector_members: Max amount of members.
	//-----------------------------------------------------------------------------
	explicit simple_vector_iterator( void** vector_memory, std::size_t* vector_members )
		: iterator_memory( vector_memory ), iterator_members( vector_members ), iterator_size( sizeof( STRUCT ) )
	{
		assert( vector_memory );
		assert( vector_members );
		assert( iterator_size );
	}

	//-----------------------------------------------------------------------------
	// @PURPOSE : Initialize iterator with memory.
	// @INPUT   :
	//				vector_memory: Location of vectors allocated_memory.
	//				vector_members: Max amount of members.
	//				vector_location: Pre-assumed location of memory.
	//-----------------------------------------------------------------------------
	simple_vector_iterator( void** vector_memory, std::size_t* vector_members, std::size_t vector_location )
		: iterator_memory( vector_memory ), iterator_members( vector_members ), iterator_size( sizeof( STRUCT ) ),
		  iterator_location( vector_location )
	{
		assert( vector_memory );
		assert( vector_members );
		assert( iterator_size );
	}

	//-----------------------------------------------------------------------------
	// @PURPOSE : Compare self to another iterator to see if equal.
	// @INPUT   :
	//				other_iterator: Iterator you're comparing.
	//-----------------------------------------------------------------------------
	bool operator==( simple_vector_iterator< STRUCT > other_iterator )
	{
		return other_iterator.iterator_location == iterator_location;
	}

	//-----------------------------------------------------------------------------
	// @PURPOSE : Compare self to another iterator to see if not equal.
	// @INPUT   :
	//				other_iterator: Iterator you're comparing.
	//-----------------------------------------------------------------------------
	bool operator!=( simple_vector_iterator< STRUCT > other_iterator )
	{
		return other_iterator.iterator_location != iterator_location;
	}

	//-----------------------------------------------------------------------------
	// @PURPOSE : Forward increment.
	// @INPUT   : No arguments.
	//-----------------------------------------------------------------------------
	simple_vector_iterator< STRUCT >& operator++( )
	{
		return next( );
	}

	//-----------------------------------------------------------------------------
	// @PURPOSE : Get via pointer operator.
	// @INPUT   : No arguments.
	//-----------------------------------------------------------------------------
	STRUCT operator*( )
	{
		return this->get( );
	}

	//-----------------------------------------------------------------------------
	// @PURPOSE : Increment the iterator and return self.
	// @INPUT   : No arguments.
	//-----------------------------------------------------------------------------
	simple_vector_iterator< STRUCT >& next( )
	{
		iterator_location++;

		return *this;
	}

	//-----------------------------------------------------------------------------
	// @PURPOSE : Get the current value of the iterator.
	// @INPUT   : No arguments.
	//-----------------------------------------------------------------------------
	STRUCT& get( )
	{
		auto& memory             = *iterator_memory;
		auto iterator_memory_ptr = reinterpret_cast< std::uintptr_t >( memory );

		return *reinterpret_cast< STRUCT* >( iterator_memory_ptr + iterator_size * iterator_location );
	}

	//-----------------------------------------------------------------------------
	// @PURPOSE : Erases the current iterator from vector's memory.
	// @INPUT   : No arguments.
	//-----------------------------------------------------------------------------
	simple_vector_iterator< STRUCT >& erase( )
	{
		auto& members = *iterator_members;
		auto& memory  = *iterator_memory;

		auto iterator_memory_ptr    = reinterpret_cast< std::uintptr_t >( memory );
		auto iterator_section_start = iterator_memory_ptr + iterator_size * ( iterator_location + 1 );

		if ( members - 1 != 0 ) {
			auto clean_data = malloc( iterator_size * ( members - iterator_location + 1 ) );
			memcpy( clean_data, reinterpret_cast< void* >( iterator_section_start ), iterator_size * ( members - iterator_location + 1 ) );

			memory = realloc( memory, iterator_size * ( members - 1 ) );
			memcpy( memory, clean_data, iterator_size * ( members - 1 ) );

			free( clean_data );

			( members )--;
		} else {
			memset( memory, 0, iterator_size );

			iterator_location = 0;
			members           = 0;
		}

		return *this;
	}
};

template< class STRUCT >
struct simple_vector {
protected:
	void* allocated_memory{ };
	std::size_t allocated_size{ };
	std::size_t allocated_members{ };

public:
	//-----------------------------------------------------------------------------
	// @PURPOSE : Blank initializer.
	// @INPUT   : No arguments.
	//-----------------------------------------------------------------------------
	simple_vector( )
	{
		allocated_memory  = malloc( sizeof( STRUCT ) );
		allocated_size    = sizeof( STRUCT );
		allocated_members = 0;
	}

	//-----------------------------------------------------------------------------
	// @PURPOSE : Initialize with some base allocation.
	// @INPUT   :
	//				base_allocation: Portion of allocated memory.
	//-----------------------------------------------------------------------------
	simple_vector( void* base_allocation, std::size_t base_members )
		: allocated_memory( base_allocation ), allocated_members( base_members ), allocated_size( sizeof( STRUCT ) )
	{
		assert( base_allocation );
	}

	//-----------------------------------------------------------------------------
	// @PURPOSE : Returns an iterator at given location.
	// @INPUT   :
	//				operator_location: Location where you want to index.
	//-----------------------------------------------------------------------------
	simple_vector_iterator< STRUCT > operator[]( int operator_location )
	{
		assert( operator_location < allocated_members || operator_location > 0 );

		return simple_vector_iterator< STRUCT >( &allocated_memory, &allocated_members, operator_location );
	}

	//-----------------------------------------------------------------------------
	// @PURPOSE : Removes all memory of the instance.
	// @INPUT   : No arguments.
	//-----------------------------------------------------------------------------
	void remove( )
	{
		free( allocated_memory );
	}

	//-----------------------------------------------------------------------------
	// @PURPOSE : Insert struct into vector's memory.
	// @INPUT   :
	//				insert_struct: What you want to insert into the vector.
	//-----------------------------------------------------------------------------
	simple_vector_iterator< STRUCT > insert( STRUCT insert_struct )
	{
		auto allocated_memory_ptr = reinterpret_cast< std::uintptr_t >( allocated_memory );

		memcpy( reinterpret_cast< void* >( allocated_memory_ptr + allocated_size * allocated_members ), &insert_struct, allocated_size );
		allocated_memory = realloc( allocated_memory, allocated_size * ( allocated_members + 2 ) );

		allocated_members++;

		return simple_vector_iterator< STRUCT >( &allocated_memory, &allocated_members, allocated_members );
	}

	//-----------------------------------------------------------------------------
	// @PURPOSE : Return the start of the vector.
	// @INPUT   : No arguments.
	//-----------------------------------------------------------------------------
	simple_vector_iterator< STRUCT > begin( )
	{
		return simple_vector_iterator< STRUCT >( &allocated_memory, &allocated_members, 0 );
	}

	//-----------------------------------------------------------------------------
	// @PURPOSE : Return the end of the vector.
	// @INPUT   : No arguments.
	//-----------------------------------------------------------------------------
	simple_vector_iterator< STRUCT > end( )
	{
		return simple_vector_iterator< STRUCT >( &allocated_memory, &allocated_members, allocated_members );
	}

	//-----------------------------------------------------------------------------
	// @PURPOSE : Returns the size of the vector in bytes.
	// @INPUT   : No arguments.
	//-----------------------------------------------------------------------------
	[[nodiscard]] std::size_t size( )
	{
		return allocated_members * allocated_size;
	}

	//-----------------------------------------------------------------------------
	// @PURPOSE : Returns the vector in bytes.
	// @INPUT   : No arguments.
	//-----------------------------------------------------------------------------
	std::byte* data( )
	{
		return reinterpret_cast< std::byte* >( allocated_memory );
	}
};

#endif // LIBRARY_TESTING_SIMPLE_VECTOR_HPP
