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
#include <map>
#include <vector>
#include <functional>

#include "Player.h"


namespace CatCore
{
	enum PermissionLevels
	{
		Owner,
		Admin,
		Trusted,
		Named,
		NonNamed,
		Guest
	};

	enum ServerReceiveType
	{
		Message = 1,
		CommandData = 2,
		Data = 3,
		PlayerAddOrRemove = 4,
		PlayerData = 5,
		Function = 6
	};
}