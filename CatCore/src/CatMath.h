#pragma once

#include <iostream>

namespace CatCore
{
	struct Vector4
	{
		float x = 0;
		float y = 0;
		float z = 0;
		float w = 0;
	};

	struct Vector3
	{
		float x = 0;
		float y = 0;
		float z = 0;

		inline bool operator==(const Vector3& otherVector) { return this->x == otherVector.x && this->y == otherVector.y && this->z == otherVector.z; }
		inline bool operator!=(const Vector3& otherVector) { return !(*this == otherVector); }
	};

	struct Vector2
	{
		float x = 0;
		float y = 0;
	};
}