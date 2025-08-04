#ifndef NON_VIRTUAL_INTERFACE_HPP
#define NON_VIRTUAL_INTERFACE_HPP

#include <memory>
#include <utility>

/** C++ Idioms */
namespace pattern {
	namespace cpp_idiom {

		/** Non-Virtual Interface (NVI) */
		namespace non_virtual_interface {
			// https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Non-Virtual_Interface
			// https://en.wikipedia.org/wiki/Non-virtual_interface_pattern

			/**
			* This pattern is closer to pattern Templated Method.
			*
			* The C++ Non-Virtual Interface (NVI) pattern is a design idiom that uses public non-virtual functions in a base class to
			* call private or protected virtual functions. This approach provides a controlled and consistent interface for clients
			* while allowing derived classes to customize specific behaviors through the virtual functions.
			*
			* Encapsulation and Control:
			* The NVI pattern encapsulates the implementation details and provides a level of control over
			* how derived classes can modify behavior. The base class can perform pre- and post-operations around
			* the virtual function calls, ensuring invariants or managing resources.
			*
			* Reduced Complexity for Clients:
			* Clients only need to interact with the public non-virtual interface, simplifying their usage and reducing
			* their dependency on the specific implementation details of derived classes.
			*
			* Relationship to Template Method Pattern:
			* The NVI pattern is closely related to the Template Method design pattern, where an algorithm's structure is
			* defined in a base class, and specific steps of the algorithm are left to be implemented by derived classes.
			*/


			class Base {
			public:
				/**
				* Public Non-Virtual Interface:
				* The functions that clients interact with are public and non-virtual. These functions define the stable
				* and consistent interface of the base class.
				*/
				void DoSomething() { // Публичный интерфейс
					Prepare();
					DoSomethingImpl(); // Вызываем виртуальный метод
					Cleanup();
				}

			protected:
				/*
				* Private/Protected Virtual Implementation:
				* Inside the public non-virtual functions, calls are made to private or protected virtual functions.
				* These virtual functions are the customization points for derived classes.
				*/
				virtual void DoSomethingImpl() = 0; // Защищённый виртуальный метод

			private:
				void Prepare() {
				}

				void Cleanup() {
				}
			};

			class Derived : public Base {
            protected:
                void DoSomethingImpl() override { /* ... */ }
            };

		} // !namespace non_virtual_interface

	} // !namespace cpp_idiom
} // !namespace pattern

#endif // !NON_VIRTUAL_INTERFACE_HPP
