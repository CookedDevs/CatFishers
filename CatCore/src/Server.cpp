#include "ServerUtils.h"

namespace CatCore
{
    void ServerUtils::SendMessage(ENetPeer* receiver, const std::string message)
    {
        char sendType = CatCore::ServerReceiveType::Message;
        std::string send;
        send.push_back(sendType);
        send += message;

        ENetPacket* packet = enet_packet_create(send.c_str(), send.size() + 1, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(receiver, 0, packet);
    }

    void ServerUtils::SendData(ENetPeer* receiver, const std::vector<std::byte> data)
    {
        std::byte sendType = (std::byte)CatCore::ServerReceiveType::Data;
        std::vector<std::byte> send;
        send.push_back(sendType);
        send.insert(send.end(), data.begin(), data.end());


        for (auto dator : send)
        {
            std::cout << (unsigned char)dator;
        }

        ENetPacket* packet = enet_packet_create(send.data(), send.size() + 1, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(receiver, 0, packet);
    }

    void ServerUtils::SendCommandData(ENetPeer* receiver, const std::string message)
    {
        char sendType = CatCore::ServerReceiveType::CommandData;
        std::string send;
        send.push_back(sendType);
        send += message;

        ENetPacket* packet = enet_packet_create(send.c_str(), send.size() + 1, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(receiver, 0, packet);
    }
}