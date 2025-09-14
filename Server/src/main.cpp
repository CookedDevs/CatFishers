#include <iostream>
#include "Server.h"
#include "Game.h"

int main()
{
    Server::Init();
    Game::Init();

    while (true)
    {
        Server::Run();
        Game::Update();
    }
    
    Game::UnInit();
    Server::Close();
    return 1;
}
