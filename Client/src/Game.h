#pragma once
#include "State.h"
#include "Player.h"
#include "ResourceManager.h"
#include <iostream>

class Game : public State
{
public:
	Game() {}
	~Game() {}

	void UnInit() override;
	void Init() override;
	void Update() override;
	std::string GetName() override;

private:

	void SetKey(const char key);
	std::unordered_map<char, bool> inputs;
	std::unordered_map<char, bool> changedInputs;
	Camera2D camera;
};