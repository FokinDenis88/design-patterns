#include "gtest/gtest.h"

#include <functional>

#include "behavioral/command.hpp"


namespace {
	namespace pattern {
		namespace architectural {
			namespace mvc {

			} // !namespace mvc
		} // namespace architectural


		namespace behavioral {
			namespace chain_of_responsibility {
			} // !namespace chain_of_responsibility

			namespace command {
					using namespace ::pattern::behavioral::command;

					TEST(CommandTest, CommandSTDFunctionClass) {
						//TestClass<decltype(Receiver::Add)> test{};
						//int a = 66 - 44;

						//std::function<CommandSTDFunction::ActionType> func_object = std::bind(&Receiver::Add, receiver.get(), 40, 50);

						std::shared_ptr<Receiver> receiver{ std::make_shared<Receiver>() };
						auto command{ std::make_shared<CommandSTDFunction>(&Receiver::Add, *receiver, 40, 50) };
						std::unique_ptr<Invoker> invoker{ std::make_unique<Invoker>() };

						/*Client client{};
						client.set_receiver(receiver);
						client.set_command(command);
						client.set_invoker(std::move(invoker));*/

						//std::cout << "Add_result: " << client.InvokeCommand() << '\n';
						ASSERT_EQ(1, 2) << "Test CommandSTDFunctionClass";
					}

					TEST(CommandTest, CommandClass) {

						//auto fn{ &Receiver::Add };
						//int (Receiver::*fn)(int, int){ &Receiver::Add };
						//(receiver_test.*fn)(20, 30);
						//auto res_2{ *fn(&receiver_test, 20, 50) };
						//auto res_2{ receiver_test.*fn(20, 30) };
						//std::function<CommandFunction::ActionType> func_object = std::bind(&Receiver::Add, receiver.get(), 40, 50);

						Receiver receiver_test{};
						std::shared_ptr<Receiver> receiver_2{ std::make_shared<Receiver>() };
						using CommandType = Command<Receiver, int, int, int>;
						auto command_2{ std::make_shared<CommandType>(receiver_2, &Receiver::Add, 40, 50) };
						command_2->Execute();
						std::unique_ptr<Invoker> invoker_2{ std::make_unique<Invoker>() };


						/*Client client{};
						client.set_receiver(receiver);
						client.set_command(command);
						client.set_invoker(std::move(invoker));*/

						ASSERT_EQ(1, 2) << "Test CommandClass";
					}


			} // !namespace command

			namespace interpreter {}
			namespace iterator {}
			namespace mediator {}
			namespace memento {}
			namespace null_object{}
			namespace observer {}
			namespace state {}
			namespace strategy {}
			namespace template_method{}
			namespace visitor {}

		} // !namespace behavioral


		namespace concurrency {
		} // !namespace concurrency


		namespace cpp_idiom {
			namespace attorney_client {}
			namespace copy_n_swap {}
			namespace crtp {}
			namespace d_pointer{}
			namespace named_parameter{}
			namespace non_virtual_interface {}
			namespace opaque_pointer{}
			namespace pimpl {}
		} // !namespace cpp_idiom


		namespace creational {
			namespace abstract_factory{}
			namespace builder {}
			namespace dependency_injection {}
			namespace factory_method {}
			namespace lazy_initialization {}
			namespace object_pool {}
			namespace prototype {}
			namespace singleton {}
		} // !namespace creational


		namespace database {

		} // !namespace database


		namespace distributed {

		} // !namespace distributed


		namespace gamedev {
			namespace bytecode {}
			namespace component {}
			namespace data_locality {}
			namespace dirty_flag {}
			namespace double_buffer {}
			namespace event_queue {}
			namespace game_loop {}
			namespace service_locator {}
			namespace spatial_partition {}
			namespace subclass_sandbox {}
			namespace type_object {}
			namespace update_method {}
		} // !namespace gamedev


		namespace structural {
			namespace adapter {}
			namespace bridge {}
			namespace composite {}
			namespace decorator {}
			namespace delegation {}
			namespace facade {}
			namespace flyweight {}
			namespace marker {}
			namespace proxy {}
		} // !namespace structural


		namespace ui {

		} // !namespace ui

	}  // !pattern
}  // !unnamed namespace
