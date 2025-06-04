#include "handshakeresponsepacket.h"
#include <cstring>
#include "encryption/xtea256.h"
#include "utils/byteparser.h"

HandshakeResponsePacket::HandshakeResponsePacket()  : Packet(PacketType::HandshakeResponse){}

HandshakeResponsePacket::HandshakeResponsePacket(uint32_t nonce, uint64_t sum, uint64_t _xor)
    : Packet(PacketType::HandshakeResponse), m_nonce(nonce), m_sum(sum), m_xor(_xor) {
}

std::vector<uint8_t> HandshakeResponsePacket::serialize() const {
    std::vector<uint8_t> payload;

    ByteParser::appendToPacket(payload, m_nonce);
    ByteParser::appendToPacket(payload, m_sum);
    ByteParser::appendToPacket(payload, m_xor);

    std::vector<uint8_t> encryptedPayload = XTEA256::encrypt(payload);

    std::vector<uint8_t> data;

    ByteParser::appendToPacket(data, PacketType::HandshakeResponse);
    ByteParser::appendToPacket(data, encryptedPayload);
    return data;
}

bool HandshakeResponsePacket::deserialize(const std::vector<uint8_t>& data) {    
    if (data.size() != expectedSize) {
        return false;
    }
    if (data[0] != PacketType::HandshakeResponse) {
        return false;
    }

    std::vector<uint8_t> encryptedData(data.begin() + 1, data.end());
    std::vector<uint8_t> decryptedData = XTEA256::decrypt(encryptedData, expectedPaddingSize);

    uint32_t nonce = -1;
    uint64_t sum = -1;
    uint64_t _xor = -1;

    size_t offset = 0;
    ByteParser::extractFromPacket(decryptedData, nonce, offset);
    ByteParser::extractFromPacket(decryptedData, sum, offset);
    ByteParser::extractFromPacket(decryptedData, _xor, offset);

    if (nonce == -1 || sum == -1 || _xor == -1) {
        return false;
    }

    m_nonce = nonce;
    m_sum = sum;
    m_xor = _xor;

    m_isValid = true;
    return true;
}

uint32_t HandshakeResponsePacket::getNonce() const {
    return m_nonce;
}

uint64_t HandshakeResponsePacket::getSum() const {
    return m_sum;
}

uint64_t HandshakeResponsePacket::getXor() const
{
    return m_xor;
}

uint8_t HandshakeResponsePacket::getExpectedSize()
{
    return expectedSize;
}
