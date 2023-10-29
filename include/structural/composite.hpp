#ifndef COMPOSITE_HPP
#define COMPOSITE_HPP

#include <memory>
#include <utility>
#include <forward_list>
#include <stdexcept>

/** Software Design Patterns */
namespace pattern {
	namespace structural {
		namespace composite {
			// https://en.wikipedia.org/wiki/Composite_pattern
			// Two variants of realization. 1) Design for Uniformity. 2) Design for Type Safety.
			// The NonTerminalExpression emphasizes uniformity over type safety
			// Part - Whole
			// Motivation: one interface for working with part and whole

			class Client {
			public:
			};

			/** Abstract. */
			class AbstractExpression {
			public:
				virtual ~AbstractExpression() = default;

				virtual void Operation() = 0;

				// Default Operations for TerminalExpression Terminal Component

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

			/*protected:
				AbstractExpression& parent() const;
				void set_parent();

			private:
				std::unique_ptr<AbstractExpression> parent_ptr;*/
			};

			/** Terminal Component of Abstract Tree */
			class TerminalExpression : public AbstractExpression {
			public:
				void Operation() override {
					ProcessLeaf();
				};

			private:
				void ProcessLeaf() {
				};
			};

			/** Non Terminal Component of Abstract Tree */
			class NonTerminalExpression : public AbstractExpression {
			public:
				void Operation() override {
					for (auto& component : composite_) { // component is non const
						component->Operation();
					}
				};

				const AbstractExpression* GetChild() const noexcept override { return this; };
				void AddChild(AbstractExpression&) override {};
				void RemoveChild(AbstractExpression&) override {};

			private:
				std::forward_list<std::unique_ptr<AbstractExpression>> composite_;
			};

		} // !namespace composite

	} // !namespace structural
} // !namespace pattern

#endif // !COMPOSITE_HPP
