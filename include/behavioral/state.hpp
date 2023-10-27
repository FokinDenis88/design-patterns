#ifndef STATE_HPP
#define STATE_HPP

#include <memory>

/** Software Design Patterns */
namespace pattern {
	namespace behavioral {
		namespace observer_state_ {

			class IState {
			public:
				virtual ~IState() = default;

				virtual void Handle() = 0;
			};

			class StartState : public IState {
			public:
				void Handle() override {

				};
			};

			class MiddleState : public IState {
			public:
				void Handle() override {

				};
			};

			class EndState : public IState {
			public:
				void Handle() override {

				};
			};

			class Client {
			public:
				void Request() {
					observer_state_->Handle();
				};
			private:
				std::unique_ptr<IState> observer_state_{};
			};

		} // !namespace observer_state_

	} // !namespace behavioral
} // !namespace pattern

#endif // !STATE_HPP
