#pragma once

#include <vector>
#include <cstdint>
#include <cstddef>

class Packet {
public:
    enum PacketType : uint8_t {
        Unknown             = 0x00,
        RegularMessage      = 0x01,
        HandshakeRequest    = 0x02,
        HandshakeResponse   = 0x03,
        HandshakeSuccess    = 0x04,
        KeepAlive          = 0x05,
        Disconnect          = 0x06,
    };

    explicit Packet(PacketType packetType) : m_packetType(packetType) {};
    virtual std::vector<uint8_t> serialize() const = 0;  // Converts object to bytes
    virtual bool deserialize(const std::vector<uint8_t>& data) = 0;  // Converts bytes to object
    virtual uint8_t getType() {return m_isValid;};
    virtual bool isValid() {return m_isValid;};
    virtual ~Packet() = default;

    bool connectionClosed() const { return m_connectionClosed; }
    void setConnectionClosed(bool connectionClosed) { m_connectionClosed = connectionClosed; }

protected:
    bool m_isValid = false;
    bool m_connectionClosed = false;
    PacketType m_packetType;  
};

