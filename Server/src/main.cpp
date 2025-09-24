#include <iostream>
#include "Server.h"
#include "Game.h"
#include "ServerConfig.h"

int main()
{
    ServerConfig::Init();
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
