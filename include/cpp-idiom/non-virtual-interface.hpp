#ifndef NON_VIRTUAL_INTERFACE_HPP
#define NON_VIRTUAL_INTERFACE_HPP

#include <memory>
#include <utility>

/** C++ Idioms */
namespace pattern {
	namespace cpp_idiom {
		namespace non_virtual_interface {
			// https://en.cppreference.com/w/cpp/language/pimpl

			class Widget {
			public:
				// public data and functions

				// All public, protected, and virtual members remain in the interface class

			private:
				// All private data members and all private non - virtual member functions are placed in the implementation class.

				/** Unspecified Type */
				struct Impl;

				/** Opaque Pointer to Unspecified Type */
				std::unique_ptr<Impl> pImpl_;
			};

		} // !namespace non_virtual_interface

	} // !namespace cpp_idiom
} // !namespace pattern

#endif // !NON_VIRTUAL_INTERFACE_HPP
