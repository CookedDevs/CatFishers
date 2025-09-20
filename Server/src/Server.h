
#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "ServerUtils.h"
#include "Player.h"
#include "Sprite.h"
#include "Inventory.h"

class Server
{
public:
	static void Init();
	static bool Run();
	static void Close();

	static void BroadcastMessage(const std::string message);
	static void SendPlayerAddOrRemove();
	static void SendPlayers();
	static void SendSpriteAddOrRemove();
	static void SendSprites();
	static void BroadcastExludeMessage(ENetPeer* excludedReseiver, const std::string message, CatCore::ServerReceiveType type);

	static std::unordered_map<ENetPeer*, CatCore::Player>& GetPlayers() { return players; }
	static const void AddPlayer(CatCore::Player player, ENetPeer* peer) { players[peer] = player; playersToAddOrRemove[peer] = false; runplayersToAddOrRemove = true;}
	static const void RemovePlayer(ENetPeer* peer) { playersToAddOrRemove[peer] = true; runplayersToAddOrRemove = true; }
	static CatCore::Player* GetPlayer(ENetPeer* peer) 
	{
		if (players.find(peer) != players.end())
			return &players[peer];
		else
			return nullptr;
	}

	static std::unordered_map<std::string, CatCore::Sprite>& GetSprites() { return sprites; }
	static const void AddSprite(CatCore::Sprite sprite, std::string id) { sprite.SetName(id); sprites[id] = sprite; spritesToAddOrRemove[id] = false; runspritesToAddOrRemove = true; }
	static const void RemoveSprite(std::string id) { spritesToAddOrRemove[id] = true; runspritesToAddOrRemove = true;}
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
	static inline std::unordered_map<ENetPeer*, bool> playersToAddOrRemove;
	static inline bool runplayersToAddOrRemove = false;

	static inline std::unordered_map<std::string, CatCore::Sprite> sprites;
	static inline std::unordered_map<std::string, bool> spritesToAddOrRemove;
	static inline bool runspritesToAddOrRemove = false;
};

