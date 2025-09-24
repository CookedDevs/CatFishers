#include "ServerConfig.h"

void ServerConfig::Init()
{
    std::ifstream ifFile("config.json");
    if (ifFile)
    {
        data = nlohmann::json::parse(ifFile);
        ip = data["ip"].get<std::string>();
        port = data["port"].get<unsigned int>();
        ipv6 = data["IsIPv6"].get<bool>();
        ifFile.close();
    }
    else
    {
        data["ip"] = ip;
        data["port"] = port;
        data["IsIPv6"] = ipv6;

        std::ofstream ofFile("config.json");
        ofFile << data.dump(4);
        ofFile.close();
    }
}

void ServerConfig::Save()
{
    std::ofstream file("config.json");
    data["ip"] = ip;
    data["port"] = port;
    data["IsIPv6"] = ipv6;

    if (file.good() && file.is_open())
    {
        file << data.dump(4);
        file.close();
    }
}