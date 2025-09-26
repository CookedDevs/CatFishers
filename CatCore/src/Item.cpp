#include "Item.h"
#include "ServerUtils.h"

namespace CatCore
{
	Item::Item(std::string name, std::string description, std::string texture, uint32_t value, uint8_t stackSize)
	{
		this->name = name;
		this->description = description;
		this->texture = texture;
		this->value = value;
		this->stackSize = stackSize;
	}

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

		ServerUtils::writeToBuffer(buffer, offset, &itemType, sizeof(itemType));
		ServerUtils::writeToBuffer(buffer, offset, &value, sizeof(value));
		ServerUtils::writeToBuffer(buffer, offset, &stackSize, sizeof(stackSize));
	}
	void Item::DeSerialize(const char* buffer, unsigned int& offset)
	{
		char* nam; char* descr; char* textr;
		ServerUtils::readTextFromBuffer(buffer, offset, nam); name = nam;
		ServerUtils::readTextFromBuffer(buffer, offset, descr); description = descr;
		ServerUtils::readTextFromBuffer(buffer, offset, textr); texture = textr;

		ServerUtils::readFromBuffer(buffer, offset, &itemType, sizeof(itemType));
		ServerUtils::readFromBuffer(buffer, offset, &value, sizeof(value));
		ServerUtils::readFromBuffer(buffer, offset, &stackSize, sizeof(stackSize));
	}

	void Item::ToJson(nlohmann::json& json)
	{
		json["name"] = name;
		json["description"] = description;
		json["texture"] = texture;
		json["itemType"] = itemType;
		json["value"] = value;
		json["stackSize"] = stackSize;
	}

	void Item::FromJson(const nlohmann::json& json)
	{
		name = json["name"];
		description = json["description"];
		texture = json["texture"];
		itemType = json["itemType"];
		value = json["value"];
		stackSize = json["stackSize"];
	}
}