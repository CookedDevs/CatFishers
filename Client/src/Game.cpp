#include "Game.h"
#include "Player.h"
#include "Client.h"
#include "AndroidInput.h"

void Game::UnInit()
{
    LoadedTextures::UnLoadAllTex();
}

std::string Game::GetName() { return "Game"; }
void Game::Init()
{
    camera.target = Vector2{ 0.0f, 0.0f };

    Client::onDisconnected = [this](const std::string& reason) 
    {
        std::cout << reason << "\n";
        Game::disconnected = true;
        CurrentState::SetState(new Menu);
    };
}

void Game::SetKey(const char key, const bool value)
{
    if (inputs.find(key) == inputs.end())
    {
        inputs[key] = value;
        changedInputs[key] = inputs[key];
    }
    else if (value != inputs[key])
    {
        inputs[key] = !inputs[key];
        changedInputs[key] = inputs[key];
    }
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
    Client::Run();
    if (disconnected) return;

    changedInputs.clear();
    SetKey('A'); SetKey('D');
    SetKey('W'); SetKey('S');

    BeginDrawing();
    ClearBackground(SKYBLUE);
    
    BeginMode2D(camera);
    EndMode2D();

    //Client::SendFuncId(1);

    for (auto player : Client::players)
        DrawTextureEx(*player.first, {player.second.position.x, player.second.position.y}, 0, 0.1f, WHITE);

    for (auto sprite : Client::sprites)
        DrawTextureEx(*sprite.first, { sprite.second.position.x, sprite.second.position.y }, sprite.second.rotation, sprite.second.size, WHITE);

    AndroidInput::Joystick();
    Client::SendInputData(changedInputs);

    DrawFPS(10, 10);
    EndDrawing();
}