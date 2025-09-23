#pragma once
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

class ClientConfig
{
public:
	static void Init();
	static void Save();

	static void SetIP(std::string ip) { ClientConfig::ip = ip; }
	static std::string GetIP() { return ip; }
	static void SetName(std::string name) { ClientConfig::name = name; }
	static std::string GetName() { return name; }

private:
	static inline nlohmann::json data;
	static inline std::string name = "";
	static inline std::string ip = "";
};