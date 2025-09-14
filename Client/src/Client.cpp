#include "Client.h"
#include "Commands.h"
#include "Player.h"
#include "ClientCommands.h"
#include "Game.h"

std::function<void()> Client::onConnected = nullptr;
std::function<void(const std::string&)> Client::onDisconnected = nullptr;


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
    enet_address_set_host(&address, ENET_ADDRESS_TYPE_ANY, serverIp.c_str());
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
    if (!clientHost) {return false;}
    
    while(enet_host_service(clientHost, &event, 0) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
            std::cout << "\nConnected to server!" << addressBuffer << "\n\n";
            Client::onConnected();
            break;

        case ENET_EVENT_TYPE_RECEIVE:
            if (event.packet && event.packet &&event.packet->data[0] == CatCore::ServerReceiveType::Message)
            {
                std::cout << event.packet->data << "\n";
                enet_packet_destroy(event.packet);
            }
            else if (event.packet && event.packet->data[0] == CatCore::ServerReceiveType::CommandData)
            {
                std::string recv = (const char*)event.packet->data;
                recv.erase(0, 1);
                ClientCommands::AddServerCommands(recv);
            }
            else if (event.packet && event.packet->data[0] == CatCore::ServerReceiveType::PlayerAddOrRemove)
            {
                char* buffer = (char*)event.packet->data;
                unsigned int offset = sizeof(uint8_t);

                uint8_t playerCount;
                CatCore::ServerUtils::readFromBuffer(buffer, offset, &playerCount, sizeof(playerCount));

                for (size_t i = 0; i < playerCount; i++)
                {
                    bool addOrRemove;
                    char* name = "";
                    char* texture = "";
                    CatCore::Vector3 position;

                    CatCore::ServerUtils::readFromBuffer(buffer, offset, &addOrRemove, sizeof(bool));
                    if (addOrRemove)
                    {
                        CatCore::ServerUtils::readTextFromBuffer(buffer, offset, name);
                        LoadedTextures::UnLoadTex(players[name].GetTexture());
                        players.erase(name);
                    }
                    else
                    {
                        CatCore::ServerUtils::readTextFromBuffer(buffer, offset, name);
                        CatCore::ServerUtils::readTextFromBuffer(buffer, offset, texture);
                        CatCore::ServerUtils::deserializeVector3(buffer, offset, position);

                        CatCore::Player player;
                        player.SetName(name);
                        player.SetTexture(texture);
                        player.SetPosition(position);
                        LoadedTextures::LoadTex(player.GetTexture());
                        players[player.GetName()] = player;
                    }
                }
            }
            else if (event.packet && event.packet->data[0] == CatCore::ServerReceiveType::PlayerData)
            {
                char* buffer = (char*)event.packet->data;
                unsigned int offset = sizeof(uint8_t);

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

                    players[name].position = position;
                    if (players[name].GetTexture() != texture)
                    {
                        LoadedTextures::UnLoadTex(players[name].GetTexture());
                        players[name].SetTexture(texture);
                        LoadedTextures::LoadTex(players[name].GetTexture());
                    }
                }
            }
            else if (event.packet && event.packet->data[0] == CatCore::ServerReceiveType::Data)
            {
                for (auto sprite : sprites)
                 //   LoadedTextures::UnLoadTex(sprite.texture);
                sprites.clear();
                char* buffer = (char*)event.packet->data;
                unsigned int offset = 1;

                uint8_t spriteCount;
                CatCore::ServerUtils::readFromBuffer(buffer, offset, &spriteCount, sizeof(spriteCount));

                for (size_t i = 0; i < spriteCount; i++)
                {
                    CatCore::Sprite sprite;
                    sprite.DeSerialize(buffer, offset);
                    LoadedTextures::LoadTex(sprite.texture);
                    //sprites[sprite.] = sprite;
                }
            }

            break;

        case ENET_EVENT_TYPE_DISCONNECT:
            if (event.peer == serverPeer) {
                enet_peer_reset(serverPeer);
                serverPeer = nullptr;
            }
            ClientCommands::ClearServerCommands();
            Client::onDisconnected("Disconnected from server!");
            return EXIT_SUCCESS;

        case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
            if (event.peer == serverPeer) {
                enet_peer_reset(serverPeer);
                serverPeer = nullptr;
            }
            Client::onDisconnected("Disconnected from server (timed out)!");
            return EXIT_FAILURE;
        }
    }
    if (serverPeer && serverPeer->state == ENET_PEER_STATE_CONNECTED)
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
                Client::onDisconnected("\nDisconnected from server!\n");
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

void Client::SendInputData()
{
    if (!serverPeer || serverPeer->state != ENET_PEER_STATE_CONNECTED) return;

    uint8_t sendType = (uint8_t)CatCore::ServerReceiveType::Data;

    for (auto input : changedInputs)
    {
        std::vector<uint8_t> send;
        send.push_back(sendType);
        send.push_back(input.first);
        send.push_back(input.second);

        ENetPacket* packet = enet_packet_create(send.data(), send.size(), ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(serverPeer, 0, packet);
        enet_host_flush(clientHost);
    }

    enet_host_flush(clientHost);
}

void Client::SendFuncId(const uint16_t id)
{
    const size_t bufferSize = sizeof(uint8_t) + sizeof(uint16_t);
    char buffer[bufferSize];
    unsigned int offset = 0;

    uint8_t messageType = CatCore::Function;
    CatCore::ServerUtils::writeToBuffer(buffer, offset, &messageType, sizeof(messageType));
    CatCore::ServerUtils::writeToBuffer(buffer, offset, &id, sizeof(id));

    ENetPacket* packet = enet_packet_create(buffer, offset, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(serverPeer, 0, packet);
    enet_host_flush(clientHost);
}


void Client::SetKey(const char key, const bool value)
{
    if (inputs.find(key) == inputs.end())
    {
        inputs[key] = value;
        changedInputs[key] = inputs[key];
    }
    else if (value != inputs[key])
    {
        inputs[key] = !inputs[key];
        changedInputs[key] = inputs[key];
    }
}

void Client::SetKey(const char key)
{
    if (inputs.find(key) == inputs.end())
    {
        inputs[key] = IsKeyDown(key);
        changedInputs[key] = inputs[key];
    }
    else if (IsKeyDown(key) != inputs[key])
    {
        inputs[key] = !inputs[key];
        changedInputs[key] = inputs[key];
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
