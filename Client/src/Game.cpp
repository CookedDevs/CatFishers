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

    for (auto player : Client::players)
        DrawTextureEx(*player.first, {player.second.position.x, player.second.position.y}, 0, 0.1f, WHITE);

    for (auto sprite : Client::sprites)
        DrawTextureEx(*sprite.first, { sprite.second.position.x, sprite.second.position.y }, sprite.second.rotation, sprite.second.size, WHITE);

#ifdef _ANDROID_
    Vector2 circlePosition = { 110.f, (float)GetScreenHeight() - 110.f };
    float biggerRadius = 70.f;

    Vector2 smallerCirclePosition = { 110.f, (float)GetScreenHeight() - 110.f };
    float smallerRadius = 30.f;

    float angle = 0.0f;
    float dx = 0.0f, dy = 0.0f, dxx = 0.0f, dyy = 0.0f;

    smallerCirclePosition = GetMousePosition();

    if (!CheckCollisionPointCircle(smallerCirclePosition, circlePosition, biggerRadius) && GetTouchPointCount() > 0)
    {
        dx = smallerCirclePosition.x - circlePosition.x;
        dy = smallerCirclePosition.y - circlePosition.y;

        angle = atan2f(dy, dx);

        dxx = (biggerRadius) * cosf(angle);
        dyy = (biggerRadius) * sinf(angle);

        smallerCirclePosition.x = circlePosition.x + dxx;
        smallerCirclePosition.y = circlePosition.y + dyy;
    }
    else
    {
        smallerCirclePosition = { 110.f, (float)GetScreenHeight() - 110.f };
    }

    unsigned int deadZoneSize = 14;

    if (smallerCirclePosition.x <= circlePosition.x - deadZoneSize) SetKey('A', true); else SetKey('A', false);
    if (smallerCirclePosition.x >= circlePosition.x + deadZoneSize) SetKey('D', true); else SetKey('D', false);
    if (smallerCirclePosition.y <= circlePosition.y - deadZoneSize) SetKey('W', true); else SetKey('W', false);
    if (smallerCirclePosition.y >= circlePosition.y + deadZoneSize) SetKey('S', true); else SetKey('S', false);

    DrawCircleV(circlePosition, biggerRadius, LIGHTGRAY);
    DrawCircleV(smallerCirclePosition, smallerRadius, RED);
#endif
    Client::SendInputData(changedInputs);

    DrawFPS(10, 10);
    EndDrawing();
}