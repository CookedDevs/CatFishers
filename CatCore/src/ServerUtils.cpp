#include "ServerUtils.h"
#include <random>

namespace CatCore
{
    // TODO: move to core
    void ServerUtils::SendMessage(ENetPeer* receiver, const std::string message)
    {
        char sendType = CatCore::ServerReceiveType::Message;
        std::string send;
        send.push_back(sendType);
        send += message;

        ENetPacket* packet = enet_packet_create(send.c_str(), send.size() + 1, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(receiver, 0, packet);
    }

    void ServerUtils::SendData(ENetPeer* receiver, const std::vector<uint8_t> data)
    {
        uint8_t sendType = (uint8_t)CatCore::ServerReceiveType::Data;
        std::vector<uint8_t> send;
        send.push_back(sendType);
        send.insert(send.end(), data.begin(), data.end());

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

    void ServerUtils::writeToBuffer(char* buffer, unsigned int& offset, const void* data, size_t size)
    {
        std::memcpy(buffer + offset, data, size);
        offset += size;
    }
    void ServerUtils::writeToBuffer(char* buffer, unsigned int& offset, const char* str)
    {
        uint32_t length = strlen(str);  // Get the length of the string
        writeToBuffer(buffer, offset, &length, sizeof(length));  // Send the length first
        writeToBuffer(buffer, offset, str, length);  // Send the actual string
    }
    void ServerUtils::serializeVector3(char* buffer, unsigned int& offset, const Vector3& vec)
    {
        writeToBuffer(buffer, offset, &vec.x, sizeof(float));
        writeToBuffer(buffer, offset, &vec.y, sizeof(float));
        writeToBuffer(buffer, offset, &vec.z, sizeof(float));
    }

    void ServerUtils::readFromBuffer(const char* buffer, unsigned int& offset, void* data, size_t size)
    {
        memcpy(data, buffer + offset, size);
        offset += size;
    }

    void ServerUtils::readTextFromBuffer(const char* buffer, unsigned int& offset, char*& data)
    {
        uint32_t length;
        readFromBuffer(buffer, offset, &length, sizeof(uint32_t));  // Read the length of the string
        data = new char[length + 1];  // Allocate memory (+1 for null-terminator)
        readFromBuffer(buffer, offset, data, sizeof(data[0]) * length);  // Read the actual string


        data[length] = '\0';  // Null-terminate the string
    }

    void ServerUtils::deserializeVector3(const char* buffer, unsigned int& offset, Vector3& vector) {
        // Read the 3 float components of the btVector3 from the buffer
        float x, y, z;

        memcpy(&x, buffer + offset, sizeof(float));
        offset += sizeof(float);

        memcpy(&y, buffer + offset, sizeof(float));
        offset += sizeof(float);

        memcpy(&z, buffer + offset, sizeof(float));
        offset += sizeof(float);

        // Set the btVector3 from the read values
        vector = { x, y, z };
    }

    void ServerUtils::SendPlayerUUID(ENetPeer* peer, std::string UUID)
    {
        const size_t bufferSize = 255;
        char buffer[bufferSize];
        unsigned int offset = 0;

        uint8_t messageType = CatCore::PeerUUID;
        CatCore::ServerUtils::writeToBuffer(buffer, offset, &messageType, sizeof(messageType));

        if (UUID.empty()) UUID = "EMPTY";
        CatCore::ServerUtils::writeToBuffer(buffer, offset, UUID.c_str());

        ENetPacket* packet = enet_packet_create(buffer, offset, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer, 1, packet);
    }

    std::string ServerUtils::GetUUID() {
        static std::random_device dev;
        static std::mt19937 rng(dev());

        std::uniform_int_distribution<int> dist(0, 15);

        const char* value = "0123456789abcdef";
        const bool dash[] = { 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0 };

        std::string UUID;
        for (int i = 0; i < 16; i++) 
        {
            if (dash[i]) UUID += "-";
            UUID += value[dist(rng)];
            UUID += value[dist(rng)];
        }
        return UUID;
    }

}