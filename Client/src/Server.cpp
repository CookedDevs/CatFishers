#include "Server.h"
#include "Commands.h"
#include "Player.h"

#include "ClientCommands.h"

Server::Server()
{
    ClientCommands::InitializeCommands();

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
    eventStatus = enet_host_service(clientHost, &event, 5);

    /* Inspect events */
    if (eventStatus > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
            std::cout << "\nConnected to server!" << addressBuffer << "\n\n";
            break;

        case ENET_EVENT_TYPE_RECEIVE:
            if (event.packet->data[0] == CatCore::ServerReceiveType::Message)
            {
                std::cout << event.packet->data << "\n";
                enet_packet_destroy(event.packet);
            }
            else if (event.packet->data[0] == CatCore::ServerReceiveType::CommandData)
            {
                std::string recv = (const char*)event.packet->data;
                recv.erase(0, 1);
                ClientCommands::AddServerCommands(recv);
            }
            else if (event.packet->data[0] == CatCore::ServerReceiveType::Data)
            {

            }

            break;

        case ENET_EVENT_TYPE_DISCONNECT:
            std::cout << "Disconnected from server!\n";
            ClientCommands::ClearServerCommands();
            return EXIT_SUCCESS;

        case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
            std::cout << "Disconnected from server (timed out)!\n";
            return EXIT_FAILURE;
        }
    }
    else if (serverPeer->state == ENET_PEER_STATE_CONNECTED)
    {
        std::cout << "> ";

        char sendType = CatCore::ServerReceiveType::Message;

        std::string line;
        std::getline(std::cin, line);
        if (!line.empty()) // user typed something (not just Enter)
        {
            std::string send;

            if (line[0] == '!' && ClientCommands::HandleCommand(line)) {}
            else if (line == "exit" || line == "quit")
            {
                running = 0;
                enet_peer_disconnect_now(serverPeer, 0);
                std::cout << "\nDisconnected from server!\n";
            }
            else
            {
                send.push_back(sendType);
                send += line;

                ENetPacket* packet = enet_packet_create(send.c_str(), send.size() + 1, ENET_PACKET_FLAG_RELIABLE);
                enet_peer_send(serverPeer, 0, packet);
            }
        }
    }
}