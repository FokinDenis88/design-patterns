// Project TODO section
// TODO: make google tests


#include <iostream>

#include "headers/all-headers.hpp"


/*
	* What to add In Header Library:
	* IFactoryMethod, IPrototype, ICommand, (IFlyweight, FlyweightFabric)
	* (IObjectPoolResource, IObjectPool)
	*/

struct Foo {
	Foo() = default;
	~Foo() = default;
	int a_{};
	int b_{};
};

void DeleteFoo(Foo* foo_ptr) {
	delete foo_ptr;
}

auto DeleteFooLambda = [](Foo* foo_ptr) {
	delete foo_ptr;
	};


int main() {
	//pattern::creational::command::Run();
	//pattern::creational::singleton::Run();
	//pattern::creational::factory_method::Run();
	//pattern::behavioral::observer::Run();
	//pattern::behavioral::strategy::Run();
	//pattern::creational::object_pool::Run();
	//pattern::structural::flyweight::Run();
	//pattern::behavioral::memento_simple::Run();
	//pattern::behavioral::memento_templated::Run();
	//pattern::behavioral::observer_ref::Run();
	//pattern::behavioral::observer_smart_ptr::Run();
	pattern::behavioral::command::Run();
	std::cout << "\nDesign Patterns Finished\n";
};
