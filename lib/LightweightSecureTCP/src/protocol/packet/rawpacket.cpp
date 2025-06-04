#include "rawpacket.h"

RawPacket::RawPacket()
    : m_type(Packet::PacketType::Unknown), m_status(Status::Success) {}

void RawPacket::setType(Packet::PacketType type) {
    m_type = type;
}

void RawPacket::setStatus(Status status) {
    m_status = status;
}

void RawPacket::setData(const std::vector<uint8_t>& data) {
    m_data = data;
}

Packet::PacketType RawPacket::type() const {
    return m_type;
}

RawPacket::Status RawPacket::status() const {
    return m_status;
}

const std::vector<uint8_t>& RawPacket::data() const {
    return m_data;
}

bool RawPacket::isSuccess() const {
    return m_status == Status::Success;
}

bool RawPacket::connectionClosed() const {
    return m_status == Status::ConnectionClosed || m_status == Status::DisconnectReceived;
}
