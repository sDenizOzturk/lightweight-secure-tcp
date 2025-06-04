#pragma once

#include "packet.h"
#include <vector>
#include <cstdint>

class KeepAlivePacket : public Packet {
public:
    KeepAlivePacket();
    KeepAlivePacket(const std::vector<uint8_t>& rawPacket);

    std::vector<uint8_t> serialize() const override;
    bool deserialize(const std::vector<uint8_t>& data) override;
};
