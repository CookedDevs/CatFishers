#pragma once
#include "State.h"
#include "Player.h"
#include "Menu.h"
#include "ResourceManager.h"
#include <iostream>
#include <vector>

#define NUM_FRAMES  2

struct Itemslot {
	Texture2D texture;
	Rectangle bounds;
	Rectangle sourceRec;
	float frameHeight;
	int state;
	bool enabled;
};

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
	bool hasBobber = false;
	void SetKey(const char key, const bool value);
	void SetKey(const char key);
	std::unordered_map<char, bool> inputs;
	std::unordered_map<char, bool> changedInputs;
	Camera2D camera;
	
};