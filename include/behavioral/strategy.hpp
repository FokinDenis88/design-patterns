#ifndef STRATEGY_HPP
#define STRATEGY_HPP

#include <iostream>
#include <memory>

/** Software Design Patterns */
namespace pattern {
	namespace behavioral {
		namespace strategy {

			/** Abstract class. */
			class IStrategy {
			public:
				virtual ~IStrategy() = default;

				virtual void Algorithm() = 0;
			};

			/** Concrete class. */
			class StrategyConcrete : public IStrategy {
				void Algorithm() override {
					std::cout << "Hello World";
				}
			};

			class MyTestClass {
			public:
				std::unique_ptr<IStrategy> strategy_{};
			};

			inline void Run() {
				MyTestClass object{};
				object.strategy_ = std::make_unique<StrategyConcrete>();
				object.strategy_->Algorithm();
			};

		} // !namespace strategy
	} // !namespace behavioral
} // !namespace pattern

#endif // !STRATEGY_HPP
