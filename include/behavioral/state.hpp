#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <memory>

/** Software Design Patterns */
namespace pattern {
	namespace behavioral {
		namespace state {

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
					state_->Handle();
				};
			private:
				std::unique_ptr<IState> state_{};
			};

			void Run();

		} // !namespace state
	} // !namespace behavioral
} // !namespace pattern

#endif // !COMMAND_HPP
