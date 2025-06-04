#include "lwssocket.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include "utils/lwsdebug.h"
#include "protocol/packet/handshakerequestpacket.h"
#include "protocol/packet/handshakeresponsepacket.h"
#include "protocol/packet/handshakesuccesspacket.h"
#include "protocol/packet/keepalivepacket.h"

// ----------------------------------------------------------------
// Constructor / Destructor
// ----------------------------------------------------------------
LWSSocket::LWSSocket(int keepAliveIntervalMs, int inactivityTimeoutMs)
    : m_fd(-1),
    m_state(State::Disconnected),
    m_keepAliveInterval(std::chrono::milliseconds(keepAliveIntervalMs)),
    m_inactivityTimeout(std::chrono::milliseconds(inactivityTimeoutMs)),
    m_lastIncomingPacketTime(std::chrono::steady_clock::now()),
    m_lastPacketSentTime(std::chrono::steady_clock::now())
{}

LWSSocket::~LWSSocket() {
    disconnect();
}

// Move semantics
LWSSocket::LWSSocket(LWSSocket&& other) noexcept {
    std::lock_guard<std::mutex> lock(other.m_mutex);
    m_fd                     = other.m_fd;
    m_state                  = other.m_state;
    m_lastIncomingPacketTime = other.m_lastIncomingPacketTime;
    m_lastPacketSentTime     = other.m_lastPacketSentTime;
    m_keepAliveInterval      = other.m_keepAliveInterval;
    m_inactivityTimeout      = other.m_inactivityTimeout;

    other.m_fd    = -1;
    other.m_state = State::Disconnected;
}

LWSSocket& LWSSocket::operator=(LWSSocket&& other) noexcept {
    if (this != &other) {
        disconnect();
        std::lock_guard<std::mutex> lockFrom(other.m_mutex);
        m_fd                     = other.m_fd;
        m_state                  = other.m_state;
        m_lastIncomingPacketTime = other.m_lastIncomingPacketTime;
        m_lastPacketSentTime     = other.m_lastPacketSentTime;
        m_keepAliveInterval      = other.m_keepAliveInterval;
        m_inactivityTimeout      = other.m_inactivityTimeout;

        other.m_fd    = -1;
        other.m_state = State::Disconnected;
    }
    return *this;
}

void LWSSocket::setKeepAliveConfig(int keepAliveIntervalMs, int inactivityTimeoutMs)
{
    m_keepAliveInterval  = std::chrono::milliseconds(keepAliveIntervalMs);
    m_inactivityTimeout  = std::chrono::milliseconds(inactivityTimeoutMs);
}


// ----------------------------------------------------------------
// Internal state setter
// ----------------------------------------------------------------
void LWSSocket::setState(State s) noexcept {
    m_state = s;
}
// ----------------------------------------------------------------
// Client: connect to remote host
// ----------------------------------------------------------------
bool LWSSocket::connectTo(const std::string& ip, uint16_t port) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_fd >= 0) disconnect();

    m_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (m_fd < 0) {
        setState(State::Error);
        return false;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) != 1) {
        ::close(m_fd);
        m_fd = -1;
        setState(State::Error);
        return false;
    }

    setState(State::Connecting);
    if (::connect(m_fd, (sockaddr*)&addr, sizeof(addr)) == 0) {
        setState(State::Connected);
        return true;
    }

    ::close(m_fd);
    m_fd = -1;
    setState(State::Error);
    return false;
}

// ----------------------------------------------------------------
// Server: bind and listen
// ----------------------------------------------------------------

bool LWSSocket::bindTo(uint16_t port) {
    // 1) create the socket
    m_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (m_fd < 0) {
        // failed to get a socket
        return false;
    }

    // 2) allow immediate reuse of the port
    int opt = 1;
    if (setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        ::close(m_fd);
        m_fd = -1;
        return false;
    }

    // 3) bind to INADDR_ANY:port
    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(port);

    if (::bind(m_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        ::close(m_fd);
        m_fd = -1;
        return false;
    }

    // success: socket is created and bound
    return true;
}



bool LWSSocket::listenOn(int backlog) {
    // make sure we have a valid socket
    if (m_fd < 0) {
        return false;
    }

    // start listening
    if (::listen(m_fd, backlog) < 0) {
        ::close(m_fd);
        m_fd = -1;
        return false;
    }

    return true;
}

std::optional<LWSSocket> LWSSocket::acceptConnection() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_fd < 0) return std::nullopt;

    int clientFd = ::accept(m_fd, nullptr, nullptr);
    if (clientFd < 0) {
        return std::nullopt;
    }

    LWSSocket clientSock(m_keepAliveInterval.count(), m_inactivityTimeout.count());
    {
        std::lock_guard<std::mutex> clientLock(clientSock.m_mutex);
        clientSock.m_fd    = clientFd;
        clientSock.m_state = State::Connected;
    }
    return clientSock;
}

bool LWSSocket::checkKeepAlive()
{
    // If inactivity timeout is disabled (-1), consider connection always alive
    if (m_inactivityTimeout.count() < 0) {
        return true;
    }

    auto now = std::chrono::steady_clock::now();

    // Drop connection if no packets received in allowed inactivity period
    if (now - m_lastIncomingPacketTime > m_inactivityTimeout) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastIncomingPacketTime).count();
        lwsdebug(PREFIX) << "Elapsed ms since last incoming: " << elapsed << " for Socket[fd=" << m_fd << "]";
        return false;
    }

    // Send KeepAlive packet if interval has passed
    if (m_keepAliveInterval.count() > 0 && now - m_lastPacketSentTime > m_keepAliveInterval) {
        KeepAlivePacket packet;
        sendPacket(packet); // Also updates m_lastPacketSentTime
    }

    return true;
}


// ----------------------------------------------------------------
// Data transfer
// ----------------------------------------------------------------

bool LWSSocket::sendPacket(const Packet& packet) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!isValid()) {
        lwsdebug(PREFIX) << "Socket is not valid!";
        return false;
    }
    auto bytes = packet.serialize();

    // Update the last sent timestamp for any packet
    m_lastPacketSentTime = std::chrono::steady_clock::now();

    ssize_t bytesSent = ::send(m_fd, bytes.data(), bytes.size(), 0);

    return (bytesSent >= 0);
}

RawPacket LWSSocket::receivePacket(Packet::PacketType expectedType)
{
    RawPacket packet;

    // Set a timeout for the header
    int timeoutMs = static_cast<int>(m_keepAliveInterval.count());
    struct timeval timeout;

    // Apply different timeout strategies based on the configured inactivity timeout
    if (timeoutMs >= 5000) {
        // For timeouts ≥ 5000ms, use stable fixed values (5s or 2s)
        timeout.tv_sec  = (expectedType == Packet::PacketType::RegularMessage) ? 5 : 2;
        timeout.tv_usec = 0;
    } else if (timeoutMs >= 2000) {
        // For mid-range timeouts between 2000ms and 4999ms, use 2s
        timeout.tv_sec  = 2;
        timeout.tv_usec = 0;
    } else {
        // For fine-grained timeouts < 2000ms, convert milliseconds precisely
        timeout.tv_sec  = timeoutMs / 1000;
        timeout.tv_usec = (timeoutMs % 1000) * 1000;
    }

    setsockopt(m_fd, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&timeout), sizeof(timeout));

    // Read header (1 byte)
    uint8_t header = 0;
    ssize_t result = recv(m_fd, &header, 1, 0);
    if (result == 0) {
        setState(State::Closing);
        packet.setStatus(RawPacket::Status::ConnectionClosed);
        return packet;
    }
    if (result < 0) {
        int err = errno;
        //lwsdebug(PREFIX) << "recv() failed: errno = " << err << " (" << strerror(err) << ")";
        switch (err) {
        case EAGAIN:
            packet.setStatus(RawPacket::Status::Timeout); break;
        case ECONNRESET:
            setState(State::Closing);
            packet.setStatus(RawPacket::Status::ConnectionReset); break;
        case EBADF: case ENOTSOCK:
            setState(State::Error);
            packet.setStatus(RawPacket::Status::InvalidSocket); break;
        default:
            setState(State::Error);
            packet.setStatus(RawPacket::Status::ReceiveError); break;
        }
        return packet;
    }
    setState(State::Connected);

    packet.setType(static_cast<Packet::PacketType>(header));

    if (header == static_cast<uint8_t>(Packet::PacketType::Disconnect)) {
        lwsdebug(PREFIX) << "Disconnect packet received.";
        packet.setStatus(RawPacket::Status::DisconnectReceived);
        return packet;
    } else if (header == static_cast<uint8_t>(Packet::PacketType::KeepAlive)) {
        m_lastIncomingPacketTime = std::chrono::steady_clock::now();
        lwsdebug(PREFIX) << "Keep alive packet received.";
        packet.setStatus(RawPacket::Status::KeepAlive);
        return packet;
    } else if (expectedType != Packet::Unknown && header != static_cast<uint8_t>(expectedType)) {
        lwsdebug(PREFIX) << "Unexpected type: " << static_cast<int>(header);
        packet.setStatus(RawPacket::Status::UnexpectedType);
        return packet;
    }

    std::vector<uint8_t> buffer;

    if (header == static_cast<uint8_t>(Packet::RegularMessage)) {
        uint16_t netLength = 0;
        recvAll(packet, &netLength, sizeof(netLength));
        if (!packet.isSuccess()) {
            lwsdebug(PREFIX) << "Length read error";
            packet.setStatus(RawPacket::Status::LengthReadError);
            return packet;
        }

        uint16_t payloadLength = ntohs(netLength);
        payloadLength = (payloadLength + 7) / 8 * 8;

        buffer.resize(1 + 2 + payloadLength);
        buffer[0] = header;
        std::memcpy(&buffer[1], &netLength, sizeof(netLength));

        if (payloadLength > 0) {
            recvAll(packet, buffer.data() + 3, payloadLength);
            if (!packet.isSuccess()) {
                lwsdebug(PREFIX) << "Incomplete payload";
                packet.setStatus(RawPacket::Status::IncompletePayload);
                return packet;
            }
        }
    } else {
        size_t messageSize = 0;
        switch (header) {
        case Packet::HandshakeRequest:  messageSize = HandshakeRequestPacket::getExpectedSize(); break;
        case Packet::HandshakeResponse: messageSize = HandshakeResponsePacket::getExpectedSize(); break;
        case Packet::HandshakeSuccess:  messageSize = HandshakeSuccessPacket::getExpectedSize(); break;
        case Packet::Disconnect:        messageSize = 1; break;
        default:
            packet.setStatus(RawPacket::Status::UnknownPacketType);
            return packet;
        }

        buffer.resize(messageSize);
        buffer[0] = header;

        if (messageSize > 1) {
            recvAll(packet, buffer.data() + 1, messageSize - 1);
            if (!packet.isSuccess()) {
                lwsdebug(PREFIX) << "Incomplete packet";
                packet.setStatus(RawPacket::Status::IncompletePayload);
                return packet;
            }
        }
    }

    m_lastIncomingPacketTime = std::chrono::steady_clock::now();
    packet.setStatus(RawPacket::Status::Success);
    packet.setData(std::move(buffer));
    return packet;
}

void LWSSocket::recvAll(RawPacket& packet, void* buffer, std::size_t size)
{
    size_t totalBytesRead = 0;

    while (totalBytesRead < size) {
        ssize_t chunk = recv(m_fd, static_cast<uint8_t*>(buffer) + totalBytesRead, size - totalBytesRead, 0);

        if (chunk == 0) {
            lwsdebug(PREFIX) << "Connection closed by peer.";
            packet.setStatus(RawPacket::Status::ConnectionClosed);
            return;
        }

        if (chunk < 0) {
            int err = errno;
            lwsdebug(PREFIX) << "recv() failed. errno: " << err;

            switch (err) {
            case EAGAIN:
                packet.setStatus(RawPacket::Status::Timeout);
                break;
            case ECONNRESET:
                packet.setStatus(RawPacket::Status::ConnectionReset);
                break;
            case EBADF:
            case ENOTSOCK:
                packet.setStatus(RawPacket::Status::InvalidSocket);
                break;
            default:
                packet.setStatus(RawPacket::Status::ReceiveError);
                break;
            }

            return;
        }

        totalBytesRead += chunk;
    }

    if (totalBytesRead != size) {
        lwsdebug(PREFIX) << "Incomplete read: " << totalBytesRead << "/" << size;
        packet.setStatus(RawPacket::Status::IncompletePayload);
    }
}

// ----------------------------------------------------------------
// Disconnect / Close
// ----------------------------------------------------------------
void LWSSocket::disconnect() {
    if(m_state == State::Disconnected){
        return;
    }
    if (m_fd >= 0) {
        setState(State::Closing);
        ::shutdown(m_fd, SHUT_RDWR);
        ::close(m_fd);
        m_fd = -1;
        setState(State::Disconnected);
    }
}

void LWSSocket::closeSocket() {
    disconnect();
}

// ----------------------------------------------------------------
// Query
// ----------------------------------------------------------------
int LWSSocket::fd() const noexcept {
    return m_fd;
}

LWSSocket::State LWSSocket::state() const noexcept {
    return m_state;
}

bool LWSSocket::isValid() const noexcept {
    return m_fd > -1 && m_fd < FD_SETSIZE;
}

std::ostream& operator<<(std::ostream& os, const LWSSocket& socket) {
    os << "Socket[fd=" << socket.fd() << "]";
    return os;
}

std::string LWSSocket::peerAddress() const {
    sockaddr_in addr;
    socklen_t addrLen = sizeof(addr);
    if (getpeername(m_fd, (sockaddr*)&addr, &addrLen) == 0) {
        char ipStr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(addr.sin_addr), ipStr, sizeof(ipStr));
        return std::string(ipStr);
    }
    return "UNKNOWN";
}
