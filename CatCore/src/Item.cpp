#include "Item.h"
#include "ServerUtils.h"

namespace CatCore
{
	Item::Item()
	{
	}

	Item::~Item()
	{
	}

	void Item::serialize(char* buffer, unsigned int& offset)
	{
		ServerUtils::writeToBuffer(buffer, offset, name.c_str());
		ServerUtils::writeToBuffer(buffer, offset, description.c_str());
		ServerUtils::writeToBuffer(buffer, offset, texture.c_str());
		ServerUtils::writeToBuffer(buffer, offset, &value, sizeof(value));
	}
	void Item::DeSerialize(const char* buffer, unsigned int& offset)
	{
		char* nam; char* descr; char* textr;
		ServerUtils::readTextFromBuffer(buffer, offset, nam); name = nam;
		ServerUtils::readTextFromBuffer(buffer, offset, descr); description = descr;
		ServerUtils::readTextFromBuffer(buffer, offset, textr); texture = textr;
		ServerUtils::readFromBuffer(buffer, offset, &value, sizeof(value));
	}
}