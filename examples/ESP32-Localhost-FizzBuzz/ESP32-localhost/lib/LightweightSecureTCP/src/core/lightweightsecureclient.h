#pragma once

#include <string>
#include <functional>
#include "../protocol/communication/handshakeresult.h"
#include "../protocol/communication/lwssocket.h"

class LightweightSecureClient {
public:
    LightweightSecureClient(const std::string& serverIp, int port);
    ~LightweightSecureClient();

    bool connectToServer();
    void disconnect();

    bool performHandshake();
    void handleConnection();
    void sendMessage(const std::string& message);

    bool isConnected() const;

private:
    LWSSocket           m_socket;
    std::string         m_serverIp;
    int                 m_port;
    HandshakeResult     handshakeResult;
    const std::string   PREFIX = "[LightweightSecureClient]";

    // Callbacks:
public:
    using SimpleCallback    = std::function<void()>;
    using MessageCallback   = std::function<void(const std::string& message)>;
    using ErrorCallback     = std::function<void(const std::string& errorMessage)>;
    using HandshakeCallback = std::function<void(const HandshakeResult&)>;

    void setOnConnected(SimpleCallback callback);
    void setOnDisconnected(SimpleCallback callback);
    void setOnMessageReceived(MessageCallback callback);
    void setOnHandshakeSuccess(HandshakeCallback callback);
    void setOnHandshakeFailed(HandshakeCallback callback);
    void setOnError(ErrorCallback callback);
    void setOnDataSent(MessageCallback callback);

private:
    SimpleCallback      onConnected;
    SimpleCallback      onDisconnected;
    MessageCallback     onMessageReceived;
    HandshakeCallback   onHandshakeSuccess;
    HandshakeCallback   onHandshakeFailed;
    ErrorCallback       onError;
    MessageCallback     onDataSent;
};

