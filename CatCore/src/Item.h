#pragma once
#include <string>
#include <cstdint>
#include <map>
#include <nlohmann/json.hpp>

namespace CatCore
{
	enum ItemType
	{
		Null,
		Fish,
		FihsingRod,
	};

	class Item
	{
	public:
		Item(std::string name, std::string description, std::string texture, uint32_t value, uint8_t stackSize = 32);
		Item();
		~Item();

		void serialize(char* buffer, unsigned int& offset);
		void DeSerialize(const char* buffer, unsigned int& offset);

		void SetName(std::string name) { this->name = name; changed = true; }
		std::string GetName() { return name; }

		void SetDescription(std::string description) { this->description = description; changed = true; }
		std::string GetDescription() { return description; }

		void SetTexture(std::string texture) { this->texture = texture; changed = true; }
		std::string GetTexture() { return texture; }

		void SetChanged(bool changed) { this->changed = changed; }
		bool GetChanged() { return changed; }

		void SetValue(uint8_t value) { this->value = value; changed = true; }
		const uint8_t& GetValue() { return value; }

		void SetStackSize(uint8_t stackSize) { this->stackSize = stackSize; changed = true; }
		const uint8_t& GetStackSize() { return stackSize; }

		void ToJson(nlohmann::json& json);
		void FromJson(const nlohmann::json& json);

	private:
		std::string name = "noname";
		std::string description = "nodescription";
		std::string texture = "notexture";
		
		ItemType itemType = Null;
		uint32_t value = 0;
		uint8_t stackSize = 32;
		bool changed = false;
	};
}