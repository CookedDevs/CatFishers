#pragma once
#include "Player.h"
#include "Sprite.h"

#include <iostream>
#include <chrono>

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

	static void ThrowBobber(ENetPeer* peer);
    static bool ReelBobber(ENetPeer* peer);

private:
};