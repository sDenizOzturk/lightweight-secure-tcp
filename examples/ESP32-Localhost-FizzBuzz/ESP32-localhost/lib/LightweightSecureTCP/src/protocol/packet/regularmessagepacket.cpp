#include "regularmessagepacket.h"
#include "encryption/xtea256.h"
#include <cstring>
#include <arpa/inet.h> // For htons and ntohs
#include "utils/byteparser.h"

// Constructor: creates a packet using the provided message string.
RegularMessagePacket::RegularMessagePacket(const std::array<uint32_t, 8> &sessionKey) : Packet(PacketType::RegularMessage), m_length(0), m_message(""), m_sessionKey(sessionKey) {}

RegularMessagePacket::RegularMessagePacket(const std::string &message, const std::array<uint32_t, 8> &sessionKey)
    : Packet(PacketType::RegularMessage), m_length(static_cast<uint16_t>(message.size())), m_message(message), m_sessionKey(sessionKey) {}

// Serializes the packet into a vector of bytes.
std::vector<uint8_t> RegularMessagePacket::serialize() const
{
    std::vector<uint8_t> data;

    try
    {
        // Add header (packet type)
        data.push_back(static_cast<uint8_t>(PacketType::RegularMessage));

        // Encrypt the message with padding
        std::vector<uint8_t> encryptedMessage = XTEA256::encrypt(XTEA256::toRawValues(m_message), m_sessionKey);

        // Update m_length to reflect the padded message length
        uint16_t length = static_cast<uint16_t>(m_message.size());

        // Convert length to network byte order
        uint16_t lengthNetworkOrder = htons(length);

        ByteParser::appendToPacket(data, lengthNetworkOrder);

        // Append the encrypted message
        ByteParser::appendToPacket(data, encryptedMessage);
    }
    catch (const std::exception &e)
    {
    }

    return data;
}

// Deserializes the packet from raw data.
bool RegularMessagePacket::deserialize(const std::vector<uint8_t> &data)
{
    try
    {
        // Minimal size: 1 byte header + 2 bytes length field
        if (data.size() < 3)
        {
            m_isValid = false;
            return false;
        }

        size_t offset = 0;
        uint8_t header = 0;

        ByteParser::extractFromPacket(data, header, offset);

        // Check header: must be RegularMessage
        if (header != static_cast<uint8_t>(PacketType::RegularMessage))
        {
            m_isValid = false;
            return false;
        }

        uint16_t length = 0;
        // Copy the 2-byte length field (bytes 1 and 2)
        ByteParser::extractFromPacket(data, length, offset);

        length = ntohs(length);                          // Convert from network to host byte order
        uint16_t decryptedLength = (length + 7) / 8 * 8; // Round up to the next multiple of 8
        uint8_t paddingLength = decryptedLength - length;

        // Check if total packet size matches expected size: header + 2 bytes length + m_length bytes message
        if (data.size() != 1 + 2 + decryptedLength)
        {
            m_isValid = false;
            return false;
        }

        std::vector<uint8_t> encryptedMessage;
        ByteParser::extractFromPacket(data, encryptedMessage, offset, decryptedLength);
        auto decryptedMessage = XTEA256::decrypt(encryptedMessage, m_sessionKey, paddingLength);
        std::string message = XTEA256::toString(decryptedMessage);

        if (length == 0 || message == "")
        {
            return false;
        }

        m_message = message;
        m_length = length;
        m_isValid = true;

        return true;
    }

    catch (const std::exception &e)
    {
        // If any error occurs, mark the packet as invalid
        m_isValid = false;
        return false;
    }
}

uint16_t RegularMessagePacket::getLength() const
{
    return m_length;
}

std::string RegularMessagePacket::getMessage() const
{
    return m_message;
}
