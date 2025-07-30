#ifndef SFINAE_HPP
#define SFINAE_HPP

#include <type_traits>


/** C++ Idioms */
namespace pattern {
	namespace cpp_idiom {
		namespace sfinae {
			// sfinae					- https://en.cppreference.com/w/cpp/language/sfinae.html
			//							- https://ru.wikipedia.org/wiki/SFINAE
			// overload resolution		- https://en.cppreference.com/w/cpp/language/overload_resolution.html
			// candidate functions		- https://en.cppreference.com/w/cpp/language/overload_resolution.html#Candidate_functions
			// name lookup				- https://en.cppreference.com/w/cpp/language/lookup.html
			// unevaluated statements	- https://en.cppreference.com/w/cpp/language/expressions.html#Potentially-evaluated_expressions
			// Template argument deduction - https://en.cppreference.com/w/cpp/language/template_argument_deduction.html

			/**
			* Overload resolution order
			* 1. Simple Not template function. Exact match & trivial conversion.
			* 2. Template Function. (No conversions with template arguments). First choose more specialized. Then more general.
			* 3. Simple Not template function. Conversion of arguments.
			* 4. Simple Not template function. Variadic. (...)
			* 5. Template function. Variadic. (...)
			*/

			/**
			* Unevaluated statements:
			* The following operands are unevaluated operands, they are not evaluated:
			* 1. expressions which the typeid operator applies to, except glvalues of polymorphic class types
			* 2. expressions which are operands of the sizeof operator
			* 3. operands of the noexcept operator
			* 4. operands of the decltype specifier
			*
			* c++ 20
			* 1. constraint-expression of concept definitions
			* 2. expressions following the requires keyword of requires clauses
			* 3. expressions appearing in requirement-seq of requires expressions
			*/

			/**
			* Main moments:
			* sizeof for resolution choose
			* decltype for statement correctness check. declval for use in decltype expression.
			* enable_if for resolution choose
			* variadic simple and template functions for resolution overload fail control
			* std::true_type, std::false_type for return from functions
			*/

			/**
			* Alternatives:
			* Where applicable, tag dispatch, if constexpr(since C++17), and concepts (since C++20) are usually
			* preferred over use of SFINAE.
			* static_assert is usually preferred over SFINAE if only a conditional compile time error is wanted.
			*/

			/**
			* A common idiom is to use expression SFINAE on the return type, where the expression uses the comma operator, whose
			* left subexpression is the one that is being examined (cast to void to ensure the user-defined operator comma on the
			* returned type is not selected), and the right subexpression has the type that the function is supposed to return.
			*/


			// Fedor Pikus. Idioms and design patterns in modern C++.
			// https://github.com/PacktPublishing/Hands-On-Design-Patterns-with-CPP-Second-Edition


			template<typename T>
			class is_class {
			private:
				template<typename C> static char test(int C::*);
				template<typename C> static int test(...);

			public:
				static constexpr bool value = sizeof(test<T>(0)) == sizeof(int);
			};

//===========================================================================================================================

			struct have_sort { char c; };
			struct have_range { char c; have_sort c1; };
			struct have_nothing { char c; have_range c1; };

			template<typename T>
			have_sort test_sort(decltype(&T::sort), decltype(&T::sort));

			template<typename T>
			have_range test_sort(decltype(&T::begin), decltype(&T::end));

			template<typename T>
            have_nothing test_sort(...);

			template<typename T>
			typename std::enable_if<sizeof(test_sort<T>(NULL, NULL)) == sizeof(have_sort), void>::type
				fast_sort(T& x) {
                x.sort();
			}

			template<typename T>
			typename std::enable_if<sizeof(test_sort<T>(NULL, NULL)) == sizeof(have_range), void>::type
				fast_sort(T& x) {
				std::sort(x.begin(), x.end());
			}

//===========================================================================================================================

			namespace last_variant {
				struct yes { char c; };
                struct no { char c; yes c1; };

				template<typename T> yes test_have_sort(decltype(&T::sort));
				template<typename T> no test_have_sort(...);
				template<typename T> yes test_have_range(decltype(&T::begin), decltype(&T::end));
				template<typename T> no test_have_range(...);

				template<typename T, bool have_sort, bool have_range> struct fast_sort_helper;

				template<typename T>
				struct fast_sort_helper<T, true, true> {
					static void fast_sort(T& x) {
						x.sort();
					}
				};

				template<typename T>
				struct fast_sort_helper<T, true, false> {
					static void fast_sort(T& x) {
						x.sort();
					}
				};

				template<typename T>
				struct fast_sort_helper<T, false, true> {
					static void fast_sort(T& x) {
						std::sort(x.begin(), x.end());
					}
				};

				template<typename T>
				struct fast_sort_helper<T, false, false> {
					static void fast_sort(T& x) {
						static_assert(sizeof(T) < 0, "No function for sorting.");
					}
				};

				template<typename T>
				void fast_sort(T& x) {
					fast_sort_helper<T, sizeof(test_have_sort<T>(NULL)) == sizeof(yes),
										sizeof(test_have_range<T>(NULL, NULL)) == sizeof(yes)>::fast_sort(x);
				};

			} // !namespace last_variant

//===========================================================================================================================

			template<typename Lambda>
			struct is_valid_helper {
				template<typename... LambdaArgs>
				constexpr auto test(int)
						-> decltype(std::declval<Lambda>()(std::declval<LambdaArgs>()...), std::true_type()) {
					return std::true_type();
				}

				template<typename... LambdaArgs>
				constexpr std::false_type test(...) {
					return std::false_type();
				}

				template<typename... LambdaArgs>
				constexpr auto operator()(const LambdaArgs&...) {
					return this->test<LambdaArgs...>(0);
				}
			};

			template<typename Lambda>
			constexpr auto is_valid(const Lambda&) {
				return is_valid_helper<Lambda>{};
			}


			struct A {
				int a{};
			};

			auto is_assignable = is_valid([](auto&& x) -> decltype(x = x) {});
			void my_function(const A& a) {
				static_assert(decltype(is_assignable(a))::value, "A is not assignable");
			}

			auto is_default_constructible = is_valid([](auto&& x) -> decltype(new typename std::remove_reference_t<decltype(x)>) {});

			auto is_destructible = is_valid([](auto&& x) -> decltype(delete x) {});

		} // !namespace sfinae


	} // !namespace cpp_idiom
} // !namespace pattern

#endif // !SFINAE_HPP
