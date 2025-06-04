#pragma once

#include "packet.h"
#include <vector>
#include <cstdint>

class DisconnectPacket : public Packet {
public:
    DisconnectPacket();
    DisconnectPacket(const std::vector<uint8_t>& rawPacket);

    // Serialize the packet into raw bytes.
    std::vector<uint8_t> serialize() const override;
    // Deserialize raw bytes into the packet.
    bool deserialize(const std::vector<uint8_t>& data) override;
};

