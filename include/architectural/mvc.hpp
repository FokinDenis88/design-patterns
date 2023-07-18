#ifndef MVC_HPP
#define MVC_HPP

#include <iostream>

/** Software Design Patterns */
namespace pattern {
	namespace creational {
		namespace mvc {
			//class Receiver {
			//public:
			//	int Add(int a, int b) { return a + b; }
			//	int Sub(int a, int b) { return a - b; }
			//};

			//class Client {
			//public:
			//	int add_result{};
			//	int sub_result{};
			//};

			///** Interface of. Abstract class*/
			//class ICommand {
			//public:
			//	ICommand() = default;
			//	virtual ~ICommand() = default;

			//	virtual void execute() = 0;
			//};

			//struct CommandSampleParams{
			//	int a{};
			//	int b{};
			//};

			//class CommandConcrete : public ICommand {
			//public:
			//	CommandConcrete(Receiver* const reciever_p, const CommandSampleParams& params_p, Client* const client_p)
			//		: receiver{ reciever_p },
			//		params{ params_p },
			//		action{ &Receiver::Add },
			//		client{ client_p } {
			//	};

			//	void execute() override {
			//		client->add_result = (receiver->*action)(params.a, params.b);
			//	};

			//private:
			//	Receiver* receiver{};
			//	CommandSampleParams params{};
			//	int (Receiver::* action)(int, int){};
			//	Client* client{};
			//};

			//class Invoker {
			//public:
			//	ICommand* command_ptr{};
			//};

			//void Command();

		} // !namespace mvc
	} // !namespace creational
} // !namespace pattern

#endif // !MVC_HPP
