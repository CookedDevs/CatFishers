
#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <raylib.h>

#ifndef __ANDROID__
#include <commandline.h>
#endif

#include "ServerUtils.h"

class Client
{
public:
	static void Init();
	static bool Run();
	static void Close();
	static void PrintLine(std::string message);

	static void SendInputData(const std::unordered_map<char, bool> input);

	static inline CatCore::Player player;
	static inline Texture2D playerTex;

private:
	static inline ENetAddress address;
	static inline ENetHost* clientHost;
	static inline ENetPeer* serverPeer;
	static inline ENetEvent event;
	static inline int eventStatus;
	static inline char addressBuffer[ENET_ADDRESS_MAX_LENGTH];
	static inline int running = 1;


#ifndef __ANDROID__
	static inline Commandline com = Commandline("");
#endif

};

