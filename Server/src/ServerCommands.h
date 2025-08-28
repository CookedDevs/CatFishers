#pragma once

#include <iostream>
#include <map>
#include "Commands.h"

struct Command
{
	std::vector<std::string> names;
	std::string description;
	CatCore::PermissionLevels permission;

	std::function<bool(const ENetPeer* sender, std::vector<std::string> args)> runCommand;
};

class ServerCommands
{
public:
	static std::vector<std::string> GetCommandInfo();
	static void InitializeCommands();
	static void HandleCommand(const std::string command, ENetPeer* sender); // Sender nullptr until Player class or if nullptr its server
private:
	static inline std::vector<Command> commands;
};