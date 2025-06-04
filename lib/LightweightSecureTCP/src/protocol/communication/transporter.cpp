#include "transporter.h"
#include <cstring>
#include <netinet/in.h>
#include "utils/randomgenerator.h"
#include "utils/lwsdebug.h"

#include "protocol/packet/disconnectpacket.h"

Transporter::Transporter(LWSSocket& socket) : m_socket(socket) {}

bool Transporter::sendHandshakeRequest(uint32_t nonce, const uint64_t num1, const uint64_t num2) {
    HandshakeRequestPacket requestPacket(nonce, num1, num2);
    lwsdebug(PREFIX) << "sendHandshakeRequest to " << m_socket;
    return m_socket.sendPacket(requestPacket);
}

bool Transporter::sendHandshakeResponse(uint32_t nonce, const uint64_t sum, const uint64_t _xor) {
    HandshakeResponsePacket responsePacket(nonce, sum, _xor);
    lwsdebug(PREFIX) << "sendHandshakeResponse to " << m_socket;
    return m_socket.sendPacket(responsePacket);
}

HandshakeSuccessPacket Transporter::sendHandshakeSuccess() {
    HandshakeSuccessPacket successPacket(RandomGenerator::randomKey());
    if(!successPacket.isValid()) return successPacket;

    lwsdebug(PREFIX) << "sendHandshakeSuccess to " << m_socket;
    if(m_socket.sendPacket(successPacket)){
        return successPacket;
    }
    return HandshakeSuccessPacket();

}

bool Transporter::sendRegularMessage(const std::string &message,const std::array<uint32_t, 8> &key)
{
    RegularMessagePacket messagePacket(message, key);
    lwsdebug(PREFIX) << "sendRegularMessage to " << m_socket;
    return m_socket.sendPacket(messagePacket);
}

bool Transporter::sendDisconnect()
{
    DisconnectPacket disconnectPacket;
    lwsdebug(PREFIX) << "sendDisconnect to " << m_socket;
    return m_socket.sendPacket(disconnectPacket);
}

bool Transporter::checkKeepAlive()
{
    return m_socket.checkKeepAlive();
}

HandshakeRequestPacket Transporter::receiveHandshakeRequest()
{
    auto rawPacket = m_socket.receivePacket(Packet::HandshakeRequest);
    HandshakeRequestPacket handshakeRequestPacket;
    handshakeRequestPacket.deserialize(rawPacket.data());
    handshakeRequestPacket.setConnectionClosed(rawPacket.connectionClosed());
    return handshakeRequestPacket;
}

HandshakeResponsePacket Transporter::receiveHandshakeResponse()
{
    auto rawPacket = m_socket.receivePacket(Packet::HandshakeResponse);
    HandshakeResponsePacket handshakeResponsePacket;
    handshakeResponsePacket.deserialize(rawPacket.data());
    handshakeResponsePacket.setConnectionClosed(rawPacket.connectionClosed());
    return handshakeResponsePacket;
}

HandshakeSuccessPacket Transporter::receiveHandshakeSuccess()
{
    auto rawPacket = m_socket.receivePacket(Packet::HandshakeSuccess);
    HandshakeSuccessPacket handshakeSuccessPacket;
    handshakeSuccessPacket.deserialize(rawPacket.data());
    handshakeSuccessPacket.setConnectionClosed(rawPacket.connectionClosed());
    return handshakeSuccessPacket;
}

RegularMessagePacket Transporter::receiveRegularMessage(const std::array<uint32_t, 8> &sessionKey)
{
    auto rawPacket = m_socket.receivePacket(Packet::RegularMessage);
    RegularMessagePacket packet(sessionKey);
    packet.deserialize(rawPacket.data());
    packet.setConnectionClosed(rawPacket.connectionClosed());
    return packet;
}
