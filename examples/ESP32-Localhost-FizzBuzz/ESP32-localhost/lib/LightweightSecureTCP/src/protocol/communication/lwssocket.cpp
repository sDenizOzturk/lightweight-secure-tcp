#include "lwssocket.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include "utils/debugger.h"
#include "protocol/packet/handshakerequestpacket.h"
#include "protocol/packet/handshakeresponsepacket.h"
#include "protocol/packet/handshakesuccesspacket.h"

// ----------------------------------------------------------------
// Constructor / Destructor
// ----------------------------------------------------------------
LWSSocket::LWSSocket()
    : m_fd(-1),
    m_state(State::Disconnected)
{}

LWSSocket::~LWSSocket() {
    disconnect();
}

// Move semantics
LWSSocket::LWSSocket(LWSSocket&& other) noexcept {
    std::lock_guard<std::mutex> lock(other.m_mutex);
    m_fd    = other.m_fd;
    m_state = other.m_state;
    other.m_fd    = -1;
    other.m_state = State::Disconnected;
}

LWSSocket& LWSSocket::operator=(LWSSocket&& other) noexcept {
    if (this != &other) {
        disconnect();
        std::lock_guard<std::mutex> lockFrom(other.m_mutex);
        m_fd    = other.m_fd;
        m_state = other.m_state;
        other.m_fd    = -1;
        other.m_state = State::Disconnected;
    }
    return *this;
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

    LWSSocket clientSock;
    {
        std::lock_guard<std::mutex> clientLock(clientSock.m_mutex);
        clientSock.m_fd    = clientFd;
        clientSock.m_state = State::Connected;
    }
    return clientSock;
}

// ----------------------------------------------------------------
// Data transfer
// ----------------------------------------------------------------
ssize_t LWSSocket::sendBytes(const void* data, size_t size) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_fd < 0) return -1;
    return ::send(m_fd, data, size, 0);
}

ssize_t LWSSocket::sendBytes(const std::vector<uint8_t>& data) {
    return sendBytes(data.data(), data.size());
}

ssize_t LWSSocket::receiveBytes(void* buffer, size_t maxSize) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_fd < 0) return -1;
    return ::recv(m_fd, buffer, maxSize, 0);
}

std::vector<uint8_t> LWSSocket::receiveBytes(size_t maxSize) {
    std::vector<uint8_t> buf;
    buf.resize(maxSize);
    ssize_t n = receiveBytes(buf.data(), maxSize);
    if (n > 0) {
        buf.resize(static_cast<size_t>(n));
    } else {
        buf.clear();
    }
    return buf;
}

bool LWSSocket::sendPackage(const std::vector<uint8_t>& packet) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!isValid()) {
        lwsdebug(PREFIX) << "Socket is not valid!";
        return false;
    }

    ssize_t bytesSent = ::send(m_fd, packet.data(), packet.size(), 0);

    return (bytesSent >= 0);
}


std::vector<uint8_t> LWSSocket::receivePackage(Packet::PacketType expectedType)
{
    // Set a timeout of 2 seconds for the header
    struct timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;
    setsockopt(m_fd, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&timeout), sizeof(timeout));

    // Read the header (1 byte)
    uint8_t header = 0;
    if (recv(m_fd, &header, 1, 0) <= 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            lwsdebug(PREFIX) << "Socket timeout (header read)";
        } else if (errno == ECONNRESET) {
            lwsdebug(PREFIX) << "Connection reset by peer (header read)";
        } else if (errno == EBADF || errno == ENOTSOCK) {
            lwsdebug(PREFIX) << "Invalid socket or not a socket";
        } else if (errno == 0) {
            lwsdebug(PREFIX) << "Connection closed cleanly (recv returned 0)";
        } else {
            lwsdebug(PREFIX) << "recv() failed while reading header. errno: " << errno;
        }
        return {};
    }

    if (expectedType != Packet::Unknown && header != static_cast<uint8_t>(expectedType)) {
        lwsdebug(PREFIX) << "Unexpected type: " << static_cast<int>(header);
        return {};
    }

    std::vector<uint8_t> buffer;

    if (header == static_cast<uint8_t>(Packet::RegularMessage)) {
        // For RegularMessage, first read the 2-byte length field.
        uint16_t netLength = 0;
        if (!recvAll(&netLength, sizeof(netLength))) {
            lwsdebug(PREFIX) << "Length read error";
            return {};
        }
        uint16_t payloadLength = ntohs(netLength);

        payloadLength = (payloadLength + 7) / 8 * 8; // Round up to the next multiple of 8

        // Total packet size: 1 (header) + 2 (length field) + payloadLength (payload)
        size_t totalSize = 1 + 2 + payloadLength;
        buffer.resize(totalSize);

        // Store header and the length field into the buffer.
        buffer[0] = header;
        std::memcpy(&buffer[1], &netLength, sizeof(netLength));

        // Read only the payload bytes.
        if (payloadLength > 0 && !recvAll(buffer.data() + 3, payloadLength)) {
            lwsdebug(PREFIX) << "Incomplete packet";
            return {};
        }
    } else {
        // For other packet types, determine the expected size.
        size_t messageSize = 0;
        switch (header) {
        case Packet::HandshakeRequest:
            messageSize = HandshakeRequestPacket::getExpectedSize();
            break;
        case Packet::HandshakeResponse:
            messageSize = HandshakeResponsePacket::getExpectedSize();
            break;
        case Packet::HandshakeSuccess:
            messageSize = HandshakeSuccessPacket::getExpectedSize();
            break;
        case Packet::Disconnect:
            messageSize = 1;
            break;
        default:
            return {};
        }
        buffer.resize(messageSize);
        buffer[0] = header;
        if (messageSize > 1 && !recvAll(buffer.data() + 1, messageSize - 1)) {
            lwsdebug(PREFIX) << "Incomplete packet";
            return {};
        }
    }
    return buffer;
}

bool LWSSocket::recvAll(void *buffer, std::size_t size)
{
    size_t totalBytesRead = 0;

    struct timeval originalTimeout;
    socklen_t len = sizeof(originalTimeout);
    getsockopt(m_fd, SOL_SOCKET, SO_RCVTIMEO, &originalTimeout, &len);

    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    setsockopt(m_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    while (totalBytesRead < size) {
        ssize_t chunk = recv(m_fd, static_cast<uint8_t*>(buffer) + totalBytesRead, size - totalBytesRead, 0);
        if (chunk == 0) {
            lwsdebug(PREFIX) << "Connection closed by peer.";
            break;
        }
        if (chunk < 0) {
            lwsdebug(PREFIX) << "recv timeout or error!";
            return false;
        }
        totalBytesRead += chunk;

    }
    setsockopt(m_fd, SOL_SOCKET, SO_RCVTIMEO, &originalTimeout, sizeof(originalTimeout));

    return (totalBytesRead == size);
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
