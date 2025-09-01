#include "Game.h"
#include <iostream>
#include <raylib.h>
#include <math.h>
#include <functional>

void Game::UnInit()
{
	std::cout << "GAME STATE DELETE";
}

std::string Game::GetName() { return "Game"; }
void Game::Init()
{
	std::cout << "GAME STATE INIT";
}

void Game::Update()
{
	//std::cout << "GAME STATE UPDATE";
}