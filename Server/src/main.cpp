#include <iostream>
#include "Server.h"

int main()
{
    Server::Init();

    

    while (true)
        Server::Run();
    
    Server::Close();
    return 1;
}
