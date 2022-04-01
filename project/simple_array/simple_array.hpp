#ifndef LIBRARY_TESTING_SIMPLE_ARRAY_HPP
#define LIBRARY_TESTING_SIMPLE_ARRAY_HPP

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

template< class STRUCT, std::size_t SIZE >
struct simple_array {
protected:
	STRUCT allocated_array[ SIZE ]{ };

public:
//	operator
};

#endif // LIBRARY_TESTING_SIMPLE_ARRAY_HPP
