#include "disconnectpacket.h"

// Default constructor: creates a valid DisconnectPacket.
DisconnectPacket::DisconnectPacket()
    : Packet(PacketType::Disconnect)
{
    m_isValid = true;
}

// Constructor: deserializes a Packet from raw data.
DisconnectPacket::DisconnectPacket(const std::vector<uint8_t>& rawPacket)
    : Packet(PacketType::Disconnect)
{
    // The DisconnectPacket is expected to be exactly one byte: the type byte.
    if (rawPacket.size() == 1 && rawPacket[0] == static_cast<uint8_t>(PacketType::Disconnect)) {
        m_isValid = true;
    } else {
        m_isValid = false;
    }
}

// Serialize the DisconnectPacket as a single byte.
std::vector<uint8_t> DisconnectPacket::serialize() const {
    return { static_cast<uint8_t>(PacketType::Disconnect) };
}

// Deserialize raw data into the DisconnectPacket.
// Since the Packet is empty (only a type byte), we only check its validity.
bool DisconnectPacket::deserialize(const std::vector<uint8_t>& data) {
    if (data.size() == 1 && data[0] == static_cast<uint8_t>(PacketType::Disconnect)) {
        m_isValid = true;
        return true;
    }
    m_isValid = false;
    return false;
}
