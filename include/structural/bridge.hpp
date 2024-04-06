#ifndef BRIDGE_HPP
#define BRIDGE_HPP

#include <memory>
#include <utility>

/** Software Design Patterns */
namespace pattern {
	namespace structural {
		namespace bridge {
			// https://en.wikipedia.org/wiki/Bridge_pattern

			/** Abstract. */
			class IImplementor {
			protected:
				IImplementor() = default;
				IImplementor(const IImplementor&) = default; // C.67	C.21
				IImplementor& operator=(const IImplementor&) = default;
				IImplementor(IImplementor&&) noexcept = default;
				IImplementor& operator=(IImplementor&&) noexcept = default;
			public:
				virtual ~IImplementor() = default;


				virtual void OperationImpl() = 0;
			};

			class ConcreteImplementor : public IImplementor {
			public:
				void OperationImpl() override {
				};
			};

			/** Abstract. */
			class Abstraction {
			protected:
				Abstraction() = delete;
				Abstraction(const Abstraction&) = delete; // C.67	C.21
				Abstraction& operator=(const Abstraction&) = delete;
				Abstraction(Abstraction&&) noexcept = default;	// for move unique_ptr
				Abstraction& operator=(Abstraction&&) noexcept = default;
			public:
				virtual ~Abstraction() = default;

				using IImplementorPtr = std::unique_ptr<IImplementor>;

				explicit Abstraction(IImplementorPtr&& implementor_p) noexcept
					: implementor_{ std::move(implementor_p) } {
				};


				virtual void Operation(IImplementorPtr&& implementor_p = nullptr) = 0;

				/*inline const std::unique_ptr<IImplementor>& GetConstImplementor() const noexcept { return implementor_; };
				inline void SetImplementor(std::unique_ptr<IImplementor>&& implementor_p) noexcept {
					implementor_ = std::move(implementor_p);
				};*/

			protected:
				/*inline IImplementorPtr& implementor_ref() noexcept { return implementor_; };
				inline void set_implementor(IImplementorPtr&& implementor_p) noexcept {
					implementor_ = std::move(implementor_p);
				};*/
				void OperationImpl(IImplementorPtr&& implementor_p = nullptr) {
					if (implementor_p) {
						implementor_ = std::move(implementor_p);
					}
					if (implementor_) {
						implementor_->OperationImpl();
					}
				}

			private:
				IImplementorPtr implementor_;
			};

			class ConcreteAbstraction : public Abstraction {
			public:
				void Operation(IImplementorPtr&& implementor_p = nullptr) override {
					OperationImpl(std::move(implementor_p));
				};

			};

		} // !namespace bridge

	} // !namespace structural
} // !namespace pattern

#endif // !BRIDGE_HPP
