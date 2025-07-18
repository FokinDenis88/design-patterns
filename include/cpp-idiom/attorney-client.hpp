#ifndef ATTORNEY_CLIENT_HPP
#define ATTORNEY_CLIENT_HPP

#include <memory>
#include <utility>

/** C++ Idioms */
namespace pattern {
	namespace cpp_idiom {
		namespace attorney_client {
			// https://en.cppreference.com/w/cpp/language/pimpl

			class Widget {
			public:
				// public data and functions

				// All public, protected, and virtual members remain in the interface class

			private:
				// All private data members and all private non - virtual member functions are placed in the implementation class.

				/** Unspecified T */
				struct Impl;

				/** Opaque Pointer to Unspecified T */
				std::unique_ptr<Impl> pImpl_;
			};

		} // !namespace attorney_client

	} // !namespace cpp_idiom
} // !namespace pattern

#endif // !ATTORNEY_CLIENT_HPP
