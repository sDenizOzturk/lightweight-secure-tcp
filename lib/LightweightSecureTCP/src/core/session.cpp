#include "session.h"
#include "protocol/communication/transporter.h" // Adjust include if needed
#include <sys/socket.h>
#include <unistd.h>
#include "utils/thread.h"
#include "utils/lwsdebug.h"

std::atomic<bool> Session::defaultRunningFlag{true};


Session::Session(LWSSocket socket,
                 const Key& sessionKey,
                 std::atomic<bool>* runningFlag)
    : m_socket(std::move(socket))
    , m_sessionKey(sessionKey)
    , m_runningPtr(runningFlag ? runningFlag : &defaultRunningFlag)
{
    lwsdebug(PREFIX) << "Started.";
    Thread::runAsync([this]() { handleCommunication(); });
}


Session::~Session()
{
    disconnect();
    lwsdebug(PREFIX) << "Ended...";
    m_active = false;
}

bool Session::isActive() const
{
    return m_active;
}

const Key& Session::sessionKey() const
{
    return m_sessionKey;
}

void Session::sendMessage(const std::string& message)
{
    if (!m_socket.isValid()) {
        if (onError) {
            onError("Session is closed, cannot send message.");
        }
        return;
    }
    
    Transporter transporter(m_socket);
    bool success = transporter.sendRegularMessage(message, m_sessionKey);
    if (!success && onError) {
        onError("Failed to send message.");
    }
    if (onDataSent) {
        onDataSent(message);
    }
}

void Session::disconnect()
{
    if (m_socket.isValid() && m_socket.state() == LWSSocket::State::Connected) {
        Transporter transporter(m_socket);
        transporter.sendDisconnect(); // Notify the other side
    }
    
    if(m_socket.state() != LWSSocket::State::Disconnected) {
        m_socket.disconnect(); // Proper shutdown and close handled internally

        if (onSessionEnded) {
            onSessionEnded();
        }
    }
}

void Session::handleCommunication()
{
    while (*m_runningPtr && m_socket.isValid()) {
        Transporter transporter(m_socket);
        RegularMessagePacket regularMessagePacket = transporter.receiveRegularMessage(m_sessionKey);
        std::string message = regularMessagePacket.getMessage();
        
        if(regularMessagePacket.isValid()){
            if (onMessageReceived) {
                onMessageReceived(message);
            }
        }

        if (regularMessagePacket.connectionClosed() ||
            m_socket.state() == LWSSocket::State::Closing||
            m_socket.state() == LWSSocket::State::Error ||
            !transporter.checkKeepAlive()) {
            if (onSessionEnded) {
                onSessionEnded();
            }
            break;
        }
#ifdef ESP_PLATFORM
        vTaskDelay(1);
#endif
    }
    disconnect();
    m_active = false;
}


#include "utils/callbacksetter.h"
#define CURRENT_CLASS Session

DEFINE_CALLBACK_SETTER(MessageReceived, MessageCallback)
DEFINE_CALLBACK_SETTER(SessionEnded,    SimpleCallback)
DEFINE_CALLBACK_SETTER(Error,           ErrorCallback)
DEFINE_CALLBACK_SETTER(DataSent,        MessageCallback)
