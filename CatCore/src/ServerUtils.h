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
#include <cstring>
#include <vector>

#include "Commands.h"
#include "CatMath.h"

namespace CatCore
{
	enum InputType
	{
		NoInput,
		LeftMouse,
		RightMouse,
		Scroll,
		Input4,
		Input5
	};

	struct Mouse
	{
		float x;
		float y;
		InputType input = NoInput;
	};

	class ServerUtils
	{
	public:
		static void SendMessage(ENetPeer* receiver, const std::string message);
		static void SendData(ENetPeer* receiver, const std::vector<uint8_t> data);
		static void SendCommandData(ENetPeer* receiver, const std::string message);

		static void writeToBuffer(char* buffer, unsigned int& offset, const void* data, size_t size);
		static void writeToBuffer(char* buffer, unsigned int& offset, const char* str);
		static void serializeVector3(char* buffer, unsigned int& offset, const Vector3& vec);
		static void readFromBuffer(const char* buffer, unsigned int& offset, void* data, size_t size);
		static void readTextFromBuffer(const char* buffer, unsigned int& offset, char*& data);
		static void deserializeVector3(const char* buffer, unsigned int& offset, Vector3& vector);
	private:
	};
}