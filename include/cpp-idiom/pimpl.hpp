#ifndef PIMPL_HPP
#define PIMPL_HPP

#include <memory>
#include <utility>

#include "cpp-idiom/pimpl-macros.hpp"


/** C++ Idioms */
namespace pattern {
	namespace cpp_idiom {
		namespace pimpl_1 {
			// https://en.cppreference.com/w/cpp/language/pimpl

			class Widget {
			public:
				Widget() = default;
				~Widget();

				Widget(const Widget& obj);
				Widget& operator=(const Widget& rhs);
				Widget(Widget&& obj) noexcept;
				Widget& operator=(Widget&& rhs) noexcept;

				// public data and functions

				// All public, protected, and virtual members remain in the interface class

			private:
				/**
				 * Unspecified Type.
				 * All private data members and all private non - virtual member functions are placed in the implementation class.
				 */
				struct Impl;

				/** Opaque Pointer to Unspecified Type */
				std::unique_ptr<Impl> pImpl_;
			};

			void Run();

		} // !namespace pimpl_1

		namespace pimpl_2 {
			class Widget {
			public:
				Widget() = default;

				DECLARE_PIMPL(Widget)
			};

			void Run();

		} // !namespace pimpl_2
	} // !namespace cpp_idiom
} // !namespace pattern

#endif // !PIMPL_HPP
