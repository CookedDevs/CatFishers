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

		Vector3 position;
		std::string getName() { return name; }
		void setName(std::string name) { this->name = name; }

	private:
		std::string name = "";
	};

}