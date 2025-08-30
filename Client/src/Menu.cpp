#include "Menu.h"
#include <iostream>
#include <raylib.h>
#include <math.h>
#include <functional>

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION 330
#else
    #define GLSL_VERSION 100
#endif

const int screenWidth = 800;
const int screenHeight = 450;

static Camera3D camera;
static Image Doggo;
static Texture DoggoTexture = {0};
static Shader shader;
static Image perlinNoiseImage;
static Texture perlinNoiseMap;
static Mesh planeMesh;
static Model planeModel;
static float timeValue = 0.0f;

void Menu::Start()
{
    camera.position = Vector3{ 0.0f, 0.4f, 20.0f };
    camera.target = Vector3{ 0.0f, 0.1f, 0.0f };
    camera.up = Vector3{ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    shader = LoadShader("displace.vs","displace.fs");

    Image perlinImage = GenImagePerlinNoise(512, 512, 0, 0, 1.0f);
    perlinNoiseMap = LoadTextureFromImage(perlinImage);
    UnloadImage(perlinImage);

    int perlinLoc = GetShaderLocation(shader, "perlinNoiseMap");
    SetShaderValueTexture(shader, perlinLoc, perlinNoiseMap);

    timeLoc = GetShaderLocation(shader, "time");

    planeMesh = GenMeshPlane(50, 50, 50, 50);
    planeModel = LoadModelFromMesh(planeMesh);
    planeModel.materials[0].shader = shader; // !!! must assign shader
    planeModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = perlinNoiseMap;
}
void Menu::Update()
{
    UpdateCamera(&camera, CAMERA_FREE);

    timeValue += (GetFrameTime());
    SetShaderValue(shader, timeLoc, &timeValue, SHADER_UNIFORM_FLOAT);

    BeginDrawing();
        ClearBackground(SKYBLUE);

        BeginMode3D(camera);
            BeginShaderMode(shader); // <-- important for mvp uniform
                DrawModel(planeModel, {0,0,0}, 1.0f, WHITE);
            EndShaderMode();
        EndMode3D();

        DrawText(TextFormat("Time: %02.2f", timeValue), 20, 20, 20, BLACK);
        DrawFPS(10, 10);
    EndDrawing();
}



Menu::~Menu()
{
    UnloadShader(shader);
    UnloadModel(planeModel);
    UnloadTexture(perlinNoiseMap);
    if (DoggoTexture.id != 0) UnloadTexture(DoggoTexture);
}
