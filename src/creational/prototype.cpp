#include "creational/prototype.hpp"

#include <iostream>

void pattern::creational::prototype::Run() {
	Client client{};
	client.creator = std::make_unique<PrototypeA>();
	client.creator->Clone();
}
