#pragma once

#include "packet.h"
#include <vector>
#include <cstdint>

class HandshakeRequestPacket : public Packet {
    uint32_t m_nonce = 0;
    uint64_t m_num1 = 0;
    uint64_t m_num2 = 0;

    static constexpr size_t expectedPayloadSize = sizeof(m_nonce) + sizeof(m_num1) + sizeof(m_num2); // 4 + 8 + 8 = 20 bytes
    static constexpr size_t expectedPackageSize = 1 + expectedPayloadSize; // 1 + 4 + 8 + 8 = 21 bytes
    static constexpr size_t expectedSize =  ((expectedPayloadSize + 7) / 8 * 8) + 1;
    static constexpr uint8_t expectedPaddingSize = expectedSize - expectedPayloadSize - 1;

public:
    HandshakeRequestPacket();

    // Constructor that builds the packet from values
    HandshakeRequestPacket(uint32_t nonce, uint64_t num1, uint64_t num2);

    std::vector<uint8_t> serialize() const override;
    bool deserialize(const std::vector<uint8_t>& data) override;

    uint32_t getNonce() const;
    uint64_t getNum1() const;
    uint64_t getNum2() const;

    static uint8_t getExpectedSize();

};

