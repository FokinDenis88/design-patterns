#ifndef MVC_2_HPP
#define MVC_2_HPP

#include <iostream>

/** Software Design Patterns */
namespace pattern {
	namespace creational {
		class Receiver {
		public:
			int Add(int a, int b) { return a + b; }
			int Sub(int a, int b) { return a - b; }
		};

		class Client {
		public:
			int add_result{};
			int sub_result{};
		};

		/** Interface of. Abstract class*/
		class ICommand {
		public:
			ICommand() = default;
			virtual ~ICommand() = default;

			virtual void execute() = 0;
		};

		struct CommandSampleParams{
			int a{};
			int b{};
		};

		class Command : public ICommand {
		public:
			Command(Receiver* const reciever_p, const CommandSampleParams& params_p, Client* const client_p)
				: receiver_{ reciever_p },
				params_{ params_p },
				action_{ &Receiver::Add },
				client{ client_p } {
			};

			void execute() override {
				client->add_result = (receiver_->*action_)(params_.a, params_.b);
			};

		private:
			Receiver* receiver_{};
			CommandSampleParams params_{};
			int (Receiver::* action_)(int, int){};
			Client* client{};
		};

		class Invoker {
		public:
			ICommand* command_{};
		};

		void Command();
	}
}

#endif // !MVC_2_HPP
