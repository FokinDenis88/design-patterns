#include "behavioral/command.hpp"

#include <iostream>

namespace pattern {
	namespace behavioral {
		namespace command {
			/*void Invoker::InvokeCommand() {
				command_ptr->execute();
			}

			void Run() {
				Invoker invoker{};
				Receiver receiver{};
				Client client{};
				invoker.command_ptr = new CommandConcrete(&receiver, CommandSampleParams{ 5, 3 }, &client);

				std::cout << __FILE__ << " " << __FUNCTION__ << " add_result: " << client.add_result;
			}*/

		} //!namespace command
	} //!namespace behavioral
} //!namespace pattern
