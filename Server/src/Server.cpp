#include "Server.h"
#include <iostream>
#include <CatCore.h>

#include "ServerCommands.h"

void Server::Init()
{
    ServerCommands::InitializeCommands();
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
    //enet_address_build_any(&address, ENET_ADDRESS_TYPE_IPV6);
    enet_address_set_host(&address, ENET_ADDRESS_TYPE_IPV6, "localhost");
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

void Server::Close()
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
            std::cout << "New connection : " << addressBuffer << "\n";
            AddPlayer(CatCore::Player(), event.peer->connectID);
            ServerCommands::SendCommandInfo(event.peer);

            break;
        }
    
        case ENET_EVENT_TYPE_RECEIVE:

            if (event.packet->data[0] == CatCore::ServerReceiveType::Message)
            {
                if (event.packet->data[1] == '!')
                {
                    std::string command = (const char*)event.packet->data;
                    command.erase(0, 1);

                    ServerCommands::HandleCommand(command, event.peer);
                    break;
                }

                if (GetPlayer(event.peer->connectID).getName() != "")
                {
                    std::cout << "(" << GetPlayer(event.peer->connectID).getName() << ") : " << event.packet->data << "\n";
                    enet_host_broadcast(serverHost, 0, event.packet);
                }
                else
                {
                    SendMessage(event.peer, "To send messages create a name !n or !name");
                }
            }
            if (event.packet->data[0] == CatCore::ServerReceiveType::Data)
            {
                //if sent data;
            }
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

void Server::SendMessage(ENetPeer* receiver, const std::string message)
{
    char sendType = CatCore::ServerReceiveType::Message;
    std::string send;
    send.push_back(sendType);
    send += message;

    ENetPacket* packet = enet_packet_create(send.c_str(), send.size() + 1, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(receiver, 0, packet);
}

void Server::SendData(ENetPeer* receiver, const std::vector<std::byte> data)
{
    std::byte sendType = (std::byte)CatCore::ServerReceiveType::Data;
    std::vector<std::byte> send;
    send.push_back(sendType);
    send.insert(send.end(), data.begin(), data.end());

    ENetPacket* packet = enet_packet_create(&send[0], send.size() + 1, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(receiver, 0, packet);
}

void Server::SendCommandData(ENetPeer* receiver, const std::string message)
{
    char sendType = CatCore::ServerReceiveType::CommandData;
    std::string send;
    send.push_back(sendType);
    send += message;

    ENetPacket* packet = enet_packet_create(send.c_str(), send.size() + 1, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(receiver, 0, packet);
}

void Server::BroadcastMessage(const std::string message)
{
    char sendType = CatCore::ServerReceiveType::Message;
    std::string send;
    send.push_back(sendType);
    send += message;

    ENetPacket* packet = enet_packet_create(send.c_str(), send.size() + 1, ENET_PACKET_FLAG_RELIABLE);
    enet_host_broadcast(serverHost, 0, packet);
}

void Server::BroadcastExludeMessage(ENetPeer* excludedReseiver, const std::string message)
{
    char sendType = CatCore::ServerReceiveType::Message;
    std::string send;
    send.push_back(sendType);
    send += message;

    ENetPacket* packet = enet_packet_create(send.c_str(), send.size() + 1, ENET_PACKET_FLAG_RELIABLE);
    enet_host_broadcast(serverHost, 0, packet);
}