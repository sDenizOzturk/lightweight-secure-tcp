#pragma once

#include <atomic>
#include <functional>
#include <string>
#include "protocol/communication/handshakeresult.h"
#include "protocol/communication/lwssocket.h"
#include "session.h"

class LightweightSecureServer {
    int m_keepAliveIntervalMs, m_inactivityTimeoutMs;

public:
    LightweightSecureServer(int port);
    ~LightweightSecureServer();

    void setKeepAliveConfig(int keepAliveIntervalMs, int inactivityTimeoutMs);

    void start();
    void stop();

private:
    LWSSocket m_serverSocket;
    int m_port;
    std::atomic<bool> m_running{false};

    HandshakeResult performHandshake(LWSSocket& clientSocket);

    void handleConnection(int clientSocket, const HandshakeResult &handshakeResult);

    const std::string PREFIX = "[LightweightSecureServer]";

    // Callbacks:
public:
    using SimpleCallback            = std::function<void()>;
    using ClientCallback            = std::function<void(int clientSocket, const std::string& peerAddress)>;
    using ErrorCallback             = std::function<void(const std::string& errorMessage)>;
    using HandshakeSuccessCallback  = std::function<void(const HandshakeResult&, Session&)>;
    using HandshakeFailedCallback   = std::function<void(const HandshakeResult&)>;

    // Event Handlers
    void setOnServerStarted(SimpleCallback callback);
    void setOnServerStopped(SimpleCallback callback);
    void setOnHandshakeStarted(ClientCallback callback);
    void setOnError(ErrorCallback callback);
    void setOnHandshakeSuccess(HandshakeSuccessCallback callback);
    void setOnHandshakeFailed(HandshakeFailedCallback callback);

private:
    SimpleCallback              onServerStarted;
    SimpleCallback              onServerStopped;
    ClientCallback              onHandshakeStarted;
    ErrorCallback               onError;
    HandshakeSuccessCallback    onHandshakeSuccess;
    HandshakeFailedCallback     onHandshakeFailed;

};

