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

	static void SetUUID(std::string UUID) { ClientConfig::UUID = UUID; }
	static std::string GetUUID() { return UUID; }

private:
	static inline std::string UUID = "EMPTY";
	static inline nlohmann::json data;
	static inline std::string name = "";
	static inline std::string ip = "";
};