
#pragma once
#include "Player.h"
#include <iostream>

struct Bobber {
    CatCore::Sprite sprite;
    std::chrono::steady_clock::time_point biteTime;
    bool ready = false;
};


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