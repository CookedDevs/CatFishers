#include "Menu.h"
#include <iostream>
#include <raylib.h>
#include <functional>

#define NUM_FRAMES  3 

const int screenWidth = 800;
const int screenHeight = 450;

static Camera2D camera;
static float timeValue = 0.0f;
Model model;
Vector3 position = { 0.0f, 0.0f, 0.0f }; // Set model position


Menu::~Menu()
{
    UnloadTexture(NewGameButton);
}

std::string Menu::GetName() { return "Menu"; }
void Menu::Init()
{
    camera.target = Vector2{ 0.0f, 0.0f};
    NewGameButton = LoadTexture("NewGame.png");
    NewGameButton.height = NewGameButton.height/2;
    NewGameButton.width = NewGameButton.width/2;

    frameHeight = (float)NewGameButton.height/NUM_FRAMES;
    sourceRec = { 0, 0, (float)NewGameButton.width, frameHeight};

    btnBounds = { screenWidth/2.0f - NewGameButton.width/2.0f, (screenHeight/2.0f - NewGameButton.height/NUM_FRAMES/2.0f) - 50, (float)NewGameButton.width, frameHeight };


}

void Menu::Update()
{
    mousePoint = GetMousePosition();
    timeValue += GetFrameTime();
    NewGameAction = false;

    if (CheckCollisionPointRec(mousePoint, btnBounds))
    {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) btnState = 2;
        else btnState = 1;

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) NewGameAction = true;
    }
    else btnState = 0;

    if (NewGameAction)
    {
        // TODO: Any desired action
    }

    sourceRec.y = btnState*frameHeight;

    BeginDrawing();
        ClearBackground(SKYBLUE);
        DrawTextureRec(NewGameButton, sourceRec, Vector2{ btnBounds.x, btnBounds.y}, WHITE);

        BeginMode2D(camera);
        EndMode2D();

        DrawFPS(10, 10);
    EndDrawing();
}

