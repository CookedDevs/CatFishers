#include <iostream>
#include "Server.h"
#include "Game.h"
#include "ServerConfig.h"

#include <signal.h>


int execute;
void trap(int signal) { execute = 0; }

int main()
{
    signal(SIGINT, &trap);
    execute = 1;

    ServerConfig::Init();
    Server::Init();
    Game::Init();

    while (execute)
    {
        Server::Run();
        Game::Update();
    }
    signal(SIGINT, SIG_DFL);
    
    Game::UnInit();
    Server::Close();
    return 1;
}
