#include "Fish.h"
#include "ServerUtils.h"

namespace CatCore
{
	std::vector<Item> Fish::fish =
	{
		{
			"California Salmon",
			"a Californian fish or something",
			"Resources/Images/Fish/californiaSalmon.png",
			150,
		},
		{
			"Northern Pike",
			"a Northern fish or something",
			"Resources/Images/Fish/northernPike.png",
			48,
		},
		{
			"Perch",
			"a Perch fish or something",
			"Resources/Images/Fish/perch.png",
			32,
		},
		{
			"Sprat",
			"a Sprat fish or something",
			"Resources/Images/Fish/sprat.png",
			1,
		},
		{
			"Striped Bass",
			"a Northern fish or something",
			"Resources/Images/Fish/stripedBass.png",
			256,
		}
	};

	Item Fish::GetRandomFish()
	{
		int fishID = (rand() % fish.size());
		return fish[fishID];
	}
}