#include "utils/debugger.h"
#include "lightweightsecureserver.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <unistd.h>
#include "protocol/communication/securityhandshake.h"
#include "protocol/communication/transporter.h"
#include "utils/thread.h"
#include "session.h"
#include <memory>
#include "encryption/xtea256.h"

LightweightSecureServer::LightweightSecureServer(int port)
    : m_port(port)
    , m_running(false)
{
    if (!XTEA256::hasValidKey()) {
        std::cerr << PREFIX << " Error: Encryption key not set. Call LightweightSecureTCP::setHandshakeKey(...) before creating server or client." << std::endl;
        std::exit(EXIT_FAILURE);
    }
}


LightweightSecureServer::~LightweightSecureServer() {
    stop();
}

void LightweightSecureServer::start() {
    // 1) Bind
    if (!m_serverSocket.bindTo(m_port)) {
        lwsdebug(PREFIX) << "Error creating or binding socket";
        if (onError) onError("Error creating or binding socket");
        return;
    }


    // 2) Listen
    if (!m_serverSocket.listenOn(5)) {
        lwsdebug(PREFIX) << "Listen failed";
        if (onError) onError("Listen failed");
        return;
    }



    m_running = true;
    lwsdebug(PREFIX) << "Server started on port " << m_port;
    if (onServerStarted) onServerStarted();



    // 3) Accept loop
    Thread::runAsync([this]() {
        while (m_running) {
            auto clientOpt = m_serverSocket.acceptConnection();
            if (!clientOpt.has_value()) {
                continue;
            }

            LWSSocket clientSock = std::move(*clientOpt);
            if (onHandshakeStarted) onHandshakeStarted(clientSock.fd(), clientSock.peerAddress());

            auto sock = std::make_shared<LWSSocket>(std::move(clientSock));
            Thread::runAsync([this, sock]() {
                HandshakeResult result = performHandshake(*sock);
                if (result.isSuccessful()) {
                    auto session = Session(std::move(*sock), result.sessionKey(), &m_running);
                    if (onHandshakeSuccess)
                        onHandshakeSuccess(result, session);
                    while (session.isActive()) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    }
                } else {
                    sock->disconnect();
                }
            });
#ifdef ESP_PLATFORM
            vTaskDelay(1);
#endif
        }

        m_serverSocket.disconnect();
        if (onServerStopped) onServerStopped();
    });
}

HandshakeResult LightweightSecureServer::performHandshake(LWSSocket& clientSocket)
{
    SecurityHandshake securityHandshake;
    // Perform the handshake over the underlying file descriptor
    HandshakeResult result = securityHandshake.serverHandshake(clientSocket);

    if (!result.isSuccessful()) {
        lwsdebug(PREFIX) << "Server handshake failed. Closing connection.";
        if (onHandshakeFailed)
            onHandshakeFailed(result);
    }

    return result;
}

void LightweightSecureServer::stop() {
    if(!m_running) return;

    // Signal accept loop to exit
    m_running = false;

    // Cleanly shut down and close the listening socket
    if (m_serverSocket.isValid()) {
        lwsdebug(PREFIX) << "Stopping server…";
        m_serverSocket.disconnect();    // calls shutdown + close internally
        if (onServerStopped) onServerStopped();
        lwsdebug(PREFIX) << "Server stopped";
    }
}

#include "utils/callbacksetter.h"
#define CURRENT_CLASS LightweightSecureServer

DEFINE_CALLBACK_SETTER(ServerStarted, SimpleCallback)
DEFINE_CALLBACK_SETTER(ServerStopped, SimpleCallback)
DEFINE_CALLBACK_SETTER(HandshakeStarted, ClientCallback)
DEFINE_CALLBACK_SETTER(Error, ErrorCallback)
DEFINE_CALLBACK_SETTER(HandshakeSuccess, HandshakeSuccessCallback)
DEFINE_CALLBACK_SETTER(HandshakeFailed, HandshakeFailedCallback)
//DEFINE_CALLBACK_SETTER(MessageReceived, MessageCallback)
//DEFINE_CALLBACK_SETTER(DataSent, DataSentCallback)
