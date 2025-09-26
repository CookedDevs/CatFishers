#include "Server.h"
#include <iostream>
#include <CatCore.h>

#include <chrono>
#include <thread>
#include <algorithm>

#include <nlohmann/json.hpp>

#include "ServerCommands.h"
#include "CatMath.h"
#include "Game.h"
#include "ServerConfig.h"

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

    ENetAddressType type = ENET_ADDRESS_TYPE_IPV4;
    if (ServerConfig::GetIsIPv6()) type = ENET_ADDRESS_TYPE_IPV6;

    enet_address_set_host(&address, type, ServerConfig::GetIP().c_str());
    address.port = ServerConfig::GetPort();

    /* Create a host using enet_host_create, address type has to match the address,  */
    /* except for the combination IPv6 + Any which enables dual stack (IPv6 socket allowing IPv4 connection)  */
    serverHost = enet_host_create(type, &address, 32, 2, 0, 0);
    if (serverHost == NULL)
    {
        std::cerr << "An error occured while trying to create an ENet6 server host!\n";
        exit(EXIT_FAILURE);
    }
    else
        std::cout << "ENet server host started\n\n\n";


    std::cout << ServerConfig::GetIP() << " " << ServerConfig::GetPort() << " " << ServerConfig::GetIsIPv6() << "\n";

    enet_host_bandwidth_throttle(serverHost);
    eventStatus = 1;
}

void Server::Close()
{
    for (auto player : players)
    {
        std::string UUID = GetPeerUUID(player.first);
        if (!ServerConfig::CheckUUID(UUID)) continue;

        ServerConfig::SetPlayerData(GetPeerUUID(player.first), *GetPlayer(player.first));
        ServerConfig::Save();
    }
    enet_host_destroy(serverHost);
}

bool Server::Run()
{
    eventStatus = enet_host_service(serverHost, &event, 8);
    if (eventStatus > 0)
    {
        switch (event.type) 
        {
        case ENET_EVENT_TYPE_CONNECT:
        {
            enet_address_get_host_ip(&event.peer->address, addressBuffer, ENET_ADDRESS_MAX_LENGTH);
            std::cout << "New connection : " << addressBuffer << "\n";
            ServerCommands::SendCommandInfo(event.peer);
            break;
        }

        case ENET_EVENT_TYPE_RECEIVE:
        {

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
                char* buffer = (char*)event.packet->data;
                unsigned int offset = 1;

                uint8_t inputCount;
                CatCore::ServerUtils::readFromBuffer(buffer, offset, &inputCount, sizeof(inputCount));

                std::unordered_map<char, bool> inputs;
                for (size_t i = 0; i < inputCount; i++)
                {
                    char input;
                    bool status;
                    
                    CatCore::ServerUtils::readFromBuffer(buffer, offset, &input, sizeof(input));
                    CatCore::ServerUtils::readFromBuffer(buffer, offset, &status, sizeof(status));

                    inputs[input] = status;
                }

                for (auto input : inputs)
                    GetPlayer(event.peer)->SetInputInfo(input.first, input.second);

                CatCore::Mouse mouse;
                CatCore::ServerUtils::readFromBuffer(buffer, offset, &mouse.input, sizeof(mouse.input));
                CatCore::ServerUtils::readFromBuffer(buffer, offset, &mouse.x, sizeof(mouse.x));
                CatCore::ServerUtils::readFromBuffer(buffer, offset, &mouse.y, sizeof(mouse.y));
                GetPlayer(event.peer)->SetMouse(mouse);

                ENetPacket* packet = enet_packet_create(buffer, offset, ENET_PACKET_FLAG_RELIABLE);
            }
            else if (event.packet->data[0] == CatCore::ServerReceiveType::Function)
            {
                char* buffer = (char*)event.packet->data;
                unsigned int offset = 1;

                uint16_t funcId;
                CatCore::ServerUtils::readFromBuffer(buffer, offset, &funcId, sizeof(funcId));
                
                switch (funcId)
                {
                    case 1: {
                        Game::ThrowBobber(event.peer);
                        break;
                    }
                    case 2:{
                        Game::ReelBobber(event.peer);
                        break;
                    }
                    default : {
                        break;
                    }
                }
                enet_packet_destroy(event.packet);
            }
            else if (event.packet->data[0] == CatCore::ServerReceiveType::PeerUUID)
            {
                char* buffer = (char*)event.packet->data;
                unsigned int offset = sizeof(uint8_t);

                char* text;
                CatCore::ServerUtils::readTextFromBuffer(buffer, offset, text);
                
                peerUUIDs[event.peer] = text;
            }
            break;
        }
        case ENET_EVENT_TYPE_DISCONNECT:
        case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
        {
            enet_address_get_host_ip(&event.peer->address, addressBuffer, ENET_ADDRESS_MAX_LENGTH);

            std::cout << "Client " << addressBuffer 
                    << " disconnected" 
                    << ((event.type == ENET_EVENT_TYPE_DISCONNECT_TIMEOUT) ? " (timeout)" : "") 
                    << "\n";

            ServerConfig::SetPlayerData(GetPeerUUID(event.peer), *GetPlayer(event.peer));
            ServerConfig::Save();
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

    if (runplayersToAddOrRemove) SendPlayerAddOrRemove();
    SendPlayers();

    if (runspritesToAddOrRemove) SendSpriteAddOrRemove();
    SendSprites();

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
        CatCore::Player* thisPlayer = GetPlayer(player.first);

        if (player.second)
        {
            CatCore::ServerUtils::writeToBuffer(buffer, offset, thisPlayer->GetName().c_str());
            players.erase(player.first);
        }
        else
        {
            CatCore::ServerUtils::writeToBuffer(buffer, offset, thisPlayer->GetName().c_str());
            CatCore::ServerUtils::writeToBuffer(buffer, offset, thisPlayer->GetTexture().c_str());
            CatCore::ServerUtils::serializeVector3(buffer, offset, thisPlayer->GetPosition());

            thisPlayer->GetInventory().serialize(buffer, offset);
            thisPlayer->GetInventory().SetChanged(false);
        }
    }

    ENetPacket* packet = enet_packet_create(buffer, offset, ENET_PACKET_FLAG_RELIABLE);
    enet_host_broadcast(serverHost, 1, packet);
    runplayersToAddOrRemove = false;
    playersToAddOrRemove.clear();
}

void Server::SendPlayers()
{
    const size_t bufferSize = 8192;
    char buffer[bufferSize];
    unsigned int offset = 0;

    std::vector<std::pair<CatCore::Player, bool>> playerDataToSend;
    for (auto& player : players)
    { 
        if (player.second.GetChanged() == true && player.second.GetName() != "")
        {
            playerDataToSend.push_back({player.second, player.second.GetInventory().GetChanged()});
            player.second.SetChanged(false);
            player.second.GetInventory().SetChanged(false);
        }
    }

    uint8_t messageType = CatCore::PlayerData;
    CatCore::ServerUtils::writeToBuffer(buffer, offset, &messageType, sizeof(messageType));

    uint8_t playerDataCount = playerDataToSend.size();
    CatCore::ServerUtils::writeToBuffer(buffer, offset, &playerDataCount, sizeof(playerDataCount));

    for (auto player : playerDataToSend)
    {
        CatCore::ServerUtils::writeToBuffer(buffer, offset, player.first.GetName().c_str());
        CatCore::ServerUtils::writeToBuffer(buffer, offset, player.first.GetTexture().c_str());
        CatCore::ServerUtils::serializeVector3(buffer, offset, player.first.GetPosition());

        CatCore::ServerUtils::writeToBuffer(buffer, offset, &player.second, sizeof(player.second));
        if (player.second) 
            player.first.GetInventory().serialize(buffer, offset); 
    }

    if (playerDataCount > 0)
    {
        ENetPacket* packet = enet_packet_create(buffer, offset, ENET_PACKET_FLAG_RELIABLE);
        enet_host_broadcast(serverHost, 1, packet);
    }
}

void Server::SendSpriteAddOrRemove()
{
    const size_t bufferSize = 8192;
    char buffer[bufferSize];
    unsigned int offset = 0;

    uint8_t messageType = CatCore::SpriteAddOrRemove;
    CatCore::ServerUtils::writeToBuffer(buffer, offset, &messageType, sizeof(messageType));

    uint8_t spriteCount = spritesToAddOrRemove.size();
    CatCore::ServerUtils::writeToBuffer(buffer, offset, &spriteCount, sizeof(spriteCount));

    for (auto sprite : spritesToAddOrRemove)
    {
        CatCore::ServerUtils::writeToBuffer(buffer, offset, &sprite.second, sizeof(bool));
        CatCore::Sprite* sendSprite = GetSprite(sprite.first);

        if (sprite.second)
        {
            CatCore::ServerUtils::writeToBuffer(buffer, offset, sendSprite->GetName().c_str());
            sprites.erase(sprite.first);
        }
        else
        {
            CatCore::ServerUtils::writeToBuffer(buffer, offset, sendSprite->GetName().c_str());
            CatCore::ServerUtils::writeToBuffer(buffer, offset, sendSprite->GetTexture().c_str());

            CatCore::ServerUtils::serializeVector3(buffer, offset, sendSprite->GetPosition());
            CatCore::ServerUtils::writeToBuffer(buffer, offset, &sendSprite->GetRotation(), sizeof(sendSprite->GetRotation()));
            CatCore::ServerUtils::writeToBuffer(buffer, offset, &sendSprite->GetSize(), sizeof(sendSprite->GetSize()));
            CatCore::ServerUtils::writeToBuffer(buffer, offset, &sendSprite->GetRenderBeforePlayer(), sizeof(sendSprite->GetRenderBeforePlayer()));
        }
    }

    ENetPacket* packet = enet_packet_create(buffer, offset, ENET_PACKET_FLAG_RELIABLE);
    enet_host_broadcast(serverHost, 1, packet);

    runspritesToAddOrRemove = false;
    spritesToAddOrRemove.clear();
}

void Server::SendSprites()
{
    const size_t bufferSize = 8192;
    char buffer[bufferSize];
    unsigned int offset = 0;

    std::vector<CatCore::Sprite> spriteDataToSend;
    for (auto& sprite : sprites)
    {
        if (sprite.second.GetChanged() == true)
        {
            spriteDataToSend.push_back(sprite.second);
            sprite.second.SetChanged(false);
        }
    }

    uint8_t messageType = CatCore::SpriteData;
    CatCore::ServerUtils::writeToBuffer(buffer, offset, &messageType, sizeof(messageType));

    uint8_t spriteCount = spriteDataToSend.size();
    CatCore::ServerUtils::writeToBuffer(buffer, offset, &spriteCount, sizeof(spriteCount));

    for (auto sprite : spriteDataToSend)
    {
        CatCore::ServerUtils::writeToBuffer(buffer, offset, sprite.GetName().c_str());
        CatCore::ServerUtils::writeToBuffer(buffer, offset, sprite.GetTexture().c_str());

        CatCore::ServerUtils::serializeVector3(buffer, offset, sprite.GetPosition());
        CatCore::ServerUtils::writeToBuffer(buffer, offset, &sprite.GetRotation(), sizeof(sprite.GetRotation()));
        CatCore::ServerUtils::writeToBuffer(buffer, offset, &sprite.GetSize(), sizeof(sprite.GetSize()));
        CatCore::ServerUtils::writeToBuffer(buffer, offset, &sprite.GetRenderBeforePlayer(), sizeof(sprite.GetRenderBeforePlayer()));
    }

    if (spriteCount > 0)
    {
        ENetPacket* packet = enet_packet_create(buffer, offset, ENET_PACKET_FLAG_RELIABLE);
        enet_host_broadcast(serverHost, 1, packet);
    }
}

void Server::SendScene(ENetPeer* peer)
{
    const size_t bufferSize = 8192;
    char buffer[bufferSize];
    unsigned int offset = 0;

    uint8_t messageType = CatCore::SceneData;
    CatCore::ServerUtils::writeToBuffer(buffer, offset, &messageType, sizeof(messageType));

    uint8_t playerDataCount = players.size();
    CatCore::ServerUtils::writeToBuffer(buffer, offset, &playerDataCount, sizeof(playerDataCount));

    for (auto player : players)
    {
        CatCore::ServerUtils::writeToBuffer(buffer, offset, player.second.GetName().c_str());
        CatCore::ServerUtils::writeToBuffer(buffer, offset, player.second.GetTexture().c_str());
        CatCore::ServerUtils::serializeVector3(buffer, offset, player.second.GetPosition());
        player.second.GetInventory().serialize(buffer, offset);
    }

    uint8_t spriteCount = sprites.size();
    CatCore::ServerUtils::writeToBuffer(buffer, offset, &spriteCount, sizeof(spriteCount));

    for (auto sprite : sprites)
    {
        CatCore::ServerUtils::writeToBuffer(buffer, offset, sprite.second.GetName().c_str());
        CatCore::ServerUtils::writeToBuffer(buffer, offset, sprite.second.GetTexture().c_str());

        CatCore::ServerUtils::serializeVector3(buffer, offset, sprite.second.GetPosition());
        CatCore::ServerUtils::writeToBuffer(buffer, offset, &sprite.second.GetRotation(), sizeof(sprite.second.GetRotation()));
        CatCore::ServerUtils::writeToBuffer(buffer, offset, &sprite.second.GetSize(), sizeof(sprite.second.GetSize()));
        CatCore::ServerUtils::writeToBuffer(buffer, offset, &sprite.second.GetRenderBeforePlayer(), sizeof(sprite.second.GetRenderBeforePlayer()));
    }

    ENetPacket* packet = enet_packet_create(buffer, offset, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
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