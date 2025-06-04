#include "transporter.h"
#include <cstring>
#include <netinet/in.h>
#include "protocol/packet/disconnectpacket.h"
#include "utils/randomgenerator.h"
#include "utils/debugger.h"

Transporter::Transporter(LWSSocket& socket) : m_socket(socket) {}

bool Transporter::sendHandshakeRequest(uint32_t nonce, const uint64_t num1, const uint64_t num2) {
    HandshakeRequestPacket requestPacket(nonce, num1, num2);
    auto serializedPacket = requestPacket.serialize();
    lwsdebug(PREFIX) << "sendHandshakeRequest to " << m_socket;
    return m_socket.sendPackage(serializedPacket);
}

bool Transporter::sendHandshakeResponse(uint32_t nonce, const uint64_t sum, const uint64_t _xor) {
    HandshakeResponsePacket responsePacket(nonce, sum, _xor);
    auto serializedPacket = responsePacket.serialize();
    lwsdebug(PREFIX) << "sendHandshakeResponse to " << m_socket;
    return m_socket.sendPackage(serializedPacket);
}

HandshakeSuccessPacket Transporter::sendHandshakeSuccess() {
    HandshakeSuccessPacket successPacket(RandomGenerator::randomKey());
    if(!successPacket.isValid()) return successPacket;
    auto serializedPacket = successPacket.serialize();

    lwsdebug(PREFIX) << "sendHandshakeSuccess to " << m_socket;
    if(m_socket.sendPackage(serializedPacket)){
        return successPacket;
    }
    return HandshakeSuccessPacket();

}

bool Transporter::sendRegularMessage(const std::string &message,const std::array<uint32_t, 8> &key)
{
    RegularMessagePacket messagePacket(message, key);
    std::vector<uint8_t> serializedPacket = messagePacket.serialize();
    lwsdebug(PREFIX) << "sendRegularMessage to " << m_socket;
    return m_socket.sendPackage(serializedPacket);
}

bool Transporter::sendDisconnect()
{
    DisconnectPacket disconnectPacket;
    std::vector<uint8_t> serializedPacket = disconnectPacket.serialize();
    lwsdebug(PREFIX) << "sendDisconnect to " << m_socket;
    return m_socket.sendPackage(serializedPacket);
}

HandshakeRequestPacket Transporter::receiveHandshakeRequest()
{
    auto rawPacket = m_socket.receivePackage(Packet::HandshakeRequest);
    HandshakeRequestPacket handshakeRequestPacket;
    handshakeRequestPacket.deserialize(rawPacket);
    return handshakeRequestPacket;
}

HandshakeResponsePacket Transporter::receiveHandshakeResponse()
{
    auto rawPacket = m_socket.receivePackage(Packet::HandshakeResponse);
    HandshakeResponsePacket handshakeResponsePacket;
    handshakeResponsePacket.deserialize(rawPacket);
    return handshakeResponsePacket;
}

HandshakeSuccessPacket Transporter::receiveHandshakeSuccess()
{
    auto rawPacket = m_socket.receivePackage(Packet::HandshakeSuccess);
    HandshakeSuccessPacket handshakeSuccessPacket;
    handshakeSuccessPacket.deserialize(rawPacket);
    return handshakeSuccessPacket;
}

RegularMessagePacket Transporter::receiveRegularMessage(const std::array<uint32_t, 8> &sessionKey)
{
    std::vector<uint8_t> rawPacket = m_socket.receivePackage(Packet::RegularMessage);
    if (rawPacket.empty()) {
        return RegularMessagePacket(sessionKey);
    }

    RegularMessagePacket packet(sessionKey);
    packet.deserialize(rawPacket);
    return packet;
}
