#include "keepalivepacket.h"

KeepAlivePacket::KeepAlivePacket()
    : Packet(PacketType::KeepAlive)
{
    m_isValid = true;
}

KeepAlivePacket::KeepAlivePacket(const std::vector<uint8_t>& rawPacket)
    : Packet(PacketType::KeepAlive)
{
    if (rawPacket.size() == 1 && rawPacket[0] == static_cast<uint8_t>(PacketType::KeepAlive)) {
        m_isValid = true;
    } else {
        m_isValid = false;
    }
}

std::vector<uint8_t> KeepAlivePacket::serialize() const {
    return { static_cast<uint8_t>(PacketType::KeepAlive) };
}

bool KeepAlivePacket::deserialize(const std::vector<uint8_t>& data) {
    if (data.size() == 1 && data[0] == static_cast<uint8_t>(PacketType::KeepAlive)) {
        m_isValid = true;
        return true;
    }
    m_isValid = false;
    return false;
}
