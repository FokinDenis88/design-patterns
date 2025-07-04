#ifndef COMMAND_HPP
#define COMMAND_HPP

//#include <iostream>	// for testing functions call

#include <algorithm>
#include <execution> // Execution policies
#include <functional>
#include <type_traits>
#include <list>
#include <memory>
#include <utility>
//#include <concepts>
//#include <variant>	// for CommandListVariant

#include "general-utilities-library/functional/functional.hpp"


/** Software Design Patterns */
namespace pattern {
	namespace behavioral {
        // https://en.wikipedia.org/wiki/Command_pattern
        // It is like First-class functions and High order functions.
        // The invoker object is a higher-order function of which the command object is a first-class argument.
        // Sequence of Commands can be stored in container.
        //
        // Pattern Friends:
        // 1) Composite is used in MacroCommand.
        // 2) Flyweight is used to store state of command for Undo function
        // 3) Prototype for command, that must be copied in history list of commands

		namespace command {
			struct Arguments {
				int a{};
				int b{};
			};

			/**
			 * Defines function, that will be executed by command.
			 * Receiver can be object of any class.
			 */
			class Receiver {
			public:
				int Add(int a, int b) {
					//std::cout << "a + b";
					return a + b;
				}
				int Sub(int a, int b) {
					//std::cout << "a - b";
					return a - b;
				}
			};



            /** T Pointer to Member Function */
            template<typename ActionReturnT, typename ReceiverT, typename... ActionParamTs>
            using MemberFunctionPtrT = ActionReturnT(ReceiverT::*)(ActionParamTs...);


			/** Abstract. Interface of commands. */
			class ICommand {
			protected:
				ICommand() = default;
				ICommand(const ICommand&) = delete; // C.67	C.21
				ICommand& operator=(const ICommand&) = delete;
				ICommand(ICommand&&) noexcept = delete;
				ICommand& operator=(ICommand&&) noexcept = delete;
			public:
				virtual ~ICommand() = default;

				/** Invoke functional object of type: void() */
				virtual void Execute() = 0;
			};

			/** Abstract. */
			class ICommandExtended : public ICommand {
			protected:
				ICommandExtended() = default;
				ICommandExtended(const ICommandExtended&) = delete; // C.67	C.21
				ICommandExtended& operator=(const ICommandExtended&) = delete;
				ICommandExtended(ICommandExtended&&) noexcept = delete;
				ICommandExtended& operator=(ICommandExtended&&) noexcept = delete;
			public:
				~ICommandExtended() override = default;

				/**
				 * Reverts the results of command execution.
				 * Commands can be hold in undo stack. Pop stack + Undo.
				 */
				virtual void Undo() = 0;

				/** Macro Recording. Save command objects somewhere for next play back. */
				virtual void Record() = 0;

				/** For progress bars */
				virtual void GetEstimatedDuration() = 0;

				/** Notifies Invoker if Command is inactive */
				virtual bool IsActive() = 0;
			};


			/**
			* Helper Function
			* Creates Command Action from Member Function. Arguments are binded with std::function(void()).
			* Command pattern is designed to have object, that invokes request, and object, that receives request.
			* Invoker mustn't know the concrete receiver object. Use only command interface.
			*
			* @param action_ptr			pointer to member function
			* @param receiver_p			ref to receiver object
			* @param ...action_args_p	arguments for member function call
			* @return					type: void(). Function object with binded arguments
			*/
			template<typename ReturnT, typename ReceiverT, typename... ParamTs>
			requires std::is_class_v<ReceiverT>
			inline std::function<void()>
				CreateActionByMemberFn(const MemberFunctionPtrT<ReturnT, ReceiverT, ParamTs...> action_ptr,
									  ReceiverT& receiver_p,	// mustn't be const. Command execution may change state
									  ParamTs&&... action_args_p) {	// noexcept: bind maybe has exceptions

				// T can be converted from any pointer to fn type to void(), if all arguments of fn are binded
				// Only reinterpret_cast helps to convert one pointer to member function to another, of another type
				// void() is unified type for all command functions.
				std::function<void()> new_action = std::bind(action_ptr, &receiver_p,
															 std::forward<ParamTs>(action_args_p)...);
				return new_action;
			};


			/**
			* Command for functor, lambda, member function, function, std::function.
			* You can't separately change receiver, action and action args when ever you want.
			*
			* Invariant: action object must be type = void().
			*
			* @param	action_with_args_ std::function wrapper, binded with args.
			*/
			class Command : public ICommand {
			public:
				using ActionT = void();
				using ActionWithArgsT = std::function<ActionT>;

				/** Create empty command with no action. */
				Command() = default; // f.e. for creating empty vector of commands

                /** Constructs Command from std::function object.
				 *
                 * @param new_action callable object must be binded with args, if there are params in function.
                 */
                explicit Command(const ActionWithArgsT& new_action) : action_{ new_action } {
                };

                /** Construction from pointer to member function with sinature: void(params...). */
                template<typename ActionReturnT, typename ReceiverT, typename... ActionParamTs>
				requires std::is_class_v<ReceiverT>
                explicit Command(const MemberFunctionPtrT<ActionReturnT, ReceiverT, ActionParamTs...> action_ptr,
								ReceiverT& receiver_p, // mustn't be const. Command execution may change state
								ActionParamTs&&... action_args_p)
                    : action_{ CreateActionByMemberFn(action_ptr, receiver_p,
														std::forward<ActionParamTs>(action_args_p)...) } {
                };

			protected:
				Command(const Command&) = delete;	// polymorph suppress copy & move
				Command& operator=(const Command&) = delete;
				Command(Command&&) noexcept = delete;
				Command& operator=(Command&&) noexcept = delete;

			public:
				~Command() override = default;

				/** Execute stored action object */
				inline void Execute() override {
					if (action_) { action_(); }
				};

				/** Assign new action object from std::function and execute it. */
				inline void Execute(const ActionWithArgsT& new_action) {
                    action_ = new_action;
                    Execute();
                };

				/**
				 * Assign new action object from pointer to member function.
				 * Then execute this action object.
				 *
				 * @param action_ptr		pointer to member function
				 * @param receiver_p		pointer to receiver object
				 * @param ...action_args_p	arguments for member function call
				 * @return					function object with binded arguments
				 */
				template<typename ActionReturnT, typename ReceiverT, typename... ActionParamTs>
				requires std::is_class_v<ReceiverT>
				inline void Execute(const MemberFunctionPtrT<ActionReturnT, ReceiverT, ActionParamTs...> action_ptr,
									ReceiverT& receiver_p,
									ActionParamTs&&... action_args_p) {
					action_ = CreateActionByMemberFn(action_ptr, receiver_p, std::forward<ActionParamTs>(action_args_p)...);
					Execute();
				};

			private:
				/**
				* Action of some Receiver. with args binded. Callable Function object
				* must be binded with args for each param. No Invariant.
				*/
				ActionWithArgsT action_{};

				/*
				* Class Design:
				* Bind arguments to std::function wrapper for each param.
				* The idea of command pattern is to abstract receiver object and to encapsulate call to member
				* function of receiver object.
				* If is used std::function, i can't change receiver object, function-action and args separately.
				*/

			};	// !class Command


			/**
			 * Command only for class member functions.
			 * You can separately change receiver, action and action args when ever you want.
			 *
			 * Realization May include count of repetitions of executions.
			 * May include state for Undo operation, that can be changed after execution.
			 * May include pattern Flyweight for storing state of command object before execution.
			 *
			 * Invariant: must hold member function pointer, receiver and args.
			 */
			template<typename ActionReturnT, typename ReceiverT, typename... ActionParamTs>
			requires std::is_class_v<ReceiverT>
			class CommandMemberFn : public ICommand {
			public:
				/** Pointer to member function */
				using ActionT = MemberFunctionPtrT<ActionReturnT, ReceiverT, ActionParamTs...>;
				using ReceiverPtrT = std::shared_ptr<ReceiverT>;
				using ArgsTupleTs = std::tuple<ActionParamTs&&...>;

				CommandMemberFn() = default;

			protected:
				CommandMemberFn(const CommandMemberFn&) = delete;	// polymorph suppress copy & move
				CommandMemberFn& operator=(const CommandMemberFn&) = delete;
				CommandMemberFn(CommandMemberFn&&) noexcept = delete;
				CommandMemberFn& operator=(CommandMemberFn&&) noexcept = delete;

			public:
				~CommandMemberFn() override = default;

				explicit CommandMemberFn(const ReceiverPtrT& receiver_p,
										 const ActionT action_p, ActionParamTs... action_args_p) // TODO: Maybe ref to args?
							: receiver_{ receiver_p },
							  action_{ action_p },
							  action_args_{ std::make_tuple(action_args_p...) } {
				};


				void Execute() override {
					Execute_11();
				};

				// TODO: refactor - replace setters and getters

				inline void set_receiver(const ReceiverPtrT& new_receiver) noexcept {
					receiver_ = new_receiver;
				};
				inline void set_action(const ActionT new_action) noexcept {
					action_ = new_action;
				};
				inline void set_action_args(const ArgsTupleTs& new_action_args) noexcept {
					action_args_ = new_action_args;
				};
				/** Change N argument in tuple */
				template<typename ArgT, size_t index>
				inline void set_concrete_action_arg(const ArgT& new_arg) noexcept {
					std::get<index>(action_args_) = new_arg;
				};


				inline std::weak_ptr<ReceiverT> receiver() const noexcept {
					return receiver_;
				};
				inline const ActionT action() const noexcept {
					return action_;
				};
				inline const ArgsTupleTs& action_args() const noexcept {
					return receiver_;
				};
				/** Get N argument in tuple */
				template<typename ArgT, size_t index>
				inline const ArgT& concrete_action_arg() const noexcept {
					return std::get<index>(action_args_);
				};

			private:
				using STDFunctionT = std::function<ActionReturnT(ActionParamTs...)>;

				/** Implementation function for Execute(). Needs C++ 11. */
				inline void Execute_11() {
					if (!receiver_.expired()) {
						STDFunctionT action_fn = std::bind_front(action_, receiver_.lock().get());
						common::Apply(action_fn, action_args_);
					}
				};

				/** Implementation function for Execute(). Needs C++ 17. */
				inline void Execute_17() {
					if (!receiver_.expired()) {
						STDFunctionT action_call = std::bind_front(action_, receiver_.lock().get());
						std::apply(action_call, action_args_);	// Needs C++ 17
					}
				};

				/* Receiver is stored by aggregation, not composition */
				std::weak_ptr<ReceiverT> receiver_{};
				ActionT action_{};
				ArgsTupleTs action_args_{};
			};	// !class CommandMemberFn




			template<typename ActionT>
			class TestClass {
			public:
				ActionT member_function_ptr{};
			};



			/**
			 * Delegator, that invoke execution of command.
			 * Define mode of execution of command.
			 * Define bookkeeping(archive) of execution of command.
			 */
			class Invoker final {
			public:
				inline void InvokeCommand() const {
					command_.lock()->Execute();
				}


				inline void set_command(std::shared_ptr<ICommand> const new_command_ptr) noexcept {
					command_ = std::weak_ptr<ICommand>(new_command_ptr);
				};

				/** Can be nullptr, because of weak_ptr */
				inline const ICommand* command() const noexcept { return command_.lock().get(); };

			private:
				std::weak_ptr<ICommand> command_{};
			};


			/** Client class, that holds invoker, command and receiver */
			class Client final {
			public:
				inline void InvokeCommand() {
					if (invoker_) { invoker_->InvokeCommand(); }
				};


				inline void set_receiver(std::shared_ptr<Receiver> new_receiver) noexcept {
					receiver_ = std::move(new_receiver);
					//if (command_) {command_}
				};
				inline void set_command(std::shared_ptr<ICommand> new_command) noexcept {
					command_ = std::move(new_command);
					if (invoker_) { invoker_->set_command(command_); }
				};
				inline void set_invoker(std::unique_ptr<Invoker>&& new_invoker) noexcept { invoker_ = std::move(new_invoker); };

				inline const Receiver& receiver() const noexcept { return *receiver_; };
				inline const ICommand& command() const noexcept { return *command_; };
				inline const Invoker& invoker() const noexcept { return *invoker_; };

			private:
				/** Receiver is also stored by aggregation in Command */
				std::shared_ptr<Receiver> receiver_{};

				/** Command is also stored by aggregation in Invoker */
				std::shared_ptr<ICommand> command_{};

				std::unique_ptr<Invoker> invoker_{};
			};	// !class Client


//===============================Macro Commands=======================================

			/** Ts for containers derived from ICommand */
			template<typename... CommandTs>
			concept ICommandDerivedTs = (std::derived_from<CommandTs, ICommand> && ...);

			/*template<typename... CommandTs>
			requires ICommandDerivedTs<CommandTs...>
			using CommandListVariant = std::list<std::variant<CommandTs...>>;*/


			/**
			 * Execute sequence of commands. May be Composite pattern.
			 * Is used for stateless or stateful references to commands.
			 *
			 * @param commands_ list of commands.
			 *
			 * Invariant: elements of Container maybe ICommand, ptr or ref to ICommand.
			 */
			template<typename ContainerT = std::vector<ICommand>>
			requires std::is_same_v<ContainerT, std::vector<ICommand>> ||
					 std::is_same_v<ContainerT, std::forward_list<ICommand>> ||
					 std::is_same_v<ContainerT, std::list<ICommand>>
					 //std::is_same_v<ContainerT, std::deque<ICommand>> ||
			struct MacroCommand: public ICommand {
			public:
				using ICommandT = ICommand;
				using ICommandPtrT = std::unique_ptr<ICommand>;
				using ICommandRefT = std::reference_wrapper<ICommand>;

				// Container Ts variants: vector, list, deque
				using ContainerVectorT = std::vector<ICommand>;
				using ContainerForwardListT = std::forward_list<ICommand>;

				MacroCommand() = default;

			protected:
				MacroCommand(const MacroCommand&) = delete;	// polymorph suppress copy & move
				MacroCommand& operator=(const MacroCommand&) = delete;
				MacroCommand(MacroCommand&&) noexcept = delete;
				MacroCommand& operator=(MacroCommand&&) noexcept = delete;

			public:
				~MacroCommand() override = default;

				void Execute() override {
					for (auto& command : commands_) { // mustn't be const, cause can change state
							command.Execute();
					}
				};

				template<typename ExecutionPolicyT>
				requires std::is_execution_policy_v<ExecutionPolicyT>
				void ExecuteWithPolicy(ExecutionPolicyT policy = std::execution::seq) {
					std::for_each(policy, commands_.begin(), commands_.end(),
									[](auto& command) { command.Execute(); });
				};


				/** Container with commands */
				ContainerT commands_{};

				/*
				* Class Design:
				* Command may be stateless or stateful.
				* All Variety of Commands is hold outside of the class. Class holds only ref to commands.
				* So you can economy memory for huge count of MacroCommands with common commands.
				* Commands must not be nullptr.
				* Command will be list for flexibility.
				*/

			}; // !class MacroCommand

		} // !namespace command

	} // !namespace behavioral
} // !namespace pattern

#endif // !COMMAND_HPP
