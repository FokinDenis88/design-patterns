#ifndef DECORATOR_HPP
#define DECORATOR_HPP

#include <memory>
#include <utility>

/** Software Design Patterns */
namespace pattern {
	namespace structural {
		namespace decorator {
			// https://en.wikipedia.org/wiki/Decorator_pattern

			class AbstractComponent {
			protected:
				AbstractComponent() = default;
				AbstractComponent(const AbstractComponent&) = delete; // C.67	C.21
				AbstractComponent& operator=(const AbstractComponent&) = delete;
				AbstractComponent(AbstractComponent&&) noexcept = delete;
				AbstractComponent& operator=(AbstractComponent&&) noexcept = delete;
			public:
				virtual ~AbstractComponent() = default;

				virtual void Operation1() = 0;
				virtual void Operation2() = 0;
			};

			class ConcreteComponent : public AbstractComponent {
			public:
				void Operation1() override {};
				void Operation2() override {};
			};

			/** Abstract Decorator. Pure Virtual. */
			class DecoratorAbstract : public AbstractComponent {
			protected:
				DecoratorAbstract() = default;
				DecoratorAbstract(const DecoratorAbstract&) = delete; // C.67	C.21
				DecoratorAbstract& operator=(const DecoratorAbstract&) = delete;
				DecoratorAbstract(DecoratorAbstract&&) noexcept = default;	// for move unique_ptr
				DecoratorAbstract& operator=(DecoratorAbstract&&) noexcept = delete;
			public:
				~DecoratorAbstract() override = default;

				explicit DecoratorAbstract(std::unique_ptr<AbstractComponent>&& component_p) noexcept
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
				std::unique_ptr<AbstractComponent> component_{};
			};

			class ConcreteDecoratorA final: public DecoratorAbstract {
			public:
				ConcreteDecoratorA() = delete;
				ConcreteDecoratorA(const ConcreteDecoratorA&) = delete;
				ConcreteDecoratorA& operator=(const ConcreteDecoratorA&) = delete;
				ConcreteDecoratorA(ConcreteDecoratorA&&) noexcept = default;
				ConcreteDecoratorA& operator=(ConcreteDecoratorA&&) noexcept = default;

				explicit ConcreteDecoratorA(std::unique_ptr<AbstractComponent>&& component_p, int a_p) noexcept
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
				ConcreteDecoratorB(ConcreteDecoratorB&&) noexcept = default;
				ConcreteDecoratorB& operator=(ConcreteDecoratorB&&) noexcept = default;

				explicit ConcreteDecoratorB(std::unique_ptr<AbstractComponent>&& component_p, int b_p) noexcept
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

		} // !namespace decorator

	} // !namespace structural
} // !namespace pattern

#endif // !DECORATOR_HPP
