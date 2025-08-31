#pragma once
#include <string>
#include <iostream>

class State
{
public:
    virtual ~State() = default;

    virtual void Init() = 0;
    virtual void Update() = 0;
    virtual std::string GetName() = 0;
};

class CurrentState
{
public:
    static void SetState(State* state)
    {
        if (currState != nullptr)
        {
            delete currState;
            currState = state;
        }
        else
        {
            currState = state;
            currState->Init();
        }

        if (state->GetName() != currState->GetName())
            currState->Init();
    }
    static void UpdateState() { currState->Update(); }
    static void DeleteCurrentState() { delete currState; }
private:
    static inline State* currState = nullptr;
};