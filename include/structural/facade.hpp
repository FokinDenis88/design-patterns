#ifndef FACADE_HPP
#define FACADE_HPP

#include <memory>
#include <utility>

/** Software Design Patterns */
namespace pattern {
	namespace structural {
		namespace facade {
			// https://en.wikipedia.org/wiki/Facade_pattern

			class ComplexClass {
			public:
				void Action_1() {};
				void Action_2() {};
				void Action_3() {};
				void Action_4() {};
				void Action_5() {};
			};

			class Facade {
			public:
				void Action_1_2() {};
				void Action_3_4() {};
			};

			inline void Run() {

			};

		} // !namespace facade
	} // !namespace creational
} // !namespace pattern

#endif // !FACADE_HPP
