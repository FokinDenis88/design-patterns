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
			protected:
				IStrategy() = default;
				IStrategy(const IStrategy&) = delete; // C.67	C.21
				IStrategy& operator=(const IStrategy&) = delete;
				IStrategy(IStrategy&&) noexcept = delete;
				IStrategy& operator=(IStrategy&&) noexcept = delete;
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

		} // !namespace strategy

	} // !namespace behavioral
} // !namespace pattern

#endif // !STRATEGY_HPP
