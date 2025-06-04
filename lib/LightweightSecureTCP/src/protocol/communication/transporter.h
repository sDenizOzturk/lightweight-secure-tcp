#pragma once

#include <string>
#include "protocol/packet/handshakerequestpacket.h"
#include "protocol/packet/handshakeresponsepacket.h"
#include "protocol/packet/handshakesuccesspacket.h"
#include "protocol/packet/regularmessagepacket.h"
#include "protocol/communication/lwssocket.h"

class Transporter {
public:
    explicit Transporter(LWSSocket& socket);
    ~Transporter() = default;

    bool sendHandshakeRequest(uint32_t nonce, const uint64_t num1, const uint64_t num2);
    bool sendHandshakeResponse(uint32_t nonce, const uint64_t sum, const uint64_t _xor);
    HandshakeSuccessPacket sendHandshakeSuccess();
    bool sendRegularMessage(const std::string& message, const std::array<uint32_t, 8> &key);
    bool sendDisconnect();
    bool checkKeepAlive();

    HandshakeRequestPacket receiveHandshakeRequest();
    HandshakeResponsePacket receiveHandshakeResponse();
    HandshakeSuccessPacket receiveHandshakeSuccess();
    RegularMessagePacket receiveRegularMessage(const std::array<uint32_t, 8> &key);

private:
    LWSSocket& m_socket;
    const std::string PREFIX = "[Transporter]";
};

