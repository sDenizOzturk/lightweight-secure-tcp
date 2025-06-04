#pragma once

#include "packet.h"
#include <vector>
#include <cstdint>
#include <string>
#include <array>

class RegularMessagePacket : public Packet
{
private:
    uint16_t m_length;     // Message length (in bytes)
    std::string m_message; // Message content
    std::array<uint32_t, 8> m_sessionKey;

public:
    RegularMessagePacket(const std::array<uint32_t, 8> &m_sessionKey);

    // Constructor to create a packet from a message string
    RegularMessagePacket(const std::string &message, const std::array<uint32_t, 8> &m_sessionKey);

    // Serializes the packet into a vector of bytes
    std::vector<uint8_t> serialize() const override;

    bool deserialize(const std::vector<uint8_t> &data) override;

    // Getters for message length and content
    uint16_t getLength() const;
    std::string getMessage() const;
};
