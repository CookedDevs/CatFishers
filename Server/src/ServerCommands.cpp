#include "ServerCommands.h"
#include "Server.h"
#include "ServerConfig.h"
#include "Fish.h"

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
	CatCore::ServerUtils::SendCommandData(receiver, json.dump());
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

	CatCore::ServerUtils::SendMessage(sender, std::string("Could not find command : !" + arguments[0] + "\n" + "Use !help or !h to see available commands"));
	return false;
}

void ServerCommands::InitializeCommands() // TODO: permissions
{
	// Test command
	Command test;
	test.names = { "test", "t" };
	test.description = { "Just a test command that prints <Hello World>" };
	test.permission = CatCore::PermissionLevels::Owner;
	test.runCommand = [](ENetPeer* sender, std::vector<std::string> args) -> bool
	{
			Server::BroadcastMessage("Hello World");
			CatCore::Player* player = Server::GetPlayer(sender);
			CatCore::Inventory& inv = player->GetInventory();
			inv.AddItem(0, 0, CatCore::Fish::GetRandomFish());
			Server::BroadcastMessage(player->GetInventory().GetSlot(0, 0).item.GetName());
			return true;
	};
	commands.push_back(test);


	// Naming command
	Command name;
	name.names = { "name", "n" };
	name.description = { "Sends your name to the server and allows you to play the game" };
	name.permission = CatCore::PermissionLevels::NonNamed;
	name.runCommand = [](ENetPeer* sender, std::vector<std::string> args) -> bool
	{
		std::string UUID = Server::GetPeerUUID(sender);
		if (!ServerConfig::CheckUUID(UUID))
		{
			std::string newUUID = CatCore::ServerUtils::GetUUID();
			CatCore::ServerUtils::SendPlayerUUID(sender, newUUID);
			Server::SetPeerUUID(sender, newUUID);
			UUID = newUUID;
			ServerConfig::AddUUID(newUUID, args[1]);
			ServerConfig::Save();
		}

		Server::AddPlayer(*ServerConfig::GetPlayerData(UUID), sender);

		CatCore::ServerUtils::SendMessage(sender, "Name set to : " + args[1] + "\n");
		CatCore::Player* player = Server::GetPlayer(sender);
		CatCore::Inventory& inv = player->GetInventory();
		inv.AddItem(0, 0, CatCore::Fish::GetRandomFish());
		Server::BroadcastMessage(player->GetInventory().GetSlot(0, 0).item.GetName());
		return true;
	}; 
	commands.push_back(name);
}