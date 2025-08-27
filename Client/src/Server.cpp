#include "Server.h"
#include <iostream>

Server::Server()
{
    /* Initialize enet6 */
    if (enet_initialize() != 0)
    {
        std::cerr << "An error occured while initializing ENet6!\n";
        exit(EXIT_FAILURE);
    }

    atexit(enet_deinitialize);

    /* Build an IPv6 or IPv4 address, depending on what the domain resolves to */
    enet_address_set_host(&address, ENET_ADDRESS_TYPE_ANY, "localhost");
    address.port = 1234;

    enet_address_get_host_ip(&address, addressBuffer, ENET_ADDRESS_MAX_LENGTH);
    std::cout << "Connecting to " << addressBuffer << "\n";

    /* Create a non-listening host using enet_host_create */
    /* Note we create a host using the same address family we resolved earlier */
    clientHost = enet_host_create(address.type, NULL, 1, 2, 0, 0);
    if (clientHost == NULL)
    {
        std::cerr << "An error occured while trying to create an ENet6 server host\n";
        exit(EXIT_FAILURE);
    }

    /* Connect and user service */
    serverPeer = enet_host_connect(clientHost, &address, 2, 0);
    if (serverPeer == NULL)
    {
        std::cerr << "No available peers for initializing an ENet connection";
        exit(EXIT_FAILURE);
    }

}

Server::~Server()
{
}

bool Server::Run()
{
    eventStatus = enet_host_service(clientHost, &event, 100);

    /* Inspect events */
    if (eventStatus > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
            std::cout << "\n(Client) Connected to server " << addressBuffer << "\n\n";
            break;

        case ENET_EVENT_TYPE_RECEIVE:
            std::cout << "(Client) Message from server: " << event.packet->data << "";
            enet_packet_destroy(event.packet);
            break;

        case ENET_EVENT_TYPE_DISCONNECT:
            std::cout << "(Client) Disconnected from server\n";
            return EXIT_SUCCESS;

        case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
            std::cout << "(Client) Disconnected from server (timed out)\n";
            return EXIT_FAILURE;
        }
    }
    else if (serverPeer->state == ENET_PEER_STATE_CONNECTED)
    {
        /* Prompt some message to send to the server, be quick to prevent timeout (TODO: Read asynchronously) */
        std::cout << "> ";
        fgets(message, sizeof(message), stdin);

        if (strlen(message) > 0 && strcmp(message, "\n") != 0)
        {
            /* Build a packet passing our bytes, length and flags (reliable means this packet will be resent if lost) */
            ENetPacket* packet = enet_packet_create(message, strlen(message) + 1, ENET_PACKET_FLAG_RELIABLE);
            /* Send the packet to the server on channel 0 */
            enet_peer_send(serverPeer, 0, packet);
        }
        else
        {
            running = 0;
            enet_peer_disconnect_now(serverPeer, 0);
            std::cout << "\nDisconnected from server\n";
        }
    }
}