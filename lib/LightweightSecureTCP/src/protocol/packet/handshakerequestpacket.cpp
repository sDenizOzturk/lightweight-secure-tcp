#include "handshakerequestpacket.h"
#include <cstring>
#include "encryption/xtea256.h"
#include "utils/byteparser.h"

HandshakeRequestPacket::HandshakeRequestPacket() : Packet(PacketType::HandshakeRequest) {}

HandshakeRequestPacket::HandshakeRequestPacket(uint32_t nonce, uint64_t num1, uint64_t num2)
    : Packet(PacketType::HandshakeRequest), m_nonce(nonce), m_num1(num1), m_num2(num2) {
}

std::vector<uint8_t> HandshakeRequestPacket::serialize() const {
    std::vector<uint8_t> payload;

    ByteParser::appendToPacket(payload, m_nonce);
    ByteParser::appendToPacket(payload, m_num1);
    ByteParser::appendToPacket(payload, m_num2);

    std::vector<uint8_t> encryptedPayload = XTEA256::encrypt(payload);

    std::vector<uint8_t> data;
    ByteParser::appendToPacket(data, PacketType::HandshakeRequest);
    ByteParser::appendToPacket(data, encryptedPayload);

    return data;
}

bool HandshakeRequestPacket::deserialize(const std::vector<uint8_t>& data) {

    if (data.size() != expectedSize) {
        return false;
    }
    if (data[0] != PacketType::HandshakeRequest) {
        return false;
    }

    std::vector<uint8_t> encryptedData(data.begin() + 1, data.end());
    std::vector<uint8_t> decryptedData = XTEA256::decrypt(encryptedData, expectedPaddingSize);

    uint32_t nonce = -1;
    uint64_t num1 = -1;
    uint64_t num2 = -1;

    size_t offset = 0;
    ByteParser::extractFromPacket(decryptedData, nonce, offset);
    ByteParser::extractFromPacket(decryptedData, num1, offset);
    ByteParser::extractFromPacket(decryptedData, num2, offset);


    if (nonce == -1 || num1 == -1 || num2 == -1) {
        return false;
    }

    m_nonce = nonce;
    m_num1 = num1;
    m_num2 = num2;
    m_isValid = true;
    return true;
}

uint32_t HandshakeRequestPacket::getNonce() const {
    return m_nonce;
}

uint64_t HandshakeRequestPacket::getNum1() const {
    return m_num1;
}

uint64_t HandshakeRequestPacket::getNum2() const {
    return m_num2;
}

uint8_t HandshakeRequestPacket::getExpectedSize()
{
    return expectedSize;
}
