#pragma once

#include <iostream>

namespace CatCore
{
	class Player
	{
	public:
		Player();
		~Player();

		std::string getName() { return name; }
		void setName(std::string name) { this->name = name; }

	private:
		std::string name = "";
	};

}