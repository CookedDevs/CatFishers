#pragma once

#include <iostream>
#include <unordered_map>
#include "CatMath.h"

namespace CatCore
{
	class Sprite
	{
	public:
		Sprite();
		~Sprite();

		virtual void serialize(char* buffer, unsigned int& offset);
		virtual void DeSerialize(const char* buffer, unsigned int& offset);

		Vector3 position = { 0,0,0 };
		float rotation = 0.f;
		float size = 0.1f;

		std::string texture = "Resources/Images/cat.png";
	private:

	};

}