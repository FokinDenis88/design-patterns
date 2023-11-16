#ifndef COMPOSITE_HPP
#define COMPOSITE_HPP

#include <memory>
#include <utility>
#include <forward_list>

#include <stdexcept>
#include <string_view>


/** Software Design Patterns */
namespace pattern {
	namespace structural {
		namespace composite {
			// https://en.wikipedia.org/wiki/Composite_pattern
			// Two variants of realization. 1) Design for Uniformity. 2) Design for Type Safety.
			// The NonTerminalExpression emphasizes uniformity over type safety
			// Part - Whole
			// Motivation: one interface for working with part and whole

			/**
			 * Realization:
			 * 1) Link to parents help to iterate composite and help in realization chain of responsibility pattern.
			 * Invariant: change parent reference only when you add or remove component.
			 * 2) Cache can increase performance.
			 */


			class ErrorTerminalChildOperation : public std::runtime_error {
			public:
				using std::runtime_error::runtime_error;
			};


			class Client {
			public:
			};

			/** Abstract. To create Terminal Component just override Operation. */
			class AbstractComponent {
			protected:
				AbstractComponent() = default;
				AbstractComponent(const AbstractComponent&) = delete; // C.67	C.21
				AbstractComponent& operator=(const AbstractComponent&) = delete;
				AbstractComponent(AbstractComponent&&) noexcept = delete;
				AbstractComponent& operator=(AbstractComponent&&) noexcept = delete;
			public:
				virtual ~AbstractComponent() = default;

				using AbstractComponentPtr = std::unique_ptr<AbstractComponent>;
				using ContainerType = std::forward_list<AbstractComponentPtr>;


				/**
				 * Main operation for Component.
				 * F.e. in Interpreter pattern it is named interpret(context).
				 */
				virtual void Operation() = 0;


			//============Default Operation for TerminalComponent===============================

				/** Default for TerminalComponent. Returns const this pointer. Must not be smart, cause const. */
				virtual inline const ContainerType* GetChildren() const noexcept { return nullptr; };

				/** Default for TerminalComponent. Checks, if Component is Composite, that can include other components. */
				virtual constexpr bool IsNonTerminal() const noexcept { return false; };

				/** Default for TerminalComponent. */
				virtual inline void AddChild(AbstractComponentPtr&& new_component) {
					constexpr std::string_view kErrorMsgAddChild{ "Error: Try AddChild in Terminal Expression" };
					if (!IsNonTerminal()) {
						throw ErrorTerminalChildOperation(kErrorMsgAddChild.data());
					}
				}

				/** Default for TerminalComponent. */
				virtual inline void RemoveChild(const AbstractComponent& new_component) {
					constexpr std::string_view kErrorMsgRemoveChild{ "Error: Try RemoveChild in Terminal Expression" };
					if (!IsNonTerminal()) {
						throw ErrorTerminalChildOperation(kErrorMsgRemoveChild.data());
					}
				};

			/*protected:
				AbstractExpression& parent() const;
				void set_parent();
			private:
				// set parent_ptr in AddChild & RemoveChild
				std::unique_ptr<AbstractExpression> parent_ptr;
				// TODO: std::reference_wrapper???*/
			};


			/**
			 * Leaf. Terminal Component of Abstract Tree.
			 * Can't include other Components inside of itself.
			 */
			class TerminalComponent : public AbstractComponent {
			public:
				void Operation() override {
					ProcessLeaf();
				};

			private:
				void ProcessLeaf() {
				};
			};

			/**
			 * Composite. Non Terminal Component of Abstract Tree.
			 * Can include other Components inside of itself.
			 */
			class NonTerminalComponent : public AbstractComponent {
			public:
				NonTerminalComponent() = default;
				NonTerminalComponent(const NonTerminalComponent&) = delete;	// cause list of unique_ptr can be only moved
				NonTerminalComponent& operator=(const NonTerminalComponent&) = delete;

				NonTerminalComponent(NonTerminalComponent&&) noexcept = default;
				NonTerminalComponent& operator=(NonTerminalComponent&&) noexcept = default;
				// TODO: virtual constructors in interface?


				void Operation() override {
					for (auto& component_ptr : components_ptrs) { // component is non const Operation can mutate component
						if (component_ptr){ component_ptr->Operation(); }
					}
				};


				/** Returns const this pointer. Must not be smart, cause const. */
				inline const ContainerType* GetChildren() const noexcept override { return &components_ptrs; };

				/** Default for TerminalComponent. Checks, if Component is Composite, that can include other components. */
				constexpr bool IsNonTerminal() const noexcept override { return true; };

				void AddChild(AbstractComponentPtr&& new_component_ptr) override {
					components_ptrs.emplace_front(std::move(new_component_ptr));
				}

				void RemoveChild(const AbstractComponent& new_component) override {
					components_ptrs.remove_if([&new_component](const AbstractComponentPtr& elem_ptr) {
							if (elem_ptr && (elem_ptr.get() == &new_component)) {
								return true;
							}
							else { return false; }
						});
				};

			private:
				/** Pointers to childern components of Composite. */
				ContainerType components_ptrs{};
			};

		} // !namespace composite

	} // !namespace structural
} // !namespace pattern

#endif // !COMPOSITE_HPP
