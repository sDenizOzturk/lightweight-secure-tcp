#include "session.h"
#include "protocol/communication/transporter.h" // Adjust include if needed
#include <sys/socket.h>
#include <unistd.h>
#include "utils/thread.h"
#include "utils/debugger.h"

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

const LWSSocket& Session::socket() const {
    return m_socket;
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
    
    m_socket.disconnect(); // Proper shutdown and close handled internally
    
    if (onSessionEnded) {
        onSessionEnded();
    }
}

void Session::handleCommunication()
{
    while (*m_runningPtr && m_socket.isValid()) {
        Transporter transporter(m_socket);
        RegularMessagePacket regularMessage = transporter.receiveRegularMessage(m_sessionKey);
        std::string message = regularMessage.getMessage();
        
        if (!regularMessage.isValid()) {
            if (onSessionEnded) {
                onSessionEnded();
            }
            break;
        } else if (message == "Error: Error reading from socket") {
            if (onError) {
                onError("Error reading from socket");
            }
        } else if (onMessageReceived) {
            onMessageReceived(message);
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
