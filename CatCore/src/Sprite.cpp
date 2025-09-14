#include "Sprite.h"
#include "ServerUtils.h"

namespace CatCore
{
	Sprite::Sprite()
	{

	}

	Sprite::~Sprite()
	{

	}

	void Sprite::serialize(char* buffer, unsigned int& offset)
	{
		ServerUtils::writeToBuffer(buffer, offset, name.c_str());
		ServerUtils::writeToBuffer(buffer, offset, texture.c_str());
		ServerUtils::serializeVector3(buffer, offset, position);
		ServerUtils::writeToBuffer(buffer, offset, &rotation, sizeof(rotation));
		ServerUtils::writeToBuffer(buffer, offset, &size, sizeof(size));
	}
	void Sprite::DeSerialize(const char* buffer, unsigned int& offset)
	{

	}
}