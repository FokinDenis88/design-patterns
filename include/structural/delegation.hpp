#ifndef DELEGATION_HPP
#define DELEGATION_HPP

#include <memory>
#include <utility>

/** Software Design Patterns */
namespace pattern {
	namespace structural {
		namespace delegation {
			// https://en.wikipedia.org/wiki/Delegation_pattern
			// https://www.geeksforgeeks.org/object-delegation-in-cpp/
			// https://www.bogotobogo.com/DesignPatterns/delegation.php

			// The main idea is to make composition as powerful, as inheritance.

			class Delegator;

			/** Abstract. */
			class IDelegatee {
			protected:
				IDelegatee() = default;
				IDelegatee(const IDelegatee&) = delete; // C.67	C.21
				IDelegatee& operator=(const IDelegatee&) = delete;
				IDelegatee(IDelegatee&&) noexcept = delete;
				IDelegatee& operator=(IDelegatee&&) noexcept = delete;
			public:
				virtual ~IDelegatee() = default;

				virtual void Foo(Delegator& object_p) const = 0;
			};

			/** Function call From Delegator to Delegatee. Class that function from delegate */
			class Delegator final {
			public:
				//friend IDelegatee;

				Delegator() = delete;
				Delegator(const Delegator&) = delete;
				Delegator& operator=(Delegator&) = delete;

				Delegator(Delegator&&) noexcept = default;
				Delegator& operator=(Delegator&&) noexcept = default;

				explicit Delegator(std::unique_ptr<IDelegatee>&& delegate_p) noexcept
					: delegate_{ std::move(delegate_p) } {
				};

				void MainFunc() {
					delegate_->Foo(*this);
				};

				inline void set_a(int a_p) { a_ = a_p; };
				inline void set_b(int b_p) { b_ = b_p; };

				inline int get_a() const { return a_; };
				inline int get_b() const { return b_; };
			private:
				std::unique_ptr<IDelegatee> delegate_;

				int a_{};
				int b_{};
			};

			class ConcreteDelegatee : public IDelegatee {
			public:
				void Foo(Delegator& object_p) const override {
					object_p.set_a(66);
					//object_p.b_ = 77;
				};
			};

		} // !namespace delegation

	} // !namespace structural
} // !namespace pattern

#endif // !DELEGATION_HPP
