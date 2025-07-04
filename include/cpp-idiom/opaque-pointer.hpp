#ifndef OPAQUE_POINTER_HPP
#define OPAQUE_POINTER_HPP

#include <memory>

#include "cpp-idiom/pimpl.hpp"

/** C++ Idioms */
namespace pattern {
	namespace cpp_idiom {
		namespace opaque_pointer {
			// https://en.wikipedia.org/wiki/Opaque_pointer
			// A pointer to a record or data structure of some unspecified type

			class A {
			public:
				/** Unspecified T */
				struct SomeStruct;

				/** Opaque Pointer to Unspecified T */
				std::unique_ptr<SomeStruct> pStruct_;
			};

		} // !namespace opaque_pointer

	} // !namespace cpp_idiom
} // !namespace pattern

#endif // !OPAQUE_POINTER_HPP
