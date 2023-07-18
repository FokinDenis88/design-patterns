#ifndef COMMAND_HPP
#define COMMAND_HPP



/** Software Design Patterns */
namespace pattern {
	namespace behavioral {
		namespace command {
			// https://en.wikipedia.org/wiki/Command_pattern

			///** Defines function, that will be executed by command.  */
			//class Receiver {
			//public:
			//	int Add(int a, int b) { return a + b; }
			//	int Sub(int a, int b) { return a - b; }
			//};

			///** Interface of commands. Abstract class.*/
			//class ICommand {
			//public:
			//	virtual ~ICommand() = default;

			//	virtual void execute() = 0;
			//};

			///** Concrete command. */
			//template<typename ReceiverType, typename ActionReturnType, typename... ActionParams>
			//class CommandConcrete : public ICommand {
			//public:
			//	using ActionType = ActionReturnType(ReceiverType::*)(ActionParams);

			//	CommandConcrete(ReceiverType* const reciever_p, , const CommandSampleParams& params_p)
			//		: receiver{ reciever_p },
			//		params{ params_p },
			//		action{ &Receiver::Add } {
			//	};
			//	CommandConcrete(const CommandConcrete&) = delete;
			//	CommandConcrete& operator=(const CommandConcrete&) = delete;

			//	void execute() override {

			//		client->add_result = (receiver->*action)(params.a, params.b);
			//	};

			//private:
			//	ReceiverType* receiver{};
			//	int (Receiver::* action)(int, int) {};
			//	ActionParams action_params{}...;
			//};

			///**
			// * Object, that invoke execution of command.
			// * Define mode of execution of command.
			// */
			//class Invoker {
			//public:
			//	void InvokeCommand();

			//	inline void SetCommand(ICommand* const new_command_ptr) { command_ptr = new_command_ptr; };
			//	inline const ICommand* GetCommand() const { return command_ptr; };

			//private:
			//	ICommand* command_ptr{};

			//};

			///** Client class, that defines invoke, command and receiver */
			//class Client {
			//public:
			//	inline void SetInvoker(Invoker* const new_invoker_ptr) { invoker_ptr = new_invoker_ptr; };
			//	inline void SetCommand(ICommand* const new_command_ptr) { command_ptr = new_command_ptr; };
			//	inline void SetReceiver(Receiver* const new_receiver_ptr) {
			//		receiver_ptr = new_receiver_ptr;
			//	};

			//	inline const ICommand* GetInvoker() const { return command_ptr; };
			//	inline const ICommand* GetInvoker() const { return command_ptr; };
			//	inline const ICommand* GetInvoker() const { return command_ptr; };

			//private:
			//	Invoker* invoker_ptr{};
			//	ICommand* command_ptr{};
			//	Receiver* receiver_ptr{};
			//};


			///** Main function of pattern module */
			//void Run();

		} // !namespace command
	} // !namespace behavioral
} // !namespace pattern

#endif // !COMMAND_HPP
