#include "lightweightsecureclient.h"
#include "protocol/communication/securityhandshake.h"
#include "utils/debugger.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "protocol/communication/transporter.h"
#include "utils/thread.h"
#include "encryption/xtea256.h"

LightweightSecureClient::LightweightSecureClient(const std::string& serverIp, int port)
    : m_serverIp(serverIp), m_port(port) {
    if (!XTEA256::hasValidKey()) {
        std::cerr << PREFIX << " Error: Encryption key not set. Call LightweightSecureTCP::setHandshakeKey(...) before creating server or client." << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

LightweightSecureClient::~LightweightSecureClient() {
    m_socket.disconnect();
    if (onError) onError("Client destroyed");
}


bool LightweightSecureClient::connectToServer() {
    if (!m_socket.connectTo(m_serverIp, m_port)) {
        lwsdebug(PREFIX) << "Connection failed";
        if (onError) onError("Connection failed");
        return false;
    }

    lwsdebug(PREFIX) << "Connected to server " << m_serverIp << ":" << m_port;
    if (onConnected) onConnected();

    if (performHandshake()) {
        handleConnection();
        return true;
    }
    return false;
}


bool LightweightSecureClient::performHandshake()
{
    SecurityHandshake securityHandshake;
    handshakeResult = securityHandshake.clientHandshake(m_socket);

    if (!handshakeResult.isSuccessful()) {
        lwsdebug(PREFIX) << "Handshake failed!";
        if (onHandshakeFailed) onHandshakeFailed(handshakeResult);
        m_socket.disconnect();
        return false;
    }

    if (onHandshakeSuccess) onHandshakeSuccess(handshakeResult);
    return true;
}


void LightweightSecureClient::handleConnection()
{
    Thread::runAsync([this]() {
        while (m_socket.isValid()) {
            Transporter transporter(m_socket);
            RegularMessagePacket msg = transporter.receiveRegularMessage(handshakeResult.sessionKey());

            if (!msg.isValid()) {
                if (onDisconnected) onDisconnected();
                m_socket.disconnect();
                break;
            }

            if (onMessageReceived) onMessageReceived(msg.getMessage());
        }
#ifdef ESP_PLATFORM
        vTaskDelay(1);
#endif
    });

}

void LightweightSecureClient::sendMessage(const std::string& message)
{
    Transporter transporter(m_socket);
    if (transporter.sendRegularMessage(message, handshakeResult.sessionKey())) {
        if (onDataSent) onDataSent(message);
    } else {
        if (onError) onError("Error sending message");
    }
}

bool LightweightSecureClient::isConnected() const
{
    return m_socket.state() == LWSSocket::State::Connected;
}

void LightweightSecureClient::disconnect()
{
    if (m_socket.isValid() && m_socket.state() == LWSSocket::State::Connected) {
        Transporter transporter(m_socket);
        transporter.sendDisconnect();

        m_socket.disconnect();

        if (onDisconnected) onDisconnected();
        lwsdebug(PREFIX) << "Disconnected";
    }
}

#include "utils/callbacksetter.h"
#define CURRENT_CLASS LightweightSecureClient

DEFINE_CALLBACK_SETTER(Connected,           SimpleCallback)
DEFINE_CALLBACK_SETTER(Disconnected,        SimpleCallback)
DEFINE_CALLBACK_SETTER(MessageReceived,     MessageCallback)
DEFINE_CALLBACK_SETTER(HandshakeSuccess,    HandshakeCallback)
DEFINE_CALLBACK_SETTER(HandshakeFailed,     HandshakeCallback)
DEFINE_CALLBACK_SETTER(Error,               ErrorCallback)
DEFINE_CALLBACK_SETTER(DataSent,            MessageCallback)
