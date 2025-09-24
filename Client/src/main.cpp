
#include "Client.h"
#include "Menu.h"
#include "Game.h"
#include "State.h"
#include "ClientConfig.h"

#include <iostream>
#include <raylib.h>

const int screenWidth = 800;
const int screenHeight = 450;

int main()
{
    InitWindow(screenWidth, screenHeight, "CatFishers");
    ClientConfig::Init();

    CurrentState::SetState(new Menu);
    while (!WindowShouldClose())
    {
        CurrentState::UpdateState();
    }

    Client::Close();
    CurrentState::DeleteCurrentState();
    CloseWindow();
    return 0;
}