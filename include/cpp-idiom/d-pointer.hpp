#ifndef D_POINTER_HPP
#define D_POINTER_HPP

#include <memory>
#include <utility>

#include "cpp-idiom/pimpl.hpp"

/** C++ Idioms */
namespace pattern {
	namespace cpp_idiom {
		namespace d_pointer {
			// https://en.cppreference.com/w/cpp/language/pimpl
			// A private data member of the class that points to an instance of a structure. A type of Opaque Pointer.

			struct Widget {
			public:

			private:
				/** Unspecified T */
				struct CheshireCat;

				/** d_ptr Opaque Pointer to Unspecified T */
				std::unique_ptr<CheshireCat> d_ptr_;
			};

		} // !namespace d_pointer

	} // !namespace cpp_idiom
} // !namespace pattern

#endif // !D_POINTER_HPP
