#pragma once

#include <string>
#include <sys/socket.h>
#include <vector>
#include <mutex>
#include <optional>
#include <cstdint>
#include "../packet/packet.h"
#include <ostream>
#include "protocol/packet/rawpacket.h"

class LWSSocket {

public:
    enum class State {
        Disconnected,
        Connecting,
        Connected,
        Closing,
        Error
    };

    LWSSocket(int keepAliveIntervalMs, int inactivityTimeoutMs);
    ~LWSSocket();

    // non-copyable, movable
    LWSSocket(const LWSSocket&) = delete;
    LWSSocket& operator=(const LWSSocket&) = delete;
    LWSSocket(LWSSocket&& other) noexcept;
    LWSSocket& operator=(LWSSocket&& other) noexcept;

    void setKeepAliveConfig(int keepAliveIntervalMs, int inactivityTimeoutMs);

    // Client operations
    bool connectTo(const std::string& ip, uint16_t port);

    // Server operations
    bool bindTo(uint16_t port);
    bool listenOn(int backlog = SOMAXCONN);
    std::optional<LWSSocket> acceptConnection();


    //Packet transfer
    bool sendPacket(const Packet &packet);
    RawPacket receivePacket(Packet::PacketType expectedType = Packet::Unknown);
    void recvAll(RawPacket& packet, void* buffer, std::size_t size);


    // Close
    void disconnect();

    void closeSocket();

    // Query
    int fd() const noexcept;
    State state() const noexcept;
    bool isValid() const noexcept;

    std::string peerAddress() const;

    bool checkKeepAlive();

private:
    int m_fd;
    State m_state;
    mutable std::mutex m_mutex;

    void setState(State s) noexcept;

    static constexpr const char* PREFIX = "[LWSSocket]";

    std::chrono::milliseconds m_keepAliveInterval;
    std::chrono::milliseconds m_inactivityTimeout;
    std::chrono::steady_clock::time_point m_lastIncomingPacketTime;
    std::chrono::steady_clock::time_point m_lastPacketSentTime ;


};

std::ostream& operator<<(std::ostream& os, const LWSSocket& socket);
