#pragma once
#include <vector>
#include <string>
#include "Item.h"

namespace CatCore
{
	struct Fish
	{
		static Item GetRandomFish();
		static std::vector<Item> fish;
	};
}