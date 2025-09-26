#include "Inventory.h"
#include "ServerUtils.h"

namespace CatCore
{
	Inventory::Inventory(uint8_t width, uint8_t height) : width(width), height(height) {}
	Inventory::~Inventory() {}

	bool Inventory::AddItem(uint8_t slotX, uint8_t slotY, Item item, uint8_t count)
	{
		if (slotX > width || slotY > height) return false;
		if (slotInfo[{slotX, slotY}].count == 0) slotInfo[{slotX, slotY}] = { item, 1 };
		else if (slotInfo[{slotX, slotY}].item.GetName() == item.GetName()) slotInfo[{slotX, slotY}].count++;
		else return false;

		changed = true;
		return true;
	}

	bool Inventory::RemoveItem(uint8_t slotX, uint8_t slotY, uint8_t count)
	{
		if (slotX > width || slotY > height) return false;
		if (slotInfo[{slotX, slotY}].count > 0) return false;
		else slotInfo[{slotX, slotY}].count--;

		changed = true;
		return true;
	}

	void Inventory::serialize(char* buffer, unsigned int& offset)
	{
		uint8_t slotDataSize = slotInfo.size();
		CatCore::ServerUtils::writeToBuffer(buffer, offset, &slotDataSize, sizeof(slotDataSize));

		for (auto slot : slotInfo)
		{
			CatCore::ServerUtils::writeToBuffer(buffer, offset, &slot.first.first, sizeof(slot.first.first));
			CatCore::ServerUtils::writeToBuffer(buffer, offset, &slot.first.second, sizeof(slot.first.second));

			slot.second.item.serialize(buffer, offset);
			CatCore::ServerUtils::writeToBuffer(buffer, offset, &slot.second.count, sizeof(slot.second.count));
		}
	}
	void Inventory::DeSerialize(const char* buffer, unsigned int& offset, std::map<std::string, bool>& texturesToLoadOrRemove)
	{
		uint8_t slotDataSize;
		CatCore::ServerUtils::readFromBuffer(buffer, offset, &slotDataSize, sizeof(slotDataSize));

		slotInfo.clear();
		for (size_t i = 0; i < slotDataSize; i++)
		{
			uint8_t slotX, slotY;
			Slot slot;

			CatCore::ServerUtils::readFromBuffer(buffer, offset, &slotX, sizeof(slotX));
			CatCore::ServerUtils::readFromBuffer(buffer, offset, &slotY, sizeof(slotY));

			slot.item.DeSerialize(buffer, offset);
			if (auto& texture = textures[{slotX, slotY}]; texture != slot.item.GetTexture())
			{
				if (!texture.empty()) texturesToLoadOrRemove[texture] = true;
				texture = slot.item.GetTexture();
				texturesToLoadOrRemove[texture] = false;
			}

			CatCore::ServerUtils::readFromBuffer(buffer, offset, &slot.count, sizeof(slot.count));

			slotInfo[{slotX,slotY}] = slot;
		}
	}

	void Inventory::ToJson(nlohmann::json& json)
	{
		struct slotData { uint8_t x = 0, y = 0; Slot slot; };
		std::vector<slotData> data;
		for (auto slot : slotInfo)
			data.push_back({ slot.first.first, slot.first.second, slot.second });

		for (size_t i = 0; i < data.size(); i++)
		{
			json[i]["slot"]["x"] = data[i].x;
			json[i]["slot"]["y"] = data[i].y;
			json[i]["slot"]["count"] = data[i].slot.count;
			data[i].slot.item.ToJson(json[i]["slot"]["item"]);
		}
	}

	void Inventory::FromJson(const nlohmann::json& json)
	{
		for (size_t i = 0; i < json.size(); i++)
		{
			uint8_t x, y, count;
			Item item;

			x = json[i]["slot"]["x"];
			y = json[i]["slot"]["y"];
			count = json[i]["slot"]["count"];
			item.FromJson(json[i]["slot"]["item"]);
		}
	}
}