#include "gtest/gtest.h"

#include "header-collection/all-headers.hpp"


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
						std::function<void()> a{};
						auto b = []() {};
						std::cout << "std::function " << sizeof(a) << "\n";
						std::cout << "lambda " << sizeof(b) << "\n";

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

					TEST(CommandTest, CommandMemberFnClass) {

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
			namespace crtp {
				using namespace ::pattern::cpp_idiom::crtp;
				TEST(CRTPTest, BaseDerived) {
					Base<Derived>* base_ptr{};
					base_ptr->Foo(13);
					//EXPECT_EQ(pool.SizeAllocatedResources(), 0);
				}
			}
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
					auto& pool{ ObjectPool<ObjectPoolResource, false>::GetInstance() };
					pool.set_new_objects_limit(10);
					EXPECT_EQ(pool.SizeAllocatedResources(), 0);
					EXPECT_EQ(pool.new_objects_limit(), 10);
					EXPECT_EQ(pool.SizeMaxAvailableResources(), 10);

					auto resource{ pool.GetResourceFromPool() };
					std::cout << "resource == nullptr: " << (resource == nullptr) << " sizeof(resource): " << sizeof(resource) << '\n';
					std::cout << "SizeAllocatedResources: " << pool.SizeAllocatedResources() <<
								" SizeMaxAvailableResources: " << pool.SizeMaxAvailableResources() << '\n';
					EXPECT_EQ(pool.SizeAllocatedResources(), 0);
					EXPECT_EQ(pool.new_objects_limit(), 9);
					EXPECT_EQ(pool.SizeMaxAvailableResources(), 9);


					std::cout << "resource.reset: ";
					resource.reset();
					std::cout << "SizeAllocatedResources: " << pool.SizeAllocatedResources() <<
								" SizeMaxAvailableResources: " << pool.SizeMaxAvailableResources() << '\n';
					EXPECT_EQ(pool.SizeAllocatedResources(), 1);
					EXPECT_EQ(pool.new_objects_limit(), 9);
					EXPECT_EQ(pool.SizeMaxAvailableResources(), 10) << " Hello World\n";
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


		/** https://google.github.io/googletest/primer.html */
		namespace help {
			// Info
			// Test Suite, Test Case
			// TEST(ObjectPoolTest, ObjectPoolClass) {}
			// Fixture class for common object for some Test Cases

			struct Foo {
				int a{};
			};

			// The fixture for testing class Foo.
			class FooTest : public testing::Test {
			protected:
				// You can remove any or all of the following functions if their bodies would
				// be empty.

				FooTest() {
					// You can do set-up work for each test here.
				}

				~FooTest() override {
					// You can do clean-up work that doesn't throw exceptions here.
				}

				// If the constructor and destructor are not enough for setting up
				// and cleaning up each test, you can define the following methods:

				void SetUp() override {
					// Code here will be called immediately after the constructor (right
					// before each test).
				}

				void TearDown() override {
					// Code here will be called immediately after each test (right
					// before the destructor).
				}

				// Class members declared here can be used by all tests in the test suite
				// for Foo.
			};

			// Tests that the Foo::Bar() method does Abc.
			TEST_F(FooTest, MethodBarDoesAbc) {
				const std::string input_filepath = "this/package/testdata/myinputfile.dat";
				const std::string output_filepath = "this/package/testdata/myoutputfile.dat";
				Foo f;
				//EXPECT_EQ(f.Bar(input_filepath, output_filepath), 0);
			}
		} // !namespace help

	}  // !pattern
}  // !unnamed namespace
