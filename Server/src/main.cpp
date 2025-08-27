#include <iostream>
#include "Server.h"

int main()
{
    Server server;

    while (true)
    {
        server.Run();
    }
    
    return 1;
}
