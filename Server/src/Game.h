#pragma once
#include "Player.h"
#include "Sprite.h"

#include <iostream>
#include <chrono>

struct Bobber {
    std::string spriteid;
    std::chrono::steady_clock::time_point biteTime;
    bool ready = false;
	std::chrono::steady_clock::time_point floatStart;
	std::chrono::seconds biteDuration;
	CatCore::Vector3 pos;
	CatCore::Vector3 basePos;
	float randomPhase;

	float lastYankStrength = 0.0f;
	float yankStart = -999.0f;
	float nextYankTime = 0.0f;
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