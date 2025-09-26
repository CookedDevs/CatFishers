#include "Game.h"
#include "Player.h"
#include "Client.h"
#include "AndroidInput.h"

void Game::UnInit()
{
    Client::players.clear();
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

void Game::Update()
{
    Client::Run();
    if (disconnected) return;

    BeginDrawing();
    ClearBackground(SKYBLUE);
    
    BeginMode2D(camera);
    EndMode2D();

    for (auto sprite : Client::sprites)
        if (sprite.second.GetRenderBeforePlayer()) DrawTextureEx(*LoadedTextures::GetTex(sprite.second.GetTexture()), { sprite.second.GetPosition().x, sprite.second.GetPosition().y }, sprite.second.GetRotation(), sprite.second.GetSize(), WHITE);

    for (auto player : Client::players)
        DrawTextureEx(*LoadedTextures::GetTex(player.second.GetTexture()), { player.second.GetPosition().x, player.second.GetPosition().y}, 0, 0.1f, WHITE);

    for (auto sprite : Client::sprites)
        if (!sprite.second.GetRenderBeforePlayer()) DrawTextureEx(*LoadedTextures::GetTex(sprite.second.GetTexture()), { sprite.second.GetPosition().x, sprite.second.GetPosition().y }, sprite.second.GetRotation(), sprite.second.GetSize(), WHITE);


    Client::ClearChangedInputs();
    Client::SetKey('A'); Client::SetKey('D');
    Client::SetKey('W'); Client::SetKey('S');

    bool usedJoystick = AndroidInput::Joystick();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !usedJoystick) {
        Vector2 mousepoint = GetMousePosition();
        Client::mouse.x = mousepoint.x;
        Client::mouse.y = mousepoint.y;
        Client::mouse.input = CatCore::LeftMouse;
        if (!hasBobber) {
            Client::SendInputData();
            Client::SendFuncId(1);
            hasBobber = true;
        }
        else {
            Client::SendFuncId(2);
            hasBobber = false;
        }
    }

    Client::SendInputData();

    DrawFPS(10, 10);
    EndDrawing();
}