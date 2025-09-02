#pragma once

#include <iostream>
#include "CatMath.h"

namespace CatCore
{
	class Player
	{
	public:
		Player();
		~Player();

		Vector3 position = { 0,0,0 };

		std::string texture = "Resources/Images/Cat.png";
		std::string name = "";
	private:
	};

}