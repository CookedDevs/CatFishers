
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

class Server
{
public:
	Server();
	~Server();

	bool Run();

private:
	ENetAddress address;
	ENetHost* clientHost;
	ENetPeer* serverPeer;
	ENetEvent event;
	int eventStatus;
	char message[1024];
	char addressBuffer[ENET_ADDRESS_MAX_LENGTH];
	int running = 1;
};

