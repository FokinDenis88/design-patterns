#include "creational/singleton.hpp"

#include <iostream>

struct MyStruct {
	int a{};
};

void pattern::creational::singleton::Run() {
	Singleton<MyStruct>& global_singleton = Singleton<MyStruct>::Get();
	global_singleton.data->a = 77;

	std::cout << "my_singleton.singleton_->a" << global_singleton.data->a;
}
