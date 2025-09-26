#pragma once

#include <iostream>
#include <map>
#include "Commands.h"
#include "CatCore.h"
#include "Inventory.h"

struct Command
{
	std::vector<std::string> names;
	std::string description;
	CatCore::PermissionLevels permission;

	std::function<bool(ENetPeer* sender, std::vector<std::string> args)> runCommand;
};

class ServerCommands
{
public:
	static void SendCommandInfo(ENetPeer* receiver);
	static void InitializeCommands();
	static bool HandleCommand(const std::string command, ENetPeer* sender); // Sender nullptr until Player class or if nullptr its server
private:
	static inline std::vector<Command> commands;
};