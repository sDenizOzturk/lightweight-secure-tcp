#include "handshakesuccesspacket.h"
#include "encryption/xtea256.h"
#include "utils/byteparser.h"


std::array<uint32_t, 8> HandshakeSuccessPacket::sessionKey() const
{
    return m_sessionKey;
}

HandshakeSuccessPacket::HandshakeSuccessPacket(const std::array<uint32_t, 8> &sessionKey)
    : Packet(PacketType::HandshakeSuccess), m_sessionKey(sessionKey) {
    m_isValid = true;
}

HandshakeSuccessPacket::HandshakeSuccessPacket()
    : Packet(PacketType::HandshakeSuccess) {}

std::vector<uint8_t> HandshakeSuccessPacket::serialize() const {
    std::vector<uint8_t> payload;
    ByteParser::appendToPacket(payload, m_sessionKey);
    std::vector<uint8_t> encryptedPayload = XTEA256::encrypt(payload);

    std::vector<uint8_t> data;
    ByteParser::appendToPacket(data, PacketType::HandshakeSuccess);
    ByteParser::appendToPacket(data, encryptedPayload);

    return data;
}

bool HandshakeSuccessPacket::deserialize(const std::vector<uint8_t>& data) {
    size_t offset = 0;
    uint8_t header = 0;

    ByteParser::extractFromPacket(data, header, offset);

    if (data.size() != expectedSize) {
        return false;
    }
    if (header != PacketType::HandshakeSuccess) {
        return false;
    }

    std::vector<uint8_t> encryptedData;
    ByteParser::extractFromPacket(data, encryptedData, offset, sizeof(m_sessionKey));

    std::vector<uint8_t> decryptedData = XTEA256::decrypt(encryptedData);

    std::array<uint32_t, 8> sessionKey;

    if (!ByteParser::extractFromPacket(decryptedData, sessionKey)) {
        return false;
    }

    if (sessionKey.size() == 0) {
        return false;
    }

    m_sessionKey = sessionKey;
    m_isValid = true;
    return true;
}

uint8_t HandshakeSuccessPacket::getExpectedSize()
{
    return expectedSize;
}
