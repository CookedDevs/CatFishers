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
	void Inventory::DeSerialize(const char* buffer, unsigned int& offset)
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
			CatCore::ServerUtils::readFromBuffer(buffer, offset, &slot.count, sizeof(slot.count));

			slotInfo[{slotX,slotY}] = slot;
		}
	}
}