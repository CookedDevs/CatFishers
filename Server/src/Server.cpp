#include "Server.h"
#include <iostream>

Server::Server()
{
    std::cout << "Starting ENet server host .....\n";
    
    /* Initialize enet6 */
    if (enet_initialize() != 0)
    {
        std::cerr << "An error occured while initializing ENet6!\n";
        exit(EXIT_FAILURE);
    }
    else
        std::cout << "ENet initialized\n";

    atexit(enet_deinitialize);

    /* Build the listen address (any + port) */
    enet_address_build_any(&address, ENET_ADDRESS_TYPE_IPV6);
    address.port = 1234;

    /* Create a host using enet_host_create, address type has to match the address,  */
    /* except for the combination IPv6 + Any which enables dual stack (IPv6 socket allowing IPv4 connection)  */
    serverHost = enet_host_create(ENET_ADDRESS_TYPE_ANY, &address, 32, 2, 0, 0);
    if (serverHost == NULL)
    {
        std::cerr << "An error occured while trying to create an ENet6 server host!\n";
        exit(EXIT_FAILURE);
    }
    else
        std::cout << "ENet server host started\n\n\n";

    /* Connect and user service */
    eventStatus = 1;
}

Server::~Server()
{
    enet_host_destroy(serverHost);
}

bool Server::Run()
{
    eventStatus = enet_host_service(serverHost, &event, 100);
    
    /* If we had some event that interested us */
    if (eventStatus > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
        {
            enet_address_get_host_ip(&event.peer->address, addressBuffer, ENET_ADDRESS_MAX_LENGTH);
            std::cout << "(Server) We got a new connection from " << addressBuffer << "\n";
            break;
        }
    
        case ENET_EVENT_TYPE_RECEIVE:
            std::cout << "(Server) Message from client : " << event.packet->data << "\n";
            enet_host_broadcast(serverHost, 0, event.packet);
            break;
    
        case ENET_EVENT_TYPE_DISCONNECT:
        case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
        {
            enet_address_get_host_ip(&event.peer->address, addressBuffer, ENET_ADDRESS_MAX_LENGTH);
            std::cout << "Client " << addressBuffer << " disconnected " << (event.type == ENET_EVENT_TYPE_DISCONNECT_TIMEOUT) ? " (timeout)" : "";
            break;
        }
        }
    }
    return true;
}