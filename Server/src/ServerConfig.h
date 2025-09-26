#pragma once
#include <nlohmann/json.hpp>
#include <fstream>
#include "Player.h"

class ServerConfig
{
public:
	static void Init();
	static void Save();

	static void SetIP(std::string ip) { ServerConfig::ip = ip; }
	static std::string GetIP() { return ip; }

	static void SetPort(unsigned int port) { ServerConfig::port = port; }
	static unsigned int GetPort() { return port; }

	static void SetIsIPv6(bool ipv6) { ServerConfig::ipv6 = ipv6; }
	static bool GetIsIPv6() { return ipv6; }

	static void AddUUID(std::string UUID, std::string playerName);
	static bool CheckUUID(std::string UUID);

	static CatCore::Player* GetPlayerData(std::string UUID);
	static void SetPlayerData(std::string UUID, CatCore::Player player);

private:
	static inline std::unordered_map<std::string, CatCore::Player> UUIDs;
	static inline nlohmann::json data;
	static inline std::string ip = "localhost";
	static inline unsigned int port = 22556;
	static inline bool ipv6 = true;
};