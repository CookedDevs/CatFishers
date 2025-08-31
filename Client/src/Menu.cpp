#include "Menu.h"
#include <iostream>
#include <raylib.h>
#include <functional>

const int screenWidth = 800;
const int screenHeight = 450;

static Camera3D camera;
static float timeValue = 0.0f;
Model model;
Vector3 position = { 0.0f, 0.0f, 0.0f }; // Set model position


Menu::~Menu()
{
    UnloadModel(model);
}

std::string Menu::GetName() { return "Menu"; }
void Menu::Init()
{
    //UnloadModel(model);
    camera.position = Vector3{ 0.0f, 0.4f, 20.0f };
    camera.target = Vector3{ 0.0f, 0.1f, 0.0f };
    camera.up = Vector3{ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    model = LoadModel("../../../../Client/Resources/Models/water.glb");
}

void Menu::Update()
{
    UpdateCamera(&camera, CAMERA_FREE);

    timeValue += GetFrameTime();

    BeginDrawing();
        ClearBackground(SKYBLUE);

        BeginMode3D(camera);
            DrawModel(model, position, 1.0f, WHITE);
        EndMode3D();

        DrawFPS(10, 10);
    EndDrawing();
}

