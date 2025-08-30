
#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "ServerUtils.h"
#include "Player.h"

class Server
{
public:
	static void Init();
	static bool Run();
	static void Close();

	static void BroadcastMessage(const std::string message);
	static void BroadcastExludeMessage(ENetPeer* excludedReseiver, const std::string message);

	static const std::unordered_map<unsigned int, CatCore::Player>& GetPlayers() { return players; }
	static const void AddPlayer(CatCore::Player player, unsigned int id) { players[id] = player; }
	static CatCore::Player& GetPlayer(unsigned int id) { return players[id]; }

private:
	static inline ENetAddress address;
	static inline ENetHost* serverHost;
	static inline ENetEvent event;
	static inline int eventStatus;
	static inline char addressBuffer[ENET_ADDRESS_MAX_LENGTH];

	static inline std::unordered_map<unsigned int, CatCore::Player> players;
};

