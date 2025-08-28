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
	static std::vector<std::string> GetCommandInfo();
	static void InitializeCommands();
	static void HandleCommand(const std::string command); // Sender nullptr until Player class or if nullptr its server
private:
	static inline std::vector<Command> commands;
	static inline std::vector<Command> serverCommands;
};