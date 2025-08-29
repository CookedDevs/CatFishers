#include "ServerCommands.h"
#include "Server.h"

#include <sstream>
#include <nlohmann/json.hpp>

void ServerCommands::SendCommandInfo(ENetPeer* receiver)
{
	nlohmann::json json;
	nlohmann::json cmds;

	for (Command command : commands)
	{
		nlohmann::json com;
		com["names"] = command.names;
		com["description"] = command.description;
		cmds[command.names[0]] = com;
	}

	json["commands"] = cmds;
	Server::SendCommandData(receiver, json.dump());
}

bool ServerCommands::HandleCommand(const std::string commandString, ENetPeer* sender)
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
				command.runCommand(sender, arguments);
				return true;
			}
	}

	Server::SendMessage(sender, std::string("Could not find command : " + arguments[0]));
	return false;
}

void ServerCommands::InitializeCommands()
{

	// Test command
	Command test;
	test.names = { "test", "t" };
	test.description = { "Just a test command that prints <Hello World>" };
	test.permission = CatCore::PermissionLevels::Owner;
	test.runCommand = [](const ENetPeer* sender, std::vector<std::string> args) -> bool
	{
			Server::BroadcastMessage("Hello World");
			return true;
	};
	commands.push_back(test);



	// Naming command needed to play the game command
	Command name;
	name.names = { "name", "n" };
	name.description = { "Sends your name to the server and allows you to play the game" };
	name.permission = CatCore::PermissionLevels::NonNamed;
	name.runCommand = [](const ENetPeer* sender, std::vector<std::string> args) -> bool
	{
		Server::GetPlayer(sender->connectID).setName(args[1]);
		return true;
	};
	commands.push_back(name);
}