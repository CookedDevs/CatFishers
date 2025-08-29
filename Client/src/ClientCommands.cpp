#include "ClientCommands.h"
#include "Server.h"

#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>

void ClientCommands::AddServerCommands(std::string commandData)
{
	nlohmann::json json = nlohmann::json::parse(commandData);

	for (nlohmann::json com : json["commands"])
	{
		Command command;
		command.names = com["names"].get<std::vector<std::string>>();
		command.description = com["description"].get<std::string>();
		serverCommands.push_back(command);
	}
}

void ClientCommands::ClearServerCommands()
{
	commands.clear();
}

bool ClientCommands::HandleCommand(const std::string commandString)
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
				return true;
			}
	}

	return false;
}

void ClientCommands::InitializeCommands()
{
	// Help command
	Command help;
	help.names = { "help", "h" };
	help.description = { "Prints description of all commands" };
	help.runCommand = [](std::vector<std::string> args) -> bool
	{
		std::cout << "Client commands :" << "\n";

		for (Command command : commands)
		{
			for (auto name : command.names)
			{
				std::cout << "\t" << name << ", ";
			}

			std::cout << "\t\t" << command.description << "\n";
		}

		std::cout << "Server commands :" << "\n";

		for (Command command : serverCommands)
		{
			for (auto name : command.names)
			{
				std::cout << "\t" << name << ", ";
			}

			std::cout << "\t\t" << command.description << "\n";
		}

		return true;
	};
	commands.push_back(help);
}