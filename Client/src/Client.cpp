#include "Client.h"
#include "Commands.h"
#include "Player.h"
#include "ClientCommands.h"
#include "Game.h"
#include "ClientConfig.h"
#include "CatLog.h"

std::function<void()> Client::onConnected = nullptr;
std::function<void(const std::string&)> Client::onDisconnected = nullptr;


void Client::Init()
{
    ClientCommands::InitializeCommands();

    /* Initialize enet6 */
    if (enet_initialize() != 0)
    {
        CatCore::LogError("An error occured while initializing ENet6!\n");
        exit(EXIT_FAILURE);
    }

    atexit(enet_deinitialize);

    /* Build an IPv6 or IPv4 address, depending on what the domain resolves to */
    enet_address_set_host(&address, ENET_ADDRESS_TYPE_ANY, serverIp.c_str());
    address.port = 22556;

    enet_address_get_host_ip(&address, addressBuffer, ENET_ADDRESS_MAX_LENGTH);
    std::string logTxt = "Connecting to : "; logTxt += addressBuffer; logTxt += "\n";
    CatCore::LogInfo(logTxt);
    /* Create a non-listening host using enet_host_create */
    /* Note we create a host using the same address family we resolved earlier */
    clientHost = enet_host_create(address.type, NULL, 1, 0, 0, 0);
    if (clientHost == NULL)
    {
        CatCore::LogError("An error occured while trying to create an ENet6 server host\n");
        exit(EXIT_FAILURE);
    }
    enet_host_bandwidth_throttle(clientHost);

    /* Connect and user service */
    serverPeer = enet_host_connect(clientHost, &address, 2, 0);
    if (serverPeer == NULL)
    {
        CatCore::LogError("No available peers for initializing an ENet connection\n");
        exit(EXIT_FAILURE);
    }

}

void Client::Close()
{
    players.clear();
    sprites.clear();
}

bool Client::Run()
{
    if (!clientHost) {return false;}
    
    while(enet_host_service(clientHost, &event, 0) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
            CatCore::LogInfo(std::string("\nConnecting to server: ") + addressBuffer + "\n\n");
            CatCore::ServerUtils::SendPlayerUUID(serverPeer, ClientConfig::GetUUID());
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
                    char* name;
                    char* texture;
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

                        std::map<std::string, bool> texturesToLoadOrRemove;
                        player.GetInventory().DeSerialize(buffer, offset, texturesToLoadOrRemove);
                        for (auto texture : texturesToLoadOrRemove)
                        {
                            if (texture.second) 
                            { 
                                LoadedTextures::UnLoadTex(texture.first); 
                            }
                            else { LoadedTextures::LoadTex(texture.first); CatCore::LogInfo(std::string("Loaded texture: ") + texture.first + "\n");}
                        }

                        LoadedTextures::LoadTex(player.GetTexture());
                        players[player.GetName()] = player;
                    }
                }
            }
            else if (event.packet && event.packet->data[0] == CatCore::ServerReceiveType::PlayerData)
            {
                char* buffer = (char*)event.packet->data;
                unsigned int offset = sizeof(uint8_t);

                uint8_t playerDataCount;
                CatCore::ServerUtils::readFromBuffer(buffer, offset, &playerDataCount, sizeof(playerDataCount));

                for (size_t i = 0; i < playerDataCount; i++)
                {
                    char* name;
                    char* texture;
                    bool inventory;

                    CatCore::Vector3 position;

                    CatCore::ServerUtils::readTextFromBuffer(buffer, offset, name);
                    CatCore::ServerUtils::readTextFromBuffer(buffer, offset, texture);
                    CatCore::ServerUtils::deserializeVector3(buffer, offset, position);

                    CatCore::ServerUtils::readFromBuffer(buffer, offset, &inventory, sizeof(inventory));
                    if (inventory)
                    {
                        std::map<std::string, bool> texturesToLoadOrRemove;
                        players[name].GetInventory().DeSerialize(buffer, offset, texturesToLoadOrRemove);
                        for (auto texture : texturesToLoadOrRemove)
                        {
                            if (texture.second) LoadedTextures::UnLoadTex(texture.first);
                            else LoadedTextures::LoadTex(texture.first);
                        }
                    }

                    players[name].SetPosition(position);
                    if (players[name].GetTexture() != texture)
                    {
                        LoadedTextures::UnLoadTex(players[name].GetTexture());
                        players[name].SetTexture(texture);
                        LoadedTextures::LoadTex(players[name].GetTexture());
                    }
                }
            }
            else if (event.packet && event.packet->data[0] == CatCore::ServerReceiveType::SpriteAddOrRemove)
            {
                char* buffer = (char*)event.packet->data;
                unsigned int offset = sizeof(uint8_t);

                uint8_t spriteCount;
                CatCore::ServerUtils::readFromBuffer(buffer, offset, &spriteCount, sizeof(spriteCount));

                for (size_t i = 0; i < spriteCount; i++)
                {
                    bool addOrRemove;
                    char* name;
                    char* texture;
                    CatCore::Vector3 position;
                    float rotation;
                    float size;
                    bool renderBeforePlayer;

                    CatCore::ServerUtils::readFromBuffer(buffer, offset, &addOrRemove, sizeof(bool));
                    if (addOrRemove)
                    {
                        CatCore::ServerUtils::readTextFromBuffer(buffer, offset, name);
                        LoadedTextures::UnLoadTex(sprites[name].GetTexture());
                        sprites.erase(name);
                    }
                    else
                    {
                        CatCore::ServerUtils::readTextFromBuffer(buffer, offset, name);
                        CatCore::ServerUtils::readTextFromBuffer(buffer, offset, texture);

                        CatCore::ServerUtils::deserializeVector3(buffer, offset, position);
                        CatCore::ServerUtils::readFromBuffer(buffer, offset, &rotation, sizeof(rotation));
                        CatCore::ServerUtils::readFromBuffer(buffer, offset, &size, sizeof(size));
                        CatCore::ServerUtils::readFromBuffer(buffer, offset, &renderBeforePlayer, sizeof(renderBeforePlayer));

                        CatCore::Sprite sprite;
                        sprite.SetName(name);
                        sprite.SetTexture(texture);
                        sprite.SetPosition(position);
                        sprite.SetRotation(rotation);
                        sprite.SetSize(size);
                        sprite.SetRenderBeforePlayer(renderBeforePlayer);
                        LoadedTextures::LoadTex(sprite.GetTexture());
                        sprites[sprite.GetName()] = sprite;
                    }
                }
            }
            else if (event.packet && event.packet->data[0] == CatCore::ServerReceiveType::SpriteData)
            {
                char* buffer = (char*)event.packet->data;
                unsigned int offset = sizeof(uint8_t);

                uint8_t spriteCount;
                CatCore::ServerUtils::readFromBuffer(buffer, offset, &spriteCount, sizeof(spriteCount));

                for (size_t i = 0; i < spriteCount; i++)
                {
                    char* name;
                    char* texturePath;
                    CatCore::Vector3 position;
                    float rotation;
                    float size;
                    bool renderBeforePlayer;

                    CatCore::ServerUtils::readTextFromBuffer(buffer, offset, name);
                    CatCore::ServerUtils::readTextFromBuffer(buffer, offset, texturePath);

                    CatCore::ServerUtils::deserializeVector3(buffer, offset, position);
                    CatCore::ServerUtils::readFromBuffer(buffer, offset, &rotation, sizeof(rotation));
                    CatCore::ServerUtils::readFromBuffer(buffer, offset, &size, sizeof(size));
                    CatCore::ServerUtils::readFromBuffer(buffer, offset, &renderBeforePlayer, sizeof(renderBeforePlayer));

                    sprites[name].SetPosition(position);
                    if (sprites[name].GetTexture() != texturePath)
                    {
                        LoadedTextures::UnLoadTex(sprites[name].GetTexture());
                        sprites[name].SetTexture(texturePath);
                        LoadedTextures::LoadTex(sprites[name].GetTexture());
                    }
                }
            }
            else if (event.packet && event.packet->data[0] == CatCore::ServerReceiveType::SceneData)
            {
                char* buffer = (char*)event.packet->data;
                unsigned int offset = sizeof(uint8_t);

                uint8_t playerCount;
                CatCore::ServerUtils::readFromBuffer(buffer, offset, &playerCount, sizeof(playerCount));

                for (size_t i = 0; i < playerCount; i++)
                {
                    char* plrname;
                    char* plrtexture;
                    bool inventory;

                    CatCore::Vector3 plrposition;

                    CatCore::ServerUtils::readTextFromBuffer(buffer, offset, plrname);
                    CatCore::ServerUtils::readTextFromBuffer(buffer, offset, plrtexture);
                    CatCore::ServerUtils::deserializeVector3(buffer, offset, plrposition);
                    
                    std::map<std::string, bool> texturesToLoadOrRemove;
                    players[plrname].GetInventory().DeSerialize(buffer, offset, texturesToLoadOrRemove);
                    for (auto texture : texturesToLoadOrRemove)
                    {
                        if (texture.second) LoadedTextures::UnLoadTex(texture.first);
                        else LoadedTextures::LoadTex(texture.first);
                    }

                    players[plrname].SetPosition(plrposition);
                    if (players[plrname].GetTexture() != plrtexture)
                    {
                        LoadedTextures::UnLoadTex(players[plrname].GetTexture());
                        players[plrname].SetTexture(plrtexture);
                        LoadedTextures::LoadTex(players[plrname].GetTexture());
                    }
                }

                uint8_t spriteCount;
                CatCore::ServerUtils::readFromBuffer(buffer, offset, &spriteCount, sizeof(spriteCount));

                for (size_t i = 0; i < spriteCount; i++)
                {
                    char* name;
                    char* texturePath;
                    CatCore::Vector3 position;
                    float rotation;
                    float size;
                    bool renderBeforePlayer;

                    CatCore::ServerUtils::readTextFromBuffer(buffer, offset, name);
                    CatCore::ServerUtils::readTextFromBuffer(buffer, offset, texturePath);

                    CatCore::ServerUtils::deserializeVector3(buffer, offset, position);
                    CatCore::ServerUtils::readFromBuffer(buffer, offset, &rotation, sizeof(rotation));
                    CatCore::ServerUtils::readFromBuffer(buffer, offset, &size, sizeof(size));
                    CatCore::ServerUtils::readFromBuffer(buffer, offset, &renderBeforePlayer, sizeof(renderBeforePlayer));

                    sprites[name].SetPosition(position);
                    if (sprites[name].GetTexture() != texturePath)
                    {
                        LoadedTextures::UnLoadTex(sprites[name].GetTexture());
                        sprites[name].SetTexture(texturePath);
                        LoadedTextures::LoadTex(sprites[name].GetTexture());
                    }
                }
            }
            else if (event.packet && event.packet->data[0] == CatCore::ServerReceiveType::PeerUUID)
            {
                char* buffer = (char*)event.packet->data;
                unsigned int offset = sizeof(uint8_t);

                char* UUID;
                CatCore::ServerUtils::readTextFromBuffer(buffer, offset, UUID);
                ClientConfig::SetUUID(UUID);
                ClientConfig::Save();
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
    const size_t bufferSize = 512;
    char buffer[bufferSize];
    unsigned int offset = 0;

    if (!serverPeer || serverPeer->state != ENET_PEER_STATE_CONNECTED) return;

    uint8_t messageType = CatCore::Data;
    CatCore::ServerUtils::writeToBuffer(buffer, offset, &messageType, sizeof(messageType));

    uint8_t inputCount = changedInputs.size();
    CatCore::ServerUtils::writeToBuffer(buffer, offset, &inputCount, sizeof(inputCount));

    for (auto input : changedInputs)
    {
        CatCore::ServerUtils::writeToBuffer(buffer, offset, &input.first, sizeof(input.first));
        CatCore::ServerUtils::writeToBuffer(buffer, offset, &input.second, sizeof(input.second));
    }

    if (mouse.input != CatCore::NoInput)
    {
        CatCore::ServerUtils::writeToBuffer(buffer, offset, &mouse.input, sizeof(mouse.input));
        CatCore::ServerUtils::writeToBuffer(buffer, offset, &mouse.x, sizeof(mouse.x));
        CatCore::ServerUtils::writeToBuffer(buffer, offset, &mouse.y, sizeof(mouse.y));
    }

    if (changedInputs.size() > 0 || mouse.input != CatCore::NoInput)
    {
        ENetPacket* packet = enet_packet_create(buffer, offset, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(serverPeer, 0, packet);
        mouse.input = CatCore::NoInput;
    }
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
