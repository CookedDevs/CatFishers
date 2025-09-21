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

		void SetPosition(Vector3 position) { this->position = position; changed = true; }
		const Vector3& GetPosition() { return position; }

		void SetRotation(float rotation) { this->rotation = rotation; changed = true; }
		const float& GetRotation() { return rotation; }

		void SetSize(float size) { this->size = size; changed = true; }
		const float& GetSize() { return size; }

		void SetTexture(std::string texture) { this->texture = texture; changed = true; }
		std::string GetTexture() { return texture; }

		void SetName(std::string name) { this->name = name; changed = true; }
		std::string GetName() { return name; }

		void SetRenderBeforePlayer(bool renderUnderPlayers) { this->renderUnderPlayers = renderUnderPlayers; changed = true;}
		const bool& GetRenderBeforePlayer() { return renderUnderPlayers; }

		void SetChanged(bool changed) { this->changed = changed; }
		const bool& GetChanged() { return changed; }

	private:
		std::string name = "";
		std::string texture = "Resources/Images/cat.png";
		Vector3 position = { 0,0,0 };
		float rotation = 0.f;
		float size = 0.1f;

		bool renderUnderPlayers = false;
		bool changed = false;
	};

}