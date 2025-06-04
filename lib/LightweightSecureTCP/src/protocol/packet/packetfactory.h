#pragma once
// UNUSED: Currently not used in the project, kept for potential future reuse.

#include <string>
#include <vector>
#include <cstdint>


#include "handshakerequestpacket.h"
#include "handshakeresponsepacket.h"
#include "handshakesuccesspacket.h"


class PacketFactory {
public:
    static HandshakeRequestPacket createHandshakeRequest(uint32_t nonce, const uint64_t num1, const uint64_t num2);
    static HandshakeResponsePacket createHandshakeResponse(uint32_t nonce, const uint64_t sum, const uint64_t _xor);
    static HandshakeSuccessPacket createHandshakeSuccess();
    static std::vector<uint8_t> createRegularMessage(const std::string& payload);
    static std::vector<uint8_t> createDisconnect();
};
