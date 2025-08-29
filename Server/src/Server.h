
#pragma once

#if defined(_WIN32)           
	#define NOGDI             // All GDI defines and routines
	#define NOUSER            // All USER defines and routines
#endif

#include <enet6/enet.h>

#if defined(_WIN32)           // raylib uses these names as function parameters
	#undef near
	#undef far
#endif

#include <string>
#include <unordered_map>

#include "Player.h"

class Server
{
public:
	static void Init();
	static bool Run();
	static void Close();

	static void SendMessage(ENetPeer* receiver, const std::string message);
	static void SendData(ENetPeer* receiver, const std::string message);
	static void SendCommandData(ENetPeer* receiver, const std::string message);
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

