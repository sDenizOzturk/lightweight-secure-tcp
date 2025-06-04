#pragma once

#include <atomic>
#include <string>
#include <functional>
#include "encryption/key.h"
#include "protocol/communication/lwssocket.h"

class Session {
public:
    Session(LWSSocket socket,
            const Key& sessionKey,
            std::atomic<bool>* runningFlag = nullptr);
    ~Session();

    bool isActive() const;

    const LWSSocket& socket() const;
    const Key& sessionKey() const;

    void sendMessage(const std::string& message);
    void disconnect();

private:
    bool        m_active = true;
    LWSSocket   m_socket;
    Key         m_sessionKey;

    void handleCommunication();

    const std::string           PREFIX = "[Session]";
    std::atomic<bool>           *m_runningPtr;
    static std::atomic<bool>    defaultRunningFlag;

    // Callbacks:
public:
    using SimpleCallback    = std::function<void()>;
    using MessageCallback   = std::function<void(const std::string&)>;
    using ErrorCallback     = std::function<void(const std::string&)>;

    void setOnSessionEnded (SimpleCallback callback);
    void setOnMessageReceived(MessageCallback callback);
    void setOnError(ErrorCallback callback);
    void setOnDataSent(MessageCallback callback);

private:
    SimpleCallback      onSessionEnded;
    MessageCallback     onMessageReceived;
    ErrorCallback       onError;
    MessageCallback     onDataSent;
};

