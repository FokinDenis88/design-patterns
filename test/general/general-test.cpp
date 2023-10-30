#include "gtest/gtest.h"

#include "header-collection/all-headers.hpp"


//#include <iostream>
//#include <functional>


namespace {
	namespace pattern {
		namespace ai {} // !namespace ai
		namespace anti_pattern{} // !namespace anti_pattern

		namespace architectural {
			namespace mvc {

			} // !namespace mvc
		} // namespace architectural


		namespace behavioral {
			namespace chain_of_responsibility {
			} // !namespace chain_of_responsibility

			namespace command {
					using namespace ::pattern::behavioral::command;

					TEST(CommandTest, CommandClass) {
						//TestClass<decltype(Receiver::Add)> test{};
						//int a = 66 - 44;

						//std::function<CommandSTDFunction::ActionType> func_object = std::bind(&Receiver::Add, receiver.get(), 40, 50);

						std::shared_ptr<Receiver> receiver{ std::make_shared<Receiver>() };
						auto command{ std::make_shared<Command>(&Receiver::Add, *receiver, 40, 50) };
						std::unique_ptr<Invoker> invoker{ std::make_unique<Invoker>() };

						/*Client client{};
						client.set_receiver(receiver);
						client.set_command(command);
						client.set_invoker(std::move(invoker));*/

						//std::cout << "Add_result: " << client.InvokeCommand() << '\n';

						//ASSERT_EQ(1, 2) << "Test CommandSTDFunctionClass";
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
						using CommandType = CommandMemberFn<Receiver, int, int, int>;
						auto command_2{ std::make_shared<CommandType>(receiver_2, &Receiver::Add, 40, 50) };
						command_2->Execute();
						std::unique_ptr<Invoker> invoker_2{ std::make_unique<Invoker>() };


						/*Client client{};
						client.set_receiver(receiver);
						client.set_command(command);
						client.set_invoker(std::move(invoker));*/

						//ASSERT_EQ(1, 2) << "Test CommandClass";
					}

					TEST(CommandTest, MacroCommandClass) {
						MacroCommand macro_cmd{};

						//MacroCommandStateless macro_stateless{};
						//MacroCommandStateful<CommandSTDFunction> macro_stateful{};
					}

			} // !namespace command

			namespace interpreter {}
			namespace iterator {}
			namespace mediator {}
			namespace memento {
				namespace memento_simple {
					using namespace ::pattern::behavioral::memento_simple;
					TEST(MementoTest, MementoSimpleClass) {
						CareTaker care_take{};
						Originator originator{};
						care_take.set_memento(originator.CreateMemento());
						const State new_state{ 99, 99 };
						originator.set_state(new_state);
						originator.Restore(care_take.memento());
					};
				}

				namespace memento_templated {
					using namespace ::pattern::behavioral::memento_templated;
					TEST(MementoTest, MementoTemplatedClass) {
						using MyOriginatorType = Originator<State>;

						CareTaker<MyOriginatorType> care_take{};
						Originator<State> originator{};
						care_take.set_memento(originator.CreateMemento());
						const State new_state{ 99, 99 };
						originator.set_state(new_state);
						originator.Restore(care_take.memento());

						int a = 66 - 33;
					};
				}
			} // !namespace memento

			namespace null_object{}
			namespace observer {
				namespace observer_ref {
					using namespace ::pattern::behavioral::observer_ref;
					TEST(ObserverTest, ObserverRefClass) {
						//=========Ref============
						SubjectRef subject_ref{};
						ObserverRef observer_ref_1{};
						ObserverRef observer_ref_2{};
						ObserverRef observer_ref_3{};
						subject_ref.AttachObserver(observer_ref_1);
						subject_ref.AttachObserver(observer_ref_2);
						subject_ref.AttachObserver(observer_ref_3);
						subject_ref.set_state(State{ 77, 77 });
						subject_ref.Notify();
						subject_ref.DetachObserver(observer_ref_1);
						subject_ref.ClearAllObservers();

						int a = 66 - 66;
					};
				} // !namespace observer_ref

				namespace observer_smart_ptr {
					using namespace ::pattern::behavioral::observer_smart_ptr;
					TEST(ObserverTest, ObserverSmartPtrSimpleClass) {
						//======Simple============
						Subject subject{};
						std::shared_ptr<IObserver> observer_1{ std::make_shared<Observer>() };
						std::shared_ptr<IObserver> observer_2{ std::make_shared<Observer>() };
						std::shared_ptr<IObserver> observer_3{ std::make_shared<Observer>() };
						subject.AttachObserver(observer_1);
						subject.AttachObserver(observer_2);
						subject.AttachObserver(observer_3);
						subject.set_state(State{ 77, 77 });
						subject.Notify();
						subject.DetachObserver(observer_1);
						subject.ClearAllObservers();
					}

					TEST(ObserverTest, ObserverSmartPtrExtendedClass) {
						//=======Extended==========
						/*SubjectExtended subject_ext{};
						std::shared_ptr<IObserver> observer_1_ext{ std::make_shared<ObserverExtended>() };
						std::shared_ptr<IObserver> observer_2_ext{ std::make_shared<ObserverExtended>() };
						std::shared_ptr<IObserver> observer_3_ext{ std::make_shared<ObserverExtended>() };
						subject_ext.AttachObserver(observer_1_ext);
						subject_ext.AttachObserver(observer_2_ext);
						subject_ext.AttachObserver(observer_3_ext);
						subject_ext.set_state(State{ 77, 77 });
						subject_ext.Notify();
						subject_ext.DetachObserver(observer_1_ext);
						subject_ext.ClearAllObservers();*/

						int a = 66 - 66;
					};
				} // !namespace observer_smart_ptr
			}
			namespace state {
				using namespace ::pattern::behavioral::observer_state_;
				TEST(StateTest, ObserverStateClass) {

				};
			}
			namespace strategy {
				using namespace ::pattern::behavioral::strategy;
				TEST(StrategyTest, StrategyClass) {
					MyTestClass object{};
					object.strategy_ = std::make_unique<StrategyConcrete>();
					object.strategy_->Algorithm();
				};
			}
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
			namespace pimpl {
				namespace pimpl_1 {
					using namespace ::pattern::cpp_idiom::pimpl_1;
					TEST(PimplTest, Pimpl1) {
						Widget widget{};
					};
				}
				namespace pimpl_2 {
					using namespace ::pattern::cpp_idiom::pimpl_2;
					TEST(PimplTest, Pimpl2) {
						Widget widget{};
					};
				}
			}
		} // !namespace cpp_idiom


		namespace creational {
			namespace abstract_factory{}
			namespace builder {}
			namespace dependency_injection {}
			namespace factory_method {}
			namespace lazy_initialization {}
			namespace object_pool {
				using namespace ::pattern::creational::object_pool;
				TEST(ObjectPoolTest, ObjectPoolClass) {
					/*auto pool{ ObjectPool::GetInstance() };
					auto r{ pool.GetResourceFromPool() };
					if (auto ptr{ r.lock() }) {
						dynamic_cast<ObjectPoolResource&>(*ptr).SetValue(77);
					} else {
						std::cout << "resource expired\n";
					}*/
					//pool.ReturnResourceToPool(std::move(r));
					int a = 0;

					int* a_origin_ptr{ new int() };
					//std::unique_ptr<int> a_ptr{ std::make_unique<int>() };
					//std::unique_ptr<int> a_ptr(a_origin_ptr);
					std::shared_ptr<int> a_ptr(a_origin_ptr);
					std::weak_ptr<int> a_weak_ptr(a_ptr);

					//delete a_ptr.get();
					delete a_origin_ptr;

					bool flag{};
					if (a_ptr) {
						flag = true;
						std::cout << "owned object alive\n";
					}
					else {
						flag = false;
						std::cout << "owned object dead\n";
					}

					std::cout << "Weak ptr:\n";
					if (a_weak_ptr.expired()) {
						std::cout << "owned object dead\n";
					}
					else {
						std::cout << "owned object alive\n";
					}

					if (a_origin_ptr == nullptr) {
						std::cout << "origin ptr is nullptr\n";
					}
					else {
						std::cout << "origin ptr is not nullptr\n";
					}

					std::shared_ptr<int> test_a{ std::make_shared<int>(66) };
					std::shared_ptr<int> test_b{ test_a };
					test_a.reset(new int(10));
					if (test_b) {
						std::cout << "test_b ptr is nullptr\n";
					}
					else {
						std::cout << "test_b ptr is not nullptr\n";
					}
				};
			} // !namespace object_pool
			namespace prototype {
				using namespace ::pattern::creational::prototype;
				TEST(PrototypeTest, PrototypeClass) {
					Client client{};
					client.creator_ = std::make_unique<PrototypeA>();
					client.creator_->Clone();
				};
			}
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
			namespace decorator {
				using namespace ::pattern::structural::decorator;
				TEST(DecoratorTest, DecoratorClass) {
					int a{ 33 };
					int b{ 66 };

					/*ConcreteComponent concrete_component{};
					ConcreteDecoratorA decorator_a{ ConcreteDecoratorA(std::make_unique<AbstractExpression>(concrete_component), a) };
					DecoratorAbstract& i_decorator = decorator_a;
					ConcreteDecoratorB decorator_b{ ConcreteDecoratorB(std::make_unique<AbstractExpression>(decorator_a), b) };*/

					std::unique_ptr<AbstractExpression> my_component = std::make_unique<ConcreteComponent>();
					my_component = std::make_unique<ConcreteDecoratorA>(std::move(my_component), a);
					my_component = std::make_unique<ConcreteDecoratorB>(std::move(my_component), b);
				};
			}
			namespace delegation {}
			namespace facade {}
			namespace flyweight {
				using namespace ::pattern::structural::flyweight;
				TEST(FlyweightTest, FlyweightClass) {
					FlyweightFabric<int, FlyweightShared, int> flyweight_fabric{};
					{
						std::shared_ptr<FlyweightShared> my_flyweight{ flyweight_fabric.GetFlyweight(0) };
						std::shared_ptr<FlyweightShared> my_flyweight_2{ flyweight_fabric.GetFlyweight(0) };
						int b = 1 + 66;
					}
					int a = 1 + 66;
				};
			}
			namespace marker {}
			namespace proxy {}
		} // !namespace structural


		namespace ui {

		} // !namespace ui

	}  // !pattern
}  // !unnamed namespace
