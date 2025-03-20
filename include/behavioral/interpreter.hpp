#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include <memory>
#include <utility>
#include <forward_list>
#include <stdexcept>

/** Software Design Patterns */
namespace pattern {
	namespace behavioral {
		namespace interpreter {
			// https://en.wikipedia.org/wiki/Interpreter_pattern
			// Common patterns: NonTerminalExpression, Flyweight, Iterator, Visitor


			class InterpreterError : public std::runtime_error {
            public:
				InterpreterError(const char* const error_message_p = nullptr) noexcept
					: std::runtime_error{ error_message_p } {};
            };

			class Client {
			public:
			};

			/** Global State of Interpreter */
			struct Context {
				std::string input_text_{};
				size_t process_position_{};
			};

			/** Abstract. */
			class AbstractComponent {
			protected:
				AbstractComponent() = default;
				AbstractComponent(const AbstractComponent&) = delete; // C.67	C.21
				AbstractComponent& operator=(const AbstractComponent&) = delete;
				AbstractComponent(AbstractComponent&&) noexcept = delete;
				AbstractComponent& operator=(AbstractComponent&&) noexcept = delete;
			public:
				virtual ~AbstractComponent() = default;


				virtual void Interpret(Context& context) = 0;

				// set parent_ptr in AddChild & RemoveChild
				virtual const AbstractComponent* GetChild() const noexcept { return nullptr; };

				virtual void AddChild(AbstractComponent&) {
					if (GetChild()) {
						// Add Child
					} else {
						throw InterpreterError("AddChild");
					}
				};

				virtual void RemoveChild(AbstractComponent&) {
					if (GetChild()) {
						// Remove Child
					} else {
						throw InterpreterError("RemoveChild");
					}
				};
			};

			/** Terminal Expression of Language Abstract Tree */
			class TerminalComponent : public AbstractComponent {
			public:
				void Interpret(Context& context) override {
					ProcessLeaf();
				};

			private:
				void ProcessLeaf() {
				};
			};

			/** Non Terminal Expression of Language Abstract Tree */
			class NonTerminalComponent : public AbstractComponent {
			public:
				void Interpret(Context& context) override {
					for (auto& expression : child_expressions_) {
						expression->Interpret(context);
					}
				};

				const AbstractComponent* GetChild() const noexcept override { return this; };
				void AddChild(AbstractComponent&) override {};
				void RemoveChild(AbstractComponent&) override {};

			private:
				std::forward_list<std::unique_ptr<AbstractComponent>> child_expressions_{};
			};

		} // !namespace interpreter

	} // !namespace behavioral
} // !namespace pattern

#endif // !INTERPRETER_HPP
