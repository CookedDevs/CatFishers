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
#include <vector>

#include "Commands.h"

namespace CatCore
{
	class ServerUtils
	{
	public:
		static void SendMessage(ENetPeer* receiver, const std::string message);
		static void SendData(ENetPeer* receiver, const std::vector<std::byte> data);
		static void SendCommandData(ENetPeer* receiver, const std::string message);
	private:
	};
}