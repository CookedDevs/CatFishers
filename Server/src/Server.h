
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
	static void SendPlayers();
	static void SendPlayerData(const std::vector<uint8_t> data);
	static void BroadcastExludeMessage(ENetPeer* excludedReseiver, const std::string message, CatCore::ServerReceiveType type);

	static const std::unordered_map<ENetPeer*, CatCore::Player>& GetPlayers() { return players; }
	static const void AddPlayer(CatCore::Player player, ENetPeer* peer) { players[peer] = player; }
	static CatCore::Player& GetPlayer(ENetPeer* peer) { return players[peer]; }

private:
	static inline std::unordered_map<char, bool> inputInfo;
	static inline ENetAddress address;
	static inline ENetHost* serverHost;
	static inline ENetEvent event;
	static inline int eventStatus;
	static inline char addressBuffer[ENET_ADDRESS_MAX_LENGTH];

	static inline std::unordered_map<ENetPeer*, CatCore::Player> players;
};

