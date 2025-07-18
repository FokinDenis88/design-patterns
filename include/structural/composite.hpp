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
			// Two variants of realization. 1) Design for Uniformity. 2) Design for T Safety.
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
			public:
                using AbstractComponentPtr = std::unique_ptr<AbstractComponent>;
                using ContainerT = std::forward_list<AbstractComponentPtr>;

			protected:
				AbstractComponent() = default;
				AbstractComponent(const AbstractComponent&) = delete; // C.67	C.21
				AbstractComponent& operator=(const AbstractComponent&) = delete;
				AbstractComponent(AbstractComponent&&) noexcept = delete;
				AbstractComponent& operator=(AbstractComponent&&) noexcept = delete;
			public:
				virtual ~AbstractComponent() = default;

				/**
				 * Main operation for Component.
				 * F.e. in Interpreter pattern it is named interpret(context).
				 */
				virtual void Operation() = 0;


			//============Default Operation for Leaf TerminalComponent===============================

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

                /** Default for TerminalComponent. Returns const this pointer. Must not be smart, cause const. */
                virtual inline const ContainerT* GetChildren() const noexcept { return nullptr; };

			protected:
                /** Default for TerminalComponent. Checks, if Component is Composite, that can include other components. */
                virtual constexpr bool IsNonTerminal() const noexcept { return false; };


			/*protected:
				AbstractExpression& parent() const;
				void set_parent();
			private:
				// set parent_ptr in AddChild & RemoveChild
				std::unique_ptr<AbstractExpression> parent_ptr;
				// TODO: std::reference_wrapper???*/

			}; // !class AbstractComponent


			/**
			 * Leaf. Terminal Component of Abstract Tree.
			 * Can't include other Components inside of itself.
			 */
			class TerminalComponent : public AbstractComponent {
            protected:
                TerminalComponent() = default;
                TerminalComponent(const TerminalComponent&) = delete; // C.67	C.21
				TerminalComponent& operator=(const TerminalComponent&) = delete;
				TerminalComponent(TerminalComponent&&) noexcept = delete;
				TerminalComponent& operator=(TerminalComponent&&) noexcept = delete;
            public:
                ~TerminalComponent() override = default;

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
			protected:
				NonTerminalComponent() = default;
				NonTerminalComponent(const NonTerminalComponent&) = delete;	// cause list of unique_ptr can be only moved
				NonTerminalComponent& operator=(const NonTerminalComponent&) = delete;

				NonTerminalComponent(NonTerminalComponent&&) noexcept = default;
				NonTerminalComponent& operator=(NonTerminalComponent&&) noexcept = default;
			public:
				~NonTerminalComponent() override = default;

				void Operation() override {
					for (auto& component_ptr : components_ptrs) { // component is non const Operation can mutate component
						if (component_ptr){ component_ptr->Operation(); }
					}
				};

				void AddChild(AbstractComponentPtr&& new_component_ptr) override {
					components_ptrs.emplace_front(std::move(new_component_ptr));
				}

				void RemoveChild(const AbstractComponent& new_component) override {
					components_ptrs.remove_if([&new_component](const AbstractComponentPtr& elem_ptr) {
							if (elem_ptr && (elem_ptr.get() == &new_component)) {
								return true;
							}
							return false;
						});
				};

                /** Returns const this pointer. Must not be smart, cause const. */
                inline const ContainerT* GetChildren() const noexcept override { return &components_ptrs; };

            protected:
                /** Default for TerminalComponent. Checks, if Component is Composite, that can include other components. */
                constexpr bool IsNonTerminal() const noexcept override { return true; };

			private:
				/** Pointers to childern components of Composite. */
				ContainerT components_ptrs{};
			};

		} // !namespace composite

	} // !namespace structural
} // !namespace pattern

#endif // !COMPOSITE_HPP
