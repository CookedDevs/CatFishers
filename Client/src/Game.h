#pragma once
#include "State.h"
#include "Player.h"
#include "Menu.h"
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
	bool disconnected = false;
	Camera2D camera;
};