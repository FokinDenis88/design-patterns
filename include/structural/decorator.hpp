#ifndef DECORATOR_HPP
#define DECORATOR_HPP

#include <memory>
#include <utility>

/** Software Design Patterns */
namespace pattern {
	namespace structural {
		namespace decorator {
			// https://en.wikipedia.org/wiki/Decorator_pattern

			class AbstractExpression {
			public:
				virtual ~AbstractExpression() = default;

				virtual void Operation1() = 0;
				virtual void Operation2() = 0;
			};

			class ConcreteComponent : public AbstractExpression {
			public:
				void Operation1() override {};
				void Operation2() override {};
			};

			/** Abstract Decorator. Pure Virtual. */
			class DecoratorAbstract : public AbstractExpression {
			public:
				DecoratorAbstract() = delete;
				DecoratorAbstract(const DecoratorAbstract&) = delete;
				DecoratorAbstract& operator=(const DecoratorAbstract&) = delete;

				~DecoratorAbstract() override = default;

				explicit DecoratorAbstract(std::unique_ptr<AbstractExpression>&& component_p) noexcept
					: component_{ std::move(component_p) } {
				}

				/* AbstractExpression Functions can be pure virtual or can have default behavior */
				/*
				void Operation1() override {
					component_->Operation1();
				};
				void Operation2() override {
					component_->Operation2();
				};
				*/
			protected:
				inline void ComponentOperation1() {
					if (component_) {
						component_->Operation1();
					}
				};
				inline void ComponentOperation2() {
					if (component_) {
						component_->Operation2();
					}
				};
				//inline std::unique_ptr<AbstractExpression>& GetComponent() noexcept { return component_; };

			private:
				std::unique_ptr<AbstractExpression> component_{};
			};

			class ConcreteDecoratorA final: public DecoratorAbstract {
			public:
				ConcreteDecoratorA() = delete;
				ConcreteDecoratorA(const ConcreteDecoratorA&) = delete;
				ConcreteDecoratorA& operator=(const ConcreteDecoratorA&) = delete;

				explicit ConcreteDecoratorA(std::unique_ptr<AbstractExpression>&& component_p, int a_p) noexcept
					:	DecoratorAbstract(std::move(component_p)),
						a_{ a_p } {
				}

				void Operation1() override {
					// Can add behavior after and/or before delegation
					// MyOperation1();
					ComponentOperation1();
					MyOperation1();
				};
				void Operation2() override {
					MyOperation2();
					ComponentOperation2();
				};

			private:
				void MyOperation1() {};
				void MyOperation2() {};

				int a_{};
			};

			/** Decorator Derived from other Decorator */
			class ConcreteDecoratorB final: public DecoratorAbstract {
			public:
				ConcreteDecoratorB() = delete;
				ConcreteDecoratorB(const ConcreteDecoratorB&) = delete;
				ConcreteDecoratorB& operator=(const ConcreteDecoratorB&) = delete;

				explicit ConcreteDecoratorB(std::unique_ptr<AbstractExpression>&& component_p, int b_p) noexcept
					:	DecoratorAbstract(std::move(component_p)),
						b_{ b_p } {
				}

				void Operation1() override {
					ComponentOperation1();
					MyNewOperation1();
				};
				void Operation2() override {
					MyNewOperation2();
					ComponentOperation2();
				};
			private:
				void MyNewOperation1() {};
				void MyNewOperation2() {};

				int b_{};
			};

			inline void Run() {
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

		} // !namespace decorator
	} // !namespace structural
} // !namespace pattern

#endif // !DECORATOR_HPP
