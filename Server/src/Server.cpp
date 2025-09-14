#include "Server.h"
#include <iostream>
#include <CatCore.h>

#include <chrono>
#include <thread>

#include <nlohmann/json.hpp>

#include "ServerCommands.h"
#include "CatMath.h"

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
    serverHost = enet_host_create(ENET_ADDRESS_TYPE_IPV6, &address, 32, 2, 0, 0);
    if (serverHost == NULL)
    {
        std::cerr << "An error occured while trying to create an ENet6 server host!\n";
        exit(EXIT_FAILURE);
    }
    else
        std::cout << "ENet server host started\n\n\n";

    enet_host_bandwidth_throttle(serverHost);

    /* Connect and user service */
    eventStatus = 1;
}

void Server::Close()
{
    enet_host_destroy(serverHost);
}

void TestFunc()
{
    std::cout << "Test";
}

bool Server::Run()
{
    eventStatus = enet_host_service(serverHost, &event, 8);
     
    /* If we had some event that interested us */
    if (eventStatus > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
        {
            enet_address_get_host_ip(&event.peer->address, addressBuffer, ENET_ADDRESS_MAX_LENGTH);
            std::cout << "New connection : " << addressBuffer << "\n";
            AddPlayer(event.peer);
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

                if (GetPlayer(event.peer)->GetName() != "")
                {
                    CatCore::Player& player = *GetPlayer(event.peer);

                    std::cout << "(" << player.GetName() << ") : " << event.packet->data << "\n";
                    BroadcastMessage("(" + player.GetName() + ") : " + (const char*)event.packet->data);
                    enet_packet_destroy(event.packet);
                }
                else
                {
                    CatCore::ServerUtils::SendMessage(event.peer, "To send messages create a name !n or !name");
                }
            }
            else if (event.packet->data[0] == CatCore::ServerReceiveType::Data)
            {
                std::vector<char> inputs;
                inputs.resize(event.packet->dataLength - 1);
                for (size_t i = 1; i < event.packet->dataLength; i++)
                    inputs[i - 1] = event.packet->data[i];

                if (inputs.size() >= 2)
                {
                    std::cout << inputs[0] << " Is : " << (bool)inputs[1] << "\n";
                    GetPlayer(event.peer)->SetInputInfo(inputs[0], inputs[1]);
                }
            }
            else if (event.packet->data[0] == CatCore::ServerReceiveType::Function)
            {
                char* buffer = (char*)event.packet->data;
                unsigned int offset = 1;

                uint16_t funcId;
                CatCore::ServerUtils::readFromBuffer(buffer, offset, &funcId, sizeof(funcId));
                
                switch (funcId)
                {
                case 1:
                    TestFunc();
                    break;
                }
            }
            break;

        case ENET_EVENT_TYPE_DISCONNECT:
        case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
        {
            enet_address_get_host_ip(&event.peer->address, addressBuffer, ENET_ADDRESS_MAX_LENGTH);
            std::cout << "Client " << addressBuffer << " disconnected " << (event.type == ENET_EVENT_TYPE_DISCONNECT_TIMEOUT) ? " (timeout)" : "";
            RemovePlayer(event.peer);
            break;
        }
        }
    }

    for (auto& player : players)
    {
        CatCore::Vector3 position = player.second.GetPosition();
        bool changed = false;

        if (player.second.IsKeyDown('A')) { position.x -= 1.f; changed = true; }
        if (player.second.IsKeyDown('D')) { position.x += 1.f; changed = true; }
        if (player.second.IsKeyDown('W')) { position.y -= 1.f; changed = true; }
        if (player.second.IsKeyDown('S')) { position.y += 1.f; changed = true; }

        if (changed) player.second.SetPosition(position);
    }

    SendPlayerAddOrRemove();
    SendPlayers();
    SendScene();

    enet_host_flush(serverHost);
    return true;
}

void Server::SendPlayerAddOrRemove()
{
    const size_t bufferSize = 8192;
    char buffer[bufferSize];
    unsigned int offset = 0;

    uint8_t messageType = CatCore::PlayerAddOrRemove;
    CatCore::ServerUtils::writeToBuffer(buffer, offset, &messageType, sizeof(messageType));

    uint8_t playerCount = playersToAddOrRemove.size();
    CatCore::ServerUtils::writeToBuffer(buffer, offset, &playerCount, sizeof(playerCount));

    for (auto player : playersToAddOrRemove)
    {
        CatCore::ServerUtils::writeToBuffer(buffer, offset, &player.second, sizeof(bool));
        if (player.second)
        {
            CatCore::Player* removePlayer = GetPlayer(player.first);
            CatCore::ServerUtils::writeToBuffer(buffer, offset, removePlayer->GetName().c_str());
        }
        else
        {
            CatCore::Player* addPlayer = GetPlayer(player.first);
            CatCore::ServerUtils::writeToBuffer(buffer, offset, addPlayer->GetName().c_str());
            CatCore::ServerUtils::writeToBuffer(buffer, offset, addPlayer->GetTexture().c_str());
            CatCore::ServerUtils::serializeVector3(buffer, offset, addPlayer->position);
        }
    }

    ENetPacket* packet = enet_packet_create(buffer, offset, ENET_PACKET_FLAG_RELIABLE);
    enet_host_broadcast(serverHost, 1, packet);
}

void Server::SendPlayers()
{
    const size_t bufferSize = 8192;
    char buffer[bufferSize];
    unsigned int offset = 0;

    std::vector<CatCore::Player> playerDataToSend;
    for (auto player : players)
    { 
        if (player.second.GetChanged() == true) 
        {
            playerDataToSend.push_back(player.second); 
            player.second.SetChanged(false); 
        }
    }

    uint8_t messageType = CatCore::PlayerData;
    CatCore::ServerUtils::writeToBuffer(buffer, offset, &messageType, sizeof(messageType));

    uint8_t playerCount = playerDataToSend.size();
    CatCore::ServerUtils::writeToBuffer(buffer, offset, &playerCount, sizeof(playerCount));

    for (auto player : playerDataToSend)
    {
        CatCore::ServerUtils::writeToBuffer(buffer, offset, player.GetName().c_str());
        CatCore::ServerUtils::writeToBuffer(buffer, offset, player.GetTexture().c_str());
        CatCore::ServerUtils::serializeVector3(buffer, offset, player.position);
    }

    ENetPacket* packet = enet_packet_create(buffer, offset, ENET_PACKET_FLAG_RELIABLE);
    enet_host_broadcast(serverHost, 1, packet);
}

void Server::SendScene()
{
    const size_t bufferSize = 8192;
    char buffer[bufferSize];
    unsigned int offset = 0;

    uint8_t messageType = CatCore::Data;
    CatCore::ServerUtils::writeToBuffer(buffer, offset, &messageType, sizeof(messageType));

    uint8_t spriteCount = sprites.size();
    CatCore::ServerUtils::writeToBuffer(buffer, offset, &spriteCount, sizeof(spriteCount));

    for (auto sprite : sprites)
        sprite.second.serialize(buffer, offset);

    ENetPacket* packet = enet_packet_create(buffer, offset, ENET_PACKET_FLAG_RELIABLE);
    enet_host_broadcast(serverHost, 1, packet);
}

void Server::SendPlayerData(const std::vector<uint8_t> data)
{
    uint8_t sendType = (uint8_t)CatCore::ServerReceiveType::PlayerData;
    std::vector<uint8_t> send;
    send.push_back(sendType);
    send.insert(send.end(), data.begin(), data.end());

    ENetPacket* packet = enet_packet_create(send.data(), send.size() + 1, ENET_PACKET_FLAG_RELIABLE);
    enet_host_broadcast(serverHost, 0, packet);
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

void Server::BroadcastExludeMessage(ENetPeer* excludedReseiver, const std::string message, CatCore::ServerReceiveType type)
{
    char sendType = type;
    std::string send;
    send.push_back(sendType);
    send += message;

    ENetPacket* packet = enet_packet_create(send.c_str(), send.size() + 1, ENET_PACKET_FLAG_RELIABLE);
    
    for (auto player : players)
    {
        if (player.first == excludedReseiver)
            continue;
        enet_peer_send(player.first, 0, packet);
    }
}