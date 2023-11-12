#ifndef NAMED_PARAMETER_HPP
#define NAMED_PARAMETER_HPP

#include <memory>
#include <utility>

#include <iostream>

/** C++ Idioms */
namespace pattern {
	namespace cpp_idiom {
		namespace named_parameter{
			// https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Named_Parameter
			// https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern#Polymorphic_chaining

			class X {
			public:
				int a;
				char b;

				X() : a{ -999 }, b{ 'C' } {}				// Initialize with default values, if any.
				X& setA(int i) { a = i; return *this; }		// non-const function
				X& setB(char c) { b = c; return *this; }	// non-const function

				static X create() {
					return X();
				}
			};

			std::ostream& operator<<(std::ostream& o, X const& x) {
				o << x.a << " " << x.b;
				return o;
			}

			inline void Run() {
				// The following code uses the named parameter idiom.
				std::cout << X::create().setA(10).setB('Z') << std::endl;
			}

		} // !namespace named_parameter

	} // !namespace cpp_idiom
} // !namespace pattern

#endif // !NAMED_PARAMETER_HPP
