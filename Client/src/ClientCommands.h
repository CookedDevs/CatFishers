#pragma once

#include <iostream>
#include <map>
#include <vector>

#include "Commands.h"

struct Command
{
	std::vector<std::string> names;
	std::string description;

	std::function<bool(std::vector<std::string> args)> runCommand;
};

class ClientCommands
{
public:
	static void AddServerCommands(std::string commandData);
	static void InitializeCommands();
	static void ClearServerCommands();
	static bool HandleCommand(const std::string command); // Sender nullptr until Player class or if nullptr its server
private:
	static inline std::vector<Command> commands;
	static inline std::vector<Command> serverCommands;
};