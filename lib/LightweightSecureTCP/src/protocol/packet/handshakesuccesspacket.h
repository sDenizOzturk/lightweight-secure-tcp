#pragma once

#include "packet.h"
#include <vector>
#include <cstdint>
#include <array>

class HandshakeSuccessPacket : public Packet {
    std::array<uint32_t, 8> m_sessionKey;
    static constexpr size_t expectedPayloadSize = sizeof(m_sessionKey); // 32
    static constexpr size_t expectedSize =  ((expectedPayloadSize + 7) / 8 * 8) + 1; // 1 + 32
public:
    HandshakeSuccessPacket(const std::array<uint32_t, 8>& sessionKey);
    HandshakeSuccessPacket();

    std::vector<uint8_t> serialize() const override;
    bool deserialize(const std::vector<uint8_t>& data) override;

    static uint8_t getExpectedSize();
    std::array<uint32_t, 8> sessionKey() const;
};

