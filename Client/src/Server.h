
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

#include <iostream>
#include <string>
#include <vector>

#ifndef __ANDROID__
#include <commandline.h>
#endif

class Server
{
public:
	static void Init();
	static bool Run();
	static void Close();

	static void SendMessage(ENetPeer* receiver, const std::string message);
	static void SendData(ENetPeer* receiver, const std::vector<std::byte> data);
	static void SendCommandData(ENetPeer* receiver, const std::string message);
private:
	static inline ENetAddress address;
	static inline ENetHost* clientHost;
	static inline ENetPeer* serverPeer;
	static inline ENetEvent event;
	static inline int eventStatus;
	static inline char addressBuffer[ENET_ADDRESS_MAX_LENGTH];
	static inline int running = 1;

#ifndef __ANDROID__
	static inline Commandline com;
#endif

};

