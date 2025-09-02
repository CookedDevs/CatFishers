#include "Client.h"
#include "Commands.h"
#include "Player.h"
#include "ClientCommands.h"
#include "Game.h"

void Client::Init()
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
    clientHost = enet_host_create(address.type, NULL, 1, 0, 0, 0);
    if (clientHost == NULL)
    {
        std::cerr << "An error occured while trying to create an ENet6 server host\n";
        exit(EXIT_FAILURE);
    }
    enet_host_bandwidth_throttle(clientHost);

    /* Connect and user service */
    serverPeer = enet_host_connect(clientHost, &address, 2, 0);
    if (serverPeer == NULL)
    {
        std::cerr << "No available peers for initializing an ENet connection";
        exit(EXIT_FAILURE);
    }

}

void Client::Close()
{

}

bool Client::Run()
{
    while(enet_host_service(clientHost, &event, 0) > 0)
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
            else if (event.packet->data[0] == CatCore::ServerReceiveType::PlayerData)
            {
                char* buffer = (char*)event.packet->data;
                unsigned int offset = 1;

                uint8_t playerCount;
                CatCore::ServerUtils::readFromBuffer(buffer, offset, &playerCount, sizeof(playerCount));

                for (size_t i = 0; i < playerCount; i++)
                {
                    char* name = "";
                    char* texture = "";
                    CatCore::Vector3 position;

                    CatCore::ServerUtils::readTextFromBuffer(buffer, offset, name);
                    CatCore::ServerUtils::readTextFromBuffer(buffer, offset, texture);
                    CatCore::ServerUtils::deserializeVector3(buffer, offset, position);

                    CatCore::Player plr;
                    plr.name = name;
                    plr.texture = texture;
                    plr.position = position;
                    player = plr;
                    playerTex = LoadedTextures::LoadTex(plr.texture);
                }
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
    if (serverPeer->state == ENET_PEER_STATE_CONNECTED)
    {
#ifndef __ANDROID__
        if (com.has_command())
        {
            std::string line = com.get_command();
            if (line[0] == '!' && ClientCommands::HandleCommand(line)) {}
            else if (line == "exit" || line == "quit")
            {
                running = 0;
                enet_peer_disconnect_now(serverPeer, 0);
                std::cout << "\nDisconnected from server!\n";
            }
            else
            {
                CatCore::ServerUtils::SendMessage(serverPeer, line);
            }
        }
#endif
    }

    return true;
}

void Client::SendInputData(const std::unordered_map<char, bool> inputs)
{
    uint8_t sendType = (uint8_t)CatCore::ServerReceiveType::Data;

    for (auto input : inputs)
    {
        std::vector<uint8_t> send;
        send.push_back(sendType);
        send.push_back(input.first);
        send.push_back(input.second);

        ENetPacket* packet = enet_packet_create(send.data(), send.size(), ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(serverPeer, 0, packet);
        enet_host_flush(clientHost);
    }


}

void Client::PrintLine(std::string message)
{
#ifdef __ANDROID__
    std::cout << message << "\n";
#else
    com.write(message);
#endif
}
