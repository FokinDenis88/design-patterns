#ifndef CHAIN_OF_RESPONSIBILITY_HPP
#define CHAIN_OF_RESPONSIBILITY_HPP

#include <memory>
#include <utility>

/** Software Design Patterns */
namespace pattern {
	namespace behavioral {
		namespace chain_of_responsibility {
			// https://en.wikipedia.org/wiki/Chain-of-responsibility_pattern

			/** Client code */
			class Sender {
			public:
			};

			/** Abstract. */
			class HandlerAbstract {
			public:
				HandlerAbstract() = delete;
				HandlerAbstract(const HandlerAbstract&) = delete;
				HandlerAbstract& operator=(HandlerAbstract&) = delete;

				HandlerAbstract(HandlerAbstract&&) noexcept = default;
				HandlerAbstract& operator=(HandlerAbstract&&) noexcept = default;

				explicit HandlerAbstract(std::unique_ptr<HandlerAbstract>&& successor_p) noexcept
					: successor_{ std::move(successor_p) } {
				};
				/** Pure virtual */
				virtual ~HandlerAbstract() = 0 {};


				/** Optional. Can be defaulted or pure virtual */
				virtual void HandleRequest() {
					if (successor_) {
						successor_->HandleRequest();
					}
				};

			protected:
				/*Set, Get successor_*/
			private:
				std::unique_ptr<HandlerAbstract> successor_;
			};


			class HandlerConcrete1 final : public HandlerAbstract {
			public:
				HandlerConcrete1() = delete;
				HandlerConcrete1(const HandlerConcrete1&) = delete;
				HandlerConcrete1& operator=(HandlerConcrete1&) = delete;

				HandlerConcrete1(HandlerConcrete1&&) noexcept = default;
				HandlerConcrete1& operator=(HandlerConcrete1&&) noexcept = default;

				explicit HandlerConcrete1(std::unique_ptr<HandlerAbstract>&& successor_p) noexcept
					: HandlerAbstract(std::move(successor_p)) {
				};

				/** Can be with pointer to Request to switch, what must be done with request */
				void HandleRequest() override {
					if (false) { // condition of handling request
					} else {
						HandlerAbstract::HandleRequest();
						//successor_->HandleRequest();
					}
				};
			};

			class HandlerConcrete2 final : public HandlerAbstract {
			};

		} // !namespace chain_of_responsibility

	} // !namespace behavioral
} // !namespace pattern

#endif // !CHAIN_OF_RESPONSIBILITY_HPP
