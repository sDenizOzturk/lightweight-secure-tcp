// UNUSED: Currently not used in the project, kept for potential future reuse.

#include "packetfactory.h"
#include "packet.h"
#include <cstring>
#include <string>

HandshakeRequestPacket PacketFactory::createHandshakeRequest(uint32_t nonce, const uint64_t num1, const uint64_t num2)
{
    HandshakeRequestPacket handshakeRequestPacket(nonce, num1, num2);
    return handshakeRequestPacket;
}

HandshakeResponsePacket PacketFactory::createHandshakeResponse(uint32_t nonce, const uint64_t sum, const uint64_t _xor)
{
    HandshakeResponsePacket handshakeResponsePacket(nonce, sum, _xor);
    return handshakeResponsePacket;
}

HandshakeSuccessPacket PacketFactory::createHandshakeSuccess()
{
    HandshakeSuccessPacket handshakeSuccessPacket;
    return handshakeSuccessPacket;
}

std::vector<uint8_t> PacketFactory::createRegularMessage(const std::string &payload)
{
    uint16_t length = payload.size();
    std::vector<uint8_t> packet(1 + 2 + length);

    packet[0] = 0x01; // Message Type: Regular Message
    std::memcpy(&packet[1], &length, 2);
    std::memcpy(&packet[3], payload.data(), length); // Copy string data

    return packet;
}

std::vector<uint8_t> PacketFactory::createDisconnect()
{
    return {Packet::Disconnect};
}
