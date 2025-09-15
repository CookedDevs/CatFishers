#pragma once

#include <iostream>
#include <unordered_map>
#include "CatMath.h"
#include "ServerUtils.h"

namespace CatCore
{
	class Player
	{
	public:
		Player();
		~Player();

		void SetPosition(Vector3 position) { this->position = position; changed = true; }
		const Vector3& GetPosition() { return position; }

		void SetTexture(std::string texture) { this->texture = texture; changed = true; }
		std::string GetTexture() { return texture; }

		void SetName(std::string name) { this->name = name; changed = true; }
		std::string GetName() { return name; }

		void SetInputInfo(std::unordered_map<char, bool> inputInfo) { this->inputInfo = inputInfo; changed = true; }
		void SetInputInfo(char key, bool value) { this->inputInfo[key] = value; changed = true; }
		const std::unordered_map<char, bool>& GetInputInfo() { return inputInfo; }

		bool IsKeyDown(char key) { return (GetInputInfo().find(key) != GetInputInfo().end() && GetInputInfo().at(key) == true); }

		void SetChanged(bool changed) { this->changed = changed;}
		bool GetChanged() { return changed; }

		void SetMouse(Mouse mouse) { this->mouse = mouse; }
		const Mouse& GetMouse() { return mouse; }

	private:

		Vector3 position = { 0,0,0 };
		std::string texture = "Resources/Images/cat.png";
		std::string name = "";
		std::unordered_map<char, bool> inputInfo;
		Mouse mouse;

		bool changed = false;
	};

}