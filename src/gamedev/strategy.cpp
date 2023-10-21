#include "behavioral/strategy.hpp"

#include <iostream>

void pattern::behavioral::strategy::Run() {
	MyTestClass object{};
	object.strategy_ = std::make_unique<StrategyConcrete>();
	object.strategy_->Algorithm();
}
