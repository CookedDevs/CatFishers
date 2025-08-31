
#include "Client.h"
#include "Menu.h"
#include "Game.h"
#include "State.h"

#include <iostream>
#include <raylib.h>

std::once_flag onceFlag;
std::once_flag onceFlag2;
const int screenWidth = 800;
const int screenHeight = 450;

int main()
{
    Client::Init();
    InitWindow(screenWidth, screenHeight, "CatFishers");

    CurrentState::SetState(new Menu);
    while (!WindowShouldClose())
    {
        Client::Run();

        if (IsKeyPressed(KEY_M)) {
            CurrentState::SetState(new Game);
        }
        CurrentState::UpdateState();
    }

    Client::Close();
    CurrentState::DeleteCurrentState();
    CloseWindow();
    return 0;
}