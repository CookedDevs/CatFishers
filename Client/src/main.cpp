
#include "Client.h"
#include "Menu.h"

#include <iostream>
#include <raylib.h>
#include <math.h>
#include <functional>
#include <mutex>

std::once_flag onceFlag;
const int screenWidth = 800;
const int screenHeight = 450;

std::function<void()> currentState;


void Game(){

}

void Menu(){
    std::call_once(onceFlag, Menu::Start);
    Menu::Update();
}

int main()
{
    Client::Init();
    currentState = Menu;

    InitWindow(screenWidth, screenHeight, "CatFishers");

    while (!WindowShouldClose())
    {
        Client::Run();

    Client::Close();

        if (IsKeyPressed(KEY_M)) {
            currentState = Game;
        }
        currentState();
    }

    CloseWindow();
    return 0;
}