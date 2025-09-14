
#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "ServerUtils.h"
#include "Player.h"
#include "Sprite.h"

class Server
{
public:
	static void Init();
	static bool Run();
	static void Close();

	static void BroadcastMessage(const std::string message);
	static void SendPlayers();
	static void SendScene();
	static void SendPlayerData(const std::vector<uint8_t> data);
	static void BroadcastExludeMessage(ENetPeer* excludedReseiver, const std::string message, CatCore::ServerReceiveType type);

	static std::unordered_map<ENetPeer*, CatCore::Player>& GetPlayers() { return players; }
	static const void AddPlayer(CatCore::Player player, ENetPeer* peer) { players[peer] = player; }
	static CatCore::Player* GetPlayer(ENetPeer* peer) 
	{
		if (players.find(peer) != players.end())
			return &players[peer];
		else
			return nullptr;
	}

	static std::unordered_map<std::string, CatCore::Sprite>& GetSprites() { return sprites; }
	static const void AddSprite(CatCore::Sprite sprite, std::string id) { sprites[id] = sprite; }
	static CatCore::Sprite* GetSprite(std::string id)
	{
		if (sprites.find(id) != sprites.end())
			return &sprites[id];
		else
			return nullptr;
	}

private:
	static inline ENetAddress address;
	static inline ENetHost* serverHost;
	static inline ENetEvent event;
	static inline int eventStatus;
	static inline char addressBuffer[ENET_ADDRESS_MAX_LENGTH];

	static inline std::unordered_map<ENetPeer*, CatCore::Player> players;
	static inline std::unordered_map<std::string, CatCore::Sprite> sprites;
};

