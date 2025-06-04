#pragma once

#include "packet.h"
#include <vector>
#include <cstdint>

class HandshakeResponsePacket : public Packet {
    uint32_t m_nonce = 0;
    uint64_t m_sum = 0;
    uint64_t m_xor = 0;

    static constexpr size_t expectedPayloadSize = sizeof(m_nonce) + sizeof(m_sum) + sizeof(m_xor); // 4 + 8 + 8 = 20 bytes
    static constexpr size_t expectedPackageSize = 1 + expectedPayloadSize; // 1 + 4 + 8 + 8 = 21 bytes
    static constexpr size_t expectedSize =  ((expectedPayloadSize + 7) / 8 * 8) + 1;
    static constexpr uint8_t expectedPaddingSize = expectedSize - expectedPayloadSize - 1;


public:
    HandshakeResponsePacket();
    HandshakeResponsePacket(uint32_t nonce, uint64_t sum, uint64_t _xor);

    std::vector<uint8_t> serialize() const override;
    bool deserialize(const std::vector<uint8_t>& data) override;

    uint32_t getNonce() const;
    uint64_t getSum() const;
    uint64_t getXor() const;

    static uint8_t getExpectedSize();
};

