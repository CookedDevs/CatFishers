#include "ServerConfig.h"

void ServerConfig::Init()
{
    std::ifstream ifFile("config.json");
    if (!ifFile) { ServerConfig::Save(); return; }

    data = nlohmann::json::parse(ifFile);
    ip = data["ip"].get<std::string>();
    port = data["port"].get<unsigned int>();
    ipv6 = data["IsIPv6"].get<bool>();

    for (auto UUID : data["UUIDs"].items())
    {
        CatCore::Player plr;
        plr.FromJson(UUID.value());
        UUIDs[UUID.key()] = plr;
    }

    ifFile.close();
}

void ServerConfig::Save()
{
    std::ofstream file("config.json");
    data["ip"] = ip;
    data["port"] = port;
    data["IsIPv6"] = ipv6;

    for (auto UUID : UUIDs)
        UUID.second.ToJson(data["UUIDs"][UUID.first]);

    if (file.good() && file.is_open())
    {
        file << data.dump(4);
        file.close();
        std::cout << "Saved server!\n";
    }
}

void ServerConfig::SavePlayer(std::string UUID)
{
    std::ofstream file("config.json");
    if (CheckUUID(UUID)) UUIDs[UUID].ToJson(data["UUIDs"][UUID]);

    if (file.good() && file.is_open())
    {
        file << data.dump(4);
        file.close();
        std::cout << "Saved server!\n";
    }
}

void ServerConfig::AddUUID(std::string UUID, std::string playerName)
{
    UUIDs[UUID] = CatCore::Player(playerName);
}
bool ServerConfig::CheckUUID(std::string UUID)
{
    for (auto uuid : UUIDs)
        if (uuid.first == UUID) return true;
    return false;
}

CatCore::Player* ServerConfig::GetPlayerData(std::string UUID)
{
    if (CheckUUID(UUID)) return &UUIDs[UUID];
    return nullptr;
}
void ServerConfig::SetPlayerData(std::string UUID, CatCore::Player player)
{
    if (CheckUUID(UUID)) UUIDs[UUID] = player;
}