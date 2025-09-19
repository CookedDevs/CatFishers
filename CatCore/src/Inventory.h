#pragma once
#include <map>
#include "Item.h"

namespace CatCore
{
	struct Slot
	{
		Item item;
		uint8_t count = 0;
	};

	class Inventory
	{
	public:
		Inventory(uint8_t width = 8, uint8_t height = 4);
		~Inventory();

		bool AddItem(uint8_t slotX, uint8_t slotY, Item item, uint8_t count = 1);
		bool RemoveItem(uint8_t slotX, uint8_t slotY, uint8_t count = 1);

		Slot& GetSlot(uint8_t slotX, uint8_t slotY) { return slotInfo[{slotX, slotY}]; changed = true; }
		std::map<std::pair<uint8_t, uint8_t>, Slot> GetSlots() { return slotInfo; }
		void ClearSlot(uint8_t slotX, uint8_t slotY) { slotInfo[{slotX, slotY}].count = 0; changed = true; }

		const uint8_t GetWidth() { return width; }
		const uint8_t GetHeight() { return height; }

		void SetChanged(bool changed) { this->changed = changed; }
		bool GetChanged() { return changed; }

		void serialize(char* buffer, unsigned int& offset);
		void DeSerialize(const char* buffer, unsigned int& offset);

	private:
		std::map<std::pair<uint8_t, uint8_t>, Slot> slotInfo;
		uint8_t width;
		uint8_t height;

		bool changed = false;
	};
}
