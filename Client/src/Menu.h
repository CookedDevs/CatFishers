#pragma once
#include "State.h"
#include "Client.h"
#include "Game.h"
#include "ServerUtils.h"
#include <string>
#include <functional>
#include <condition_variable>
#include <iostream>
#include <raylib.h>
#include <math.h>

#define NUM_FRAMES  3 
#define NUM_FRAMES_TEXTFIELD  4

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
