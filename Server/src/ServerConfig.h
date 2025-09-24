#pragma once
#include <nlohmann/json.hpp>
#include <fstream>

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
private:
	static inline nlohmann::json data;
	static inline std::string ip = "localhost";
	static inline unsigned int port = 22556;
	static inline bool ipv6 = true;
};