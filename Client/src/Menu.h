#pragma once
#include "State.h"
#include <raylib.h>
#include <string>
#include <functional>

struct Button {
    Texture2D texture;
    Rectangle bounds;
    Rectangle sourceRec;
    float frameHeight;
    int state;
    bool enabled;
    std::function<void()> onClick;
};

struct Textfield {
    Texture2D texture;
    Rectangle bounds;
    Rectangle sourceRec;
    float frameHeight;
    int state;
    bool focused = false;
    bool enabled = true;
    std::string text;
};


class Menu : public State
{
public:
    Menu() {}
    ~Menu() {}

    void UnInit() override;
    void Init() override;
    void Update() override;
    std::string GetName() override;

private:
};
