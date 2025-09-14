
#pragma once
#include "Player.h"
#include <iostream>

class Game
{
public:
	Game() {}
	~Game() {}

	static void UnInit();
	static void Init();
	static void Update();

private:
};