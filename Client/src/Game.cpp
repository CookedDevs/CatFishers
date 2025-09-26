#include "Game.h"
#include "Player.h"
#include "Client.h"
#include "ClientConfig.h"
#include "AndroidInput.h"

static Vector2 mousePoint = { 0.0f, 0.0f };

static std::vector<Itemslot> itemslots;

CatCore::Player& player = Client::players[ClientConfig::GetName()];
CatCore::Inventory* inv = nullptr;

void Game::UnInit()
{
    for (auto b : itemslots) UnloadTexture(b.texture);
    Client::players.clear();
    LoadedTextures::UnLoadAllTex();
}

static Itemslot CreateItemSlot(Texture2D tex, Vector2 pos, float scale, bool enabled) {
    Itemslot itemslot{};
    itemslot.texture = tex;
    itemslot.frameHeight = (float)itemslot.texture.height / NUM_FRAMES;
    pos.x -= itemslot.texture.width * scale / 2.0f;
    pos.y -= itemslot.frameHeight * scale / 2.0f;
    itemslot.sourceRec = { 0, 0, (float)itemslot.texture.width, itemslot.frameHeight };
    itemslot.bounds = { pos.x, pos.y, itemslot.texture.width * scale, itemslot.frameHeight * scale };
    itemslot.enabled = enabled;
    itemslot.state = 0;

    return itemslot;
}

static void DrawItemSlot(const Itemslot& slot, int index) {
    DrawTexturePro(slot.texture, slot.sourceRec, slot.bounds, Vector2{ 0, 0 }, 0.0f, WHITE);

    CatCore::Inventory* invCheck = &player.GetInventory();

    if (inv != invCheck)
    {
        CatCore::Inventory* inv = invCheck;
        auto& item = player.GetInventory().GetSlot(index + 1, 0).item;
        if (item.GetTexture() != "notexture") {
            Texture2D* texPtr = LoadedTextures::GetTex(item.GetTexture());
            std::cout << item.GetTexture() + "/n";
            if (texPtr)
            {
                Rectangle src = { 0, 0, (float)texPtr->width, (float)texPtr->height};
                Rectangle dst = slot.bounds;
                DrawTexturePro(*texPtr, src, dst, Vector2{ 0, 0 }, 0.0f, WHITE);
            }
        }
    }
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


    Texture2D* tex = LoadedTextures::LoadTex("Resources/Images/itemslot.png");
    float scale = 0.13f;
    Vector2 startPos = { 400.0f, 400.0f };
    itemslots.resize(7);
    float spacing = 65.0f;
    float totalWidth = (itemslots.size() - 1 ) * spacing;
    startPos.x -= totalWidth / 2.0f;

    for (int i = 0; i < itemslots.size(); i++) {
        Vector2 pos = { startPos.x + i * spacing, startPos.y };
        itemslots[i] = CreateItemSlot(*tex, pos, scale, true);
        LoadedTextures::LoadTex(player.GetInventory().GetSlot(i, 0).item.GetTexture());
    }

}

static void UpdateItemSlot(Itemslot& slot, bool& mouseOverAny) {
    mousePoint = GetMousePosition();

    if (CheckCollisionPointRec(mousePoint, slot.bounds)) {
        mouseOverAny = true;
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            for (auto& q : itemslots) if (q.state == 1) q.state = 0;
            slot.state = 1;
        }
    }

    slot.sourceRec.y = slot.state * slot.frameHeight;
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

    bool mouseOverAny = false;
    for (int i = 0; i < itemslots.size(); i++) DrawItemSlot(itemslots[i], i);
    for (auto& e : itemslots) UpdateItemSlot(e, mouseOverAny);

    bool usedJoystick = AndroidInput::Joystick();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !usedJoystick && !mouseOverAny) {
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