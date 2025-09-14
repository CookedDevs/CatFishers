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
		ServerUtils::writeToBuffer(buffer, offset, texture.c_str());
		ServerUtils::serializeVector3(buffer, offset, position);
		ServerUtils::writeToBuffer(buffer, offset, &rotation, sizeof(rotation));
		ServerUtils::writeToBuffer(buffer, offset, &size, sizeof(size));
	}
	void Sprite::DeSerialize(const char* buffer, unsigned int& offset)
	{
		char* texturePath = "";
		CatCore::ServerUtils::readTextFromBuffer(buffer, offset, texturePath);
		texture = texturePath;

		CatCore::ServerUtils::deserializeVector3(buffer, offset, position);
		CatCore::ServerUtils::readFromBuffer(buffer, offset, &rotation, sizeof(rotation));
		CatCore::ServerUtils::readFromBuffer(buffer, offset, &size, sizeof(size));
	}
}