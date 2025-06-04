#pragma once

#include <string>
#include <sys/socket.h>
#include <vector>
#include <mutex>
#include <optional>
#include <cstdint>
#include "../packet/packet.h"
#include <ostream>

class LWSSocket {

public:
    enum class State {
        Disconnected,
        Connecting,
        Connected,
        Closing,
        Error
    };

    LWSSocket();
    ~LWSSocket();

    // non-copyable, movable
    LWSSocket(const LWSSocket&) = delete;
    LWSSocket& operator=(const LWSSocket&) = delete;
    LWSSocket(LWSSocket&& other) noexcept;
    LWSSocket& operator=(LWSSocket&& other) noexcept;

    // Client operations
    bool connectTo(const std::string& ip, uint16_t port);

    // Server operations
    bool bindTo(uint16_t port);
    bool listenOn(int backlog = SOMAXCONN);
    std::optional<LWSSocket> acceptConnection();

    // Data transfer
    ssize_t sendBytes(const void* data, size_t size);
    ssize_t sendBytes(const std::vector<uint8_t>& data);
    ssize_t receiveBytes(void* buffer, size_t maxSize);
    std::vector<uint8_t> receiveBytes(size_t maxSize);

    //Package transfer
    bool sendPackage(const std::vector<uint8_t>& packet);
    std::vector<uint8_t> receivePackage(Packet::PacketType expectedType = Packet::Unknown);
    bool recvAll(void* buffer, std::size_t size);


    // Close
    void disconnect();

    void closeSocket();

    // Query
    int fd() const noexcept;
    State state() const noexcept;
    bool isValid() const noexcept;



    std::string peerAddress() const;
private:
    int m_fd;
    State m_state;
    mutable std::mutex m_mutex;

    void setState(State s) noexcept;

    static constexpr const char* PREFIX = "[LWSSocket]";
};

std::ostream& operator<<(std::ostream& os, const LWSSocket& socket);
