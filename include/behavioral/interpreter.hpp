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

			class Client {
			public:
			};

			/** Global State of Interpreter */
			struct Context {
				std::string input_text_{};
				size_t process_position_{};
			};

			/** Abstract. */
			class AbstractExpression {
			public:
				virtual ~AbstractExpression() = default;

				virtual void Interpret(Context& context) = 0;

				// set parent_ptr in AddChild & RemoveChild
				virtual const AbstractExpression* GetChild() const noexcept { return nullptr; };

				virtual void AddChild(AbstractExpression&) {
					if (GetChild()) {
						// Add Child
					} else {
						throw std::runtime_error("AddChild");
					}
				};

				virtual void RemoveChild(AbstractExpression&) {
					if (GetChild()) {
						// Remove Child
					} else {
						throw std::runtime_error("RemoveChild");
					}
				};
			};

			/** Terminal Expression of Language Abstract Tree */
			class TerminalExpression : public AbstractExpression {
			public:
				void Interpret(Context& context) override {
					ProcessLeaf();
				};

			private:
				void ProcessLeaf() {
				};
			};

			/** Non Terminal Expression of Language Abstract Tree */
			class NonTerminalExpression : public AbstractExpression {
			public:
				void Interpret(Context& context) override {
					for (auto& expression : child_expressions_) {
						expression->Interpret(context);
					}
				};

				const AbstractExpression* GetChild() const noexcept override { return this; };
				void AddChild(AbstractExpression&) override {};
				void RemoveChild(AbstractExpression&) override {};

			private:
				std::forward_list<std::unique_ptr<AbstractExpression>> child_expressions_{};
			};

			inline void Run() {

			};

		} // !namespace interpreter
	} // !namespace behavioral
} // !namespace pattern

#endif // !INTERPRETER_HPP
