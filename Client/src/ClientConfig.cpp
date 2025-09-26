#include "ClientConfig.h"


void ClientConfig::Init()
{
    std::ifstream ifFile("Resources/config.json");
    if (ifFile)
    {
        data = nlohmann::json::parse(ifFile);
        name = data["name"].get<std::string>();
        ip = data["ip"].get<std::string>();
        UUID = data["UUID"].get<std::string>();
        ifFile.close();
    }
    else
    {
        data["name"] = name;
        data["ip"] = ip;
        data["UUID"] = UUID;

        std::ofstream ofFile("Resources/config.json");
        ofFile << data.dump(4);
        ofFile.close();
    }
}

void ClientConfig::Save()
{
    std::ofstream file("Resources/config.json");
    data["name"] = name;
    data["ip"] = ip;
    data["UUID"] = UUID;

    if (file.good() && file.is_open())
    {
        file << data.dump(4);
        file.close();
    }
}