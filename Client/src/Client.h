
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>

#include <raylib.h>

#ifndef __ANDROID__
#include <commandline.h>
#endif

#include "ServerUtils.h"
#include "Sprite.h"


class Client
{
public:
	static void Init();
	static bool Run();
	static void Close();
	static void PrintLine(std::string message);
	static void SetIp(std::string ip){ serverIp = ip;}

	static void SendInputData(const std::unordered_map<char, bool> input);
	static ENetPeer* GetPeer(){return serverPeer;}

	static inline std::unordered_map<Texture2D*, CatCore::Player> players;
	static inline std::unordered_map<Texture2D*, CatCore::Sprite> sprites;

	static std::function<void()> onConnected;
	static std::function<void(const std::string&)> onDisconnected;


private:
	static inline ENetAddress address;
	static inline ENetHost* clientHost;
	static inline ENetPeer* serverPeer;
	static inline ENetEvent event;
	static inline int eventStatus;
	static inline char addressBuffer[ENET_ADDRESS_MAX_LENGTH];
	static inline int running = 1;
	static inline std::string serverIp = "localhost";



#ifndef __ANDROID__
	static inline Commandline com = Commandline("");
#endif

};

