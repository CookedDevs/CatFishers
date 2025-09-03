#include "Game.h"
#include "Player.h"
#include "Client.h"

void Game::UnInit()
{
    LoadedTextures::UnLoadAllTex();
}

std::string Game::GetName() { return "Game"; }
void Game::Init()
{
    camera.target = Vector2{ 0.0f, 0.0f };

    //CatCore::Player plr;
    //plr.name = "Sus";
    //plr.position = { 1,1 };
    //Client::playerTex = LoadedTextures::LoadTex(plr.texture);
    //player = plr;

}

void Game::SetKey(const char key)
{
    if (inputs.find(key) == inputs.end())
    {
        inputs[key] = IsKeyDown(key);
        changedInputs[key] = inputs[key];
    }
    else if (IsKeyDown(key) != inputs[key])
    {
        inputs[key] = !inputs[key];
        changedInputs[key] = inputs[key];
    }
}

void Game::Update()
{
    //SetTargetFPS(20);
    changedInputs.clear();
    SetKey('A'); SetKey('D');
    SetKey('W'); SetKey('S');
    Client::SendInputData(changedInputs);

    BeginDrawing();
    ClearBackground(SKYBLUE);
    
    BeginMode2D(camera);
    EndMode2D();

    for (CatCore::Player player : Client::players)
        DrawTexture(Client::playerTex, player.position.x, player.position.y, WHITE);

    DrawFPS(10, 10);
    EndDrawing();
}