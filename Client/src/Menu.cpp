#include "Menu.h"
#include "AndroidInput.h"
#include <vector>

const int screenWidth = 800;
const int screenHeight = 450;

static Camera2D camera;
static float timeValue = 0.0f;
static Vector2 mousePoint = { 0.0f, 0.0f };

static Button newGameBtn;
static Button joinGameBtn;
static Button settingsBtn;
static Button playBtn;
static Button backBtn;
static Textfield serverIpTxtField;
static Textfield playerNamepTxtField;

Font TextFont;

static std::vector<Button*> buttons;
static std::vector<Textfield*> textfields;

void Menu::UnInit()
{
    for (auto b : buttons)UnloadTexture(b->texture);
    for (auto b : textfields)UnloadTexture(b->texture);
    Client::Close();
}

std::string Menu::GetName() { return "Menu"; }

static void InitButton(Button &btn, const char* path, Vector2 pos, float scale, bool enabled) {
    btn.texture = LoadTexture(path);
    btn.frameHeight = (float)btn.texture.height / NUM_FRAMES;
    pos.x -= btn.texture.width * scale / 2.0f;
    pos.y -= btn.frameHeight * scale / 2.0f;
    btn.sourceRec = { 0, 0, (float)btn.texture.width, btn.frameHeight };
    btn.bounds = { pos.x, pos.y, btn.texture.width * scale, btn.frameHeight * scale };
    btn.enabled = enabled;
    btn.state = 0;
    buttons.push_back(&btn);
}

static void InitTextfield(Textfield &txtfld, const char* path, Vector2 pos, float scale, bool enabled) {
    txtfld.texture = LoadTexture(path);
    txtfld.frameHeight = (float)txtfld.texture.height / NUM_FRAMES_TEXTFIELD;
    pos.x -= txtfld.texture.width * scale / 2.0f;
    pos.y -= txtfld.frameHeight * scale / 2.0f;
    txtfld.sourceRec = { 0, 0, (float)txtfld.texture.width, txtfld.frameHeight };
    txtfld.bounds = { pos.x, pos.y, txtfld.texture.width * scale, txtfld.frameHeight * scale };
    txtfld.enabled = enabled;
    txtfld.state = 0;
    textfields.push_back(&txtfld);
}

static void UpdateButton(Button &btn) {
    mousePoint = GetMousePosition();

    if (CheckCollisionPointRec(mousePoint, btn.bounds)) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) btn.state = 2;
        else btn.state = 1;

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && btn.onClick) {
            btn.onClick();
        }
    }
    else btn.state = 0;

    btn.sourceRec.y = btn.state * btn.frameHeight;
}

static void UpdateTextfield(Textfield &txtfld) {
    mousePoint = GetMousePosition();

    bool hovered = CheckCollisionPointRec(mousePoint, txtfld.bounds);
    if (hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        txtfld.focused = true;
    } else if (!hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        txtfld.focused = false;
    }

    if (txtfld.focused) {

        // Maybe make this better or sum
        txtfld.text.resize(10000);
        AndroidInput::GetSoftKeyboardInput(txtfld.text, 10000);
        DrawText(txtfld.text.c_str(), 150, 10, 25, WHITE);

        int key = GetCharPressed();
        while (key > 0) {
            if (key >= 32 && key <= 125) {
                txtfld.text.push_back((char)key);
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && !txtfld.text.empty()) {
            txtfld.text.pop_back();
        }

        if (IsKeyPressed(KEY_ENTER)) {
            txtfld.focused = false;
        }

        if ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_C)) {
            SetClipboardText(txtfld.text.c_str());
        }

        if ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_V)) {
            const char* clipText = GetClipboardText();
            if (clipText) {
                txtfld.text += clipText;
            }
        }  
    }
    else
        AndroidInput::HideSoftKeyboard();

    if (txtfld.focused) {
        txtfld.state = 3;
        AndroidInput::ShowAndroidKeyboard();
    } else if (hovered && txtfld.text.empty()) {
        txtfld.state = 1;
    } else if (!txtfld.text.empty()) {
        txtfld.state = 2;
    } else {
        txtfld.state = 0;
    }

    txtfld.sourceRec.y = txtfld.state * txtfld.frameHeight;
}

static void DrawButton(const Button &btn) {
    DrawTexturePro(btn.texture, btn.sourceRec, btn.bounds, Vector2{0, 0}, 0.0f, WHITE);
}

static void DrawTextfield(const Textfield &txtfld) {
    DrawTexturePro(txtfld.texture, txtfld.sourceRec, txtfld.bounds, Vector2{0, 0}, 0.0f, WHITE);

    float maxFontSize = 40;
    float minFontSize = 10;
    float spacing = 1.0f;
    float fontSize = maxFontSize;

    std::string displayText = txtfld.text;


    while (fontSize > minFontSize &&
           MeasureTextEx(TextFont, displayText.c_str(), fontSize, spacing).x > txtfld.bounds.width - 10) {
        fontSize -= 1.0f;
    }

    if (displayText.empty()) fontSize = maxFontSize;

    Vector2 textSize = MeasureTextEx(TextFont, displayText.empty() ? "_" : displayText.c_str(), fontSize, spacing);
    Vector2 textPos = {
        txtfld.bounds.x + (txtfld.bounds.width - textSize.x) / 2.0f,
        txtfld.bounds.y + (txtfld.bounds.height - textSize.y) / 2.0f
    };

    if (!displayText.empty()) {
        DrawTextEx(TextFont, displayText.c_str(), textPos, fontSize, spacing, Color{ 131, 144, 176, 255 });
    }
    else if (txtfld.focused) {
        DrawTextEx(TextFont, "_", textPos, fontSize, spacing, Color{ 131, 144, 176, 255 });
    }
}

void Menu::Init()
{
    camera.target = Vector2{ 0.0f, 0.0f };

    TextFont = LoadFontEx("Resources/fredoka-one.ttf", 100, nullptr, 0);
    InitButton(newGameBtn,  "Resources/Images/newgame.png",  { screenWidth/2.0f, screenHeight/2.0f - 110 }, 0.5f , true);
    InitButton(joinGameBtn, "Resources/Images/joingame.png", { screenWidth/2.0f, screenHeight/2.0f - 50  }, 0.5f , true);
    InitButton(settingsBtn, "Resources/Images/settings.png", { screenWidth/2.0f, screenHeight/2.0f + 10  }, 0.5f , true);
    InitButton(playBtn, "Resources/Images/play.png", { screenWidth/2.0f - 130, screenHeight/2.0f - 50 }, 0.5f , false);
    InitButton(backBtn, "Resources/Images/back.png", { screenWidth/2.0f - 145, screenHeight/2.0f + 10 }, 0.5f , false);

    InitTextfield(serverIpTxtField, "Resources/Images/serverip.png", { screenWidth/2.0f + 40, screenHeight/2.0f + 10 }, 0.5f , false);
    InitTextfield(playerNamepTxtField, "Resources/Images/playername.png", { screenWidth/2.0f + 57, screenHeight/2.0f - 50 }, 0.5f , false);

    newGameBtn.onClick = []() { std::cout << "Starting new game...\n"; };
    joinGameBtn.onClick = []() {
        playBtn.enabled = true;
        backBtn.enabled = true;
        playerNamepTxtField.enabled = true;
        serverIpTxtField.enabled = true;
        settingsBtn.enabled = false;
        joinGameBtn.enabled = false;
        newGameBtn.enabled = false;
    };
    settingsBtn.onClick = []() { std::cout << "Opening settings...\n"; };
    playBtn.onClick = []() {
        Client::SetIp(serverIpTxtField.text);
        Client::Init();
    };
    backBtn.onClick = []() { 
        serverIpTxtField.text = playerNamepTxtField.text = "";        
        playBtn.enabled = false;
        backBtn.enabled = false;
        playerNamepTxtField.enabled = false;
        serverIpTxtField.enabled = false;
        settingsBtn.enabled = true;
        joinGameBtn.enabled = true;
        newGameBtn.enabled = true;

    };

    Client::onConnected = []() {
        CatCore::ServerUtils::SendMessage(Client::GetPeer(), "!name " + playerNamepTxtField.text);
        CurrentState::SetState(new Game);
    };
}

void Menu::Update()
{
    Client::Run();
    timeValue += GetFrameTime();
    for (auto* b : buttons) if (b->enabled) UpdateButton(*b);
    for (auto* t : textfields) if (t->enabled) UpdateTextfield(*t);

    BeginDrawing();
        ClearBackground(SKYBLUE);
        for (auto* b : buttons) if (b->enabled) DrawButton(*b);
        for (auto* t : textfields) if (t->enabled) DrawTextfield(*t);
        BeginMode2D(camera);
        EndMode2D();
        DrawFPS(10, 10);
    EndDrawing();
}
