#include "ClientCommands.h"
#include "Server.h"

#include <sstream>

std::vector<std::string> ClientCommands::GetCommandInfo()
{
	return {};
}

void ClientCommands::HandleCommand(const std::string commandString)
{
	std::vector<std::string> arguments;

	if (commandString.find(' ') == std::string::npos)
		arguments.push_back(commandString);
	else
	{
		std::istringstream string(commandString);
		std::string argument;

		while (std::getline(string, argument, ' '))
			arguments.push_back(argument);
	}

	arguments[0].erase(0, 1);

	for (Command command : commands)
	{
		for (auto name : command.names)
			if (name == arguments[0])
			{
				command.runCommand(arguments);
				return;
			}
	}
}

void ClientCommands::InitializeCommands()
{
	// Help command
	Command test;
	test.names = { "help", "h" };
	test.description = { "Prints description of all commands" };
	test.runCommand = [](std::vector<std::string> args) -> bool
	{
		for (Command command : commands)
		{
			for (auto name : command.names)
			{
				std::cout << name << ", ";
			}

			std::cout << "\t\t" << command.description << "\n";
		}
		return true;
	};
	commands.push_back(test);
}