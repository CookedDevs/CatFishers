#include "Game.h"
#include <iostream>
#include <raylib.h>
#include <math.h>
#include <functional>

Game::~Game()
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