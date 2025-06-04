#pragma once

#include <vector>
#include <cstdint>
#include "packet.h"

class RawPacket {
public:
    enum class Status {
        Success,                // Everything OK
        Timeout,                // No data received in time
        DisconnectReceived,     // Disconnect packet received
        KeepAlive,              // Keep alive packet received
        ConnectionClosed,       // recv() returned 0
        ConnectionReset,        // ECONNRESET
        InvalidSocket,          // EBADF, ENOTSOCK
        ReceiveError,           // recv() failed with other errno
        UnexpectedType,         // Received a different type than expected
        LengthReadError,        // Failed to read length field
        IncompletePayload,      // Payload did not arrive completely
        UnknownPacketType       // Header value not recognized
    };


    RawPacket();

    void setType(Packet::PacketType type);
    void setStatus(Status status);
    void setData(const std::vector<uint8_t>& data);

    Packet::PacketType type() const;
    Status status() const;
    const std::vector<uint8_t>& data() const;

    bool isSuccess() const;
    bool connectionClosed() const;
private:
    Packet::PacketType m_type;
    Status m_status;
    std::vector<uint8_t> m_data;
};
