#pragma once

#include <iostream>
#include <unordered_map>
#include "CatMath.h"

namespace CatCore
{
	class Player
	{
	public:
		Player();
		~Player();

		Vector3 position = { 0,0,0 };

		std::string texture = "Resources/Images/cat.png";
		std::string name = "";
		std::unordered_map<char, bool> inputInfo;

	private:

	};

}