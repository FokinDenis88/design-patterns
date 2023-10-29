#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <iostream>

#include <list>
#include <functional>
#include <memory>
#include <utility>
#include <concepts>
#include <functional>
#include <variant>

#include "tuple.hpp"


/** Software Design Patterns */
namespace pattern {
	namespace behavioral {

		namespace command {
			// https://en.wikipedia.org/wiki/Command_pattern
			// It is like First-class functions and High order functions.
			// The invoker object is a higher-order function of which the command object is a first-class argument.
			// Sequence of Commands can be stored in container.
			// Pattern Friends:
			// 1) Composite is used in MacroCommand.
			// 2) Flyweight is used to store state of command for Undo function
			// 3) Prototype for command, that must be copied in history list of commands


			struct Arguments {
				int a{};
				int b{};
			};

			/** Defines function, that will be executed by command.  */
			class Receiver {
			public:
				int Add(int a, int b) {
					std::cout << "a + b";
					return a + b;
				}
				int Sub(int a, int b) {
					std::cout << "a - b";
					return a - b;
				}
			};


			/** Abstract. Interface of commands. */
			class ICommand {
			public:
				virtual ~ICommand() = default;

				virtual void Execute() = 0;
			};

			/** Abstract. */
			class ICommandExtended : public ICommand {
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
			 * To create std::function object use: std::bind(&ObjectClass::MemberFunction, PointerToObject, MemberFunctionArgs...);
			 * Only Copyable. Not Movable.
			 */
			class CommandSTDFunction final : public ICommand {
				// Class Description:
			public:
				using ActionType = void();

				CommandSTDFunction() = default;
				CommandSTDFunction(const CommandSTDFunction&) = default;
				CommandSTDFunction& operator=(const CommandSTDFunction&) = default;

				// No Move constructors, because std::function support Copy operations

				/** @param new_action callable object must be binded with args, if there are params in function */
				explicit CommandSTDFunction(const std::function<ActionType>& new_action)
						: action_with_args_{ new_action } {
				};

				template<typename ReceiverType, typename ActionReturnType, typename... ActionParamTypes>
				explicit CommandSTDFunction(ActionReturnType(ReceiverType::* const action_ptr)(ActionParamTypes...),
											ReceiverType& receiver_p, // mustn't be const. Command execution may change state
											ActionParamTypes&&... action_args_p)
						: action_with_args_{ CreateAction(action_ptr, receiver_p, std::forward<ActionParamTypes>(action_args_p)...) } {
				};

				/**
				 * Command pattern is designed to have object, that invokes request, and object, that receives request.
				 * Invoker mustn't know the concrete receiver object. Use only command interface.
				 */
				template<typename ReceiverType, typename ActionReturnType, typename... ActionParamTypes>
				static auto CreateAction(ActionReturnType(ReceiverType::* const action_ptr)(ActionParamTypes...),
										 ReceiverType&  receiver_p,	// mustn't be const. Command execution may change state
										 ActionParamTypes&&... action_args_p) {
					std::function<ActionType> new_action = std::bind_front(action_ptr, &receiver_p,
																		   std::forward<ActionParamTypes>(action_args_p)...);
					return new_action;
				};


				void Execute() override {
					if (action_with_args_) { action_with_args_(); }
				};

				/** Check, if there is action function object */
				inline bool HasAction() const noexcept { return action_with_args_.operator bool(); };


				/** @param new_action callable object must be binded with args, if there are params in function */
				inline void set_action_with_args(const std::function<ActionType>& new_action) {
					action_with_args_ = new_action;
				};

				/**
				 * Create callable action object for command
				 *
				 * @param receiver_ptr		pointer to receiver object
				 * @param action_ptr		pointer to member function
				 * @param ...action_args_p	arguments for member function call
				 * @return					function object with no arguments in call
				 */
				template<typename ReceiverType, typename ActionReturnType, typename... ActionParamTypes>
				inline void set_action_with_args(ActionReturnType(ReceiverType::* const action_ptr)(ActionParamTypes...),
												  ReceiverType& receiver_p,
												  ActionParamTypes&&... action_args_p) {
					action_with_args_ = CreateAction(action_ptr, receiver_p, std::forward<ActionParamTypes>(action_args_p)...);
				};


				inline const std::function<ActionType>& action_with_args() const noexcept {
					return action_with_args_;
				};

			private:
				/** Service functionality optional */
				template<typename ReceiverType, typename ActionReturnType, typename... ActionParamTypes>
				void CreateActionNoArgs(ActionReturnType(ReceiverType::* const action_ptr)(ActionParamTypes...),
										ReceiverType& receiver_p) {
					action_with_args_ = std::bind_front(action_ptr, &receiver_p);
				}

				std::function<ActionType> action_with_args_{};
			};	// !class CommandSTDFunction


			/**
			 * Concrete command.
			 * You can change receiver, action and action args when ever you want
			 * Realization May include count of repetitions of executions.
			 * May include state for Undo operation, that can be changed after execution.
			 * May include pattern Flyweight for storing state of command object before execution.
			 */
			template<typename ReceiverType, typename ActionReturnType, typename... ActionParamTypes>
			// requires callable
			class Command final : public ICommand {
			public:
				/** Pointer to member function */
				using ActionType = ActionReturnType(ReceiverType::*)(ActionParamTypes...);
				using ArgsTupleTypes = std::tuple<ActionParamTypes...>;
				using STDFunctionType = ActionReturnType(ActionParamTypes...);

				Command() = default;
				Command(const Command&) = default;
				Command& operator=(const Command&) = default;
				Command(Command&&) noexcept = default;
				Command& operator=(Command&&) noexcept = default;

				explicit Command(std::shared_ptr<ReceiverType> receiver_p,
								 ActionType action_p, ActionParamTypes... action_args_p) // TODO: Maybe ref to args?
							: receiver_{ std::move(receiver_p) },
							  action_{ action_p },
							  action_args_{ std::make_tuple(action_args_p...) } {
				};


				void Execute() override {
					Execute_11();
				};


				inline void set_receiver(const std::shared_ptr<ReceiverType> new_receiver) noexcept {
					receiver_ = new_receiver;
				};
				inline void set_action(ActionType new_action) noexcept {
					action_ = new_action;
				};
				inline void set_action_args(const ArgsTupleTypes& new_action_args) noexcept {
					action_args_ = new_action_args;
				};
				/** Change N argument in tuple */
				template<typename ArgType, size_t index>
				inline void set_concrete_action_arg(const ArgType& new_arg) noexcept {
					std::get<index>(action_args_) = new_arg;
				};


				inline std::weak_ptr<ReceiverType> receiver() const noexcept {
					return receiver_;
				};
				inline ActionType action() const noexcept {
					return action_;
				};
				inline const ArgsTupleTypes& action_args() const noexcept {
					return receiver_;
				};
				/** Get N argument in tuple */
				template<typename ArgType, size_t index>
				inline const ArgType& concrete_action_arg() const noexcept {
					return std::get<index>(action_args_);
				};

			private:
				/** Implementation function for Execute(). Needs C++ 11. */
				inline void Execute_11() {
					if (!receiver_.expired()) {
						std::function<STDFunctionType> action_fn = std::bind_front(action_, receiver_.lock().get());
						cpp::ApplyTuple(action_fn, action_args_);
					}
				};

				/** Implementation function for Execute(). Needs C++ 17. */
				inline void Execute_17() {
					if (!receiver_.expired()) {
						std::function<STDFunctionType> action_call = std::bind_front(action_, receiver_.lock().get());
						std::apply(action_call, action_args_);	// Needs C++ 17
					}
				};

				/* Receiver is stored by aggregation, not composition */
				std::weak_ptr<ReceiverType> receiver_{};
				ActionType action_{};
				ArgsTupleTypes action_args_{};
			};	// !class Command




			template<typename ActionType>
			class TestClass {
			public:
				ActionType member_function_ptr{};
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


				inline void set_command(std::shared_ptr<ICommand> const new_command_ptr) {
					command_ = std::weak_ptr<ICommand>(new_command_ptr);
				};

				/** Can be nullptr, because of weak_ptr */
				inline const ICommand* command() const { return command_.lock().get(); };

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

			/** Types for containers derived from ICommand */
			template<typename... CommandTypes>
			concept ICommandDerivedTypes = (std::derived_from<CommandTypes, ICommand> && ...);

			template<typename... CommandTypes>
			requires ICommandDerivedTypes<CommandTypes...>
			using CommandListVariant = std::list<std::variant<CommandTypes...>>;

			/** List of reference wrappers to ICommand objects */
			using ICommandRefList = std::list<std::reference_wrapper<ICommand>>;

			using ICommandPtrList = std::list<std::unique_ptr<ICommand>>;


			/**
			 * Execute sequence of commands. May be Composite pattern.
			 * Is used for stateless or stateful references to commands.
			 *
			 * @param commands_ list of commands. Invariant: CommandType must be derived from ICommand.
			 * Element of commands_ must be ref. Invariant: can't be a nullptr pointer.
			 */
			struct MacroCommand: public ICommand {
			public:
				void Execute() override {
					for (auto& command_ptr : commands_) {
						if (command_ptr) {
							command_ptr->Execute();
						}
					}
				};

				ICommandPtrList commands_{};

				/*
				* Class Design:
				* Command may be stateless or stateful.
				* All Variety of Commands is hold outside of the class. Class holds only ref to commands.
				* So you can economy memory for huge count of MacroCommands with common commands.
				* Commands must not be nullptr.
				* Command will be list for flexibility.
				*/
			};



			/**
			 * Execute sequence of commands. May be Composite pattern.
			 * Is used only stateless references to commands.
			 *
			 * @param commands_ list of commands. Invariant: CommandType must be derived from ICommand.
			 * Element of commands_ must be ref. Invariant: can't be a nullptr pointer.
			 */
			/*struct MacroCommandStateless : public ICommand {
			public:
				void Execute() override {
					for (auto& command_ref : commands_) {
						command_ref.get().Execute();
					}
				};

				ICommandRefList commands_{};*/

				/*
				* Class Design:
				* Command is stateless.
				* All Variety of Commands is hold outside of the class. Class holds only ref to commands.
				* So you can economy memory for huge count of MacroCommands with common commands.
				* Store single archive of all commands.
				* Commands must not be nullptr.
				* Command will be list for flexibility.
				*/
			//};


			/**
			 * Execute sequence of commands. May be Composite pattern.
			 * Is used by stateful commands.
			 *
			 * @param commands_ list of commands. Invariant: CommandTypes... must be derived from ICommand.
			 */
			/*template<typename... CommandTypes>
			requires ICommandDerivedTypes<CommandTypes...>
			struct MacroCommandStateful : public ICommand {
			public:*/
				// Needs C++ 17

				//void Execute() override {
				//	for (auto& command : commands_) {	// commands can change state, so not const auto&
				//		//std::get<command.index()>(command).Execute(); // can throw std::bad_variant_access
				//		constexpr size_t indx { command.index() };
				//		std::get<0>(command).Execute(); // can throw std::bad_variant_access

				//		// Can't access to variant element at compile time. Only runtime.
				//	}
				//};

				//CommandListVariant<CommandTypes...> commands_{};

				/*
				* Class Design:
				* Command is stateful.
				* All Variety of Commands is hold inside of class, coz the state of command
				* is unique for each command and needed for undo operation.
				* Commands must not be nullptr.
				* Command will be list for flexibility.
				* Class is only static polymorphic, coz template of class.
				*/
			//};

		} // !namespace command

	} // !namespace behavioral
} // !namespace pattern

#endif // !COMMAND_HPP
