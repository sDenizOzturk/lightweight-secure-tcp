#include "test.h"
#include <lightweightsecuretcp.h>

#include <thread>

void testSendMessageToServer(LightweightSecureServer &server, const int port, const std::string &prefix) {
    const int targetMessages = 10;
    int successfullMessages = 0;

    server.setOnHandshakeSuccess([&](HandshakeResult, Session &session) {
        session.setOnMessageReceived([&](const std::string& message) {
            if(message == "Test"+std::to_string(successfullMessages)){
                successfullMessages++;
            }
        });
    });

    for(int i=1;i<=targetMessages;i++){
        lwsdebug(prefix) << "Attempt " + std::to_string(i);
        //LightweightSecureTCP::enableDebug(false);
        LightweightSecureClient client("127.0.0.1", port);
        client.connectToServer();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        client.sendMessage("Test"+std::to_string(successfullMessages));
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        client.disconnect();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        //LightweightSecureTCP::enableDebug(true);
        assert(successfullMessages == i);
    }
    lwsdebug(prefix) << "Test PASSED!";
}

void testSendMessageFromServer(LightweightSecureServer &server, const int port, const std::string &prefix) {
    const int targetMessages = 10;
    int successfullMessages = 0;

    server.setOnHandshakeSuccess([&](HandshakeResult, Session &session) {
        session.sendMessage("Test"+std::to_string(successfullMessages));
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            session.disconnect();
    });

    for(int i=1;i<=targetMessages;i++){
        lwsdebug(prefix) << "Attempt " + std::to_string(i);
      //  LightweightSecureTCP::enableDebug(false);
        LightweightSecureClient client("127.0.0.1", port);

        client.setOnMessageReceived([&](const std::string& message) {
            if(message == "Test"+std::to_string(successfullMessages)){
                successfullMessages++;
            }
        });

        client.connectToServer();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
       // LightweightSecureTCP::enableDebug(true);
        assert(successfullMessages == i);
    }
    lwsdebug(prefix) << "Test PASSED!";
}

void testPingPong(LightweightSecureServer &server, const int port, const std::string &prefix) {
    bool pongReceived = false;

    server.setOnHandshakeSuccess([&](HandshakeResult, Session &session) {
        session.setOnMessageReceived([&](const std::string &message) {
            if (message == "PING") {
                lwsdebug(prefix) << "[Server] Got PING, sending PONG";
                session.sendMessage("PONG");
            }
        });
    });

    LightweightSecureClient client("127.0.0.1", port);
    client.setOnMessageReceived([&](const std::string &message) {
        if (message == "PONG") {
            lwsdebug(prefix) << "[Client] Got PONG, disconnecting";
            pongReceived = true;
            client.disconnect();
        }
    });

    client.connectToServer();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    client.sendMessage("PING");

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(100);
    while (!pongReceived && std::chrono::steady_clock::now() < deadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    assert(pongReceived); // test passes only if we got the PONG

    lwsdebug(prefix) << "Test PASSED!";
}

void testTCPCommunication() {
    const std::string prefix = "\033[32m~~~ [testTCPCommunication]";

    lwsdebug(prefix) << "Testing TCP Communication...";
    const int PORT = 44444;
    LightweightSecureServer server(PORT);

    server.start();

    std::this_thread::sleep_for(std::chrono::microseconds(100));
    testSendMessageToServer(server, PORT, prefix+"[testSendMessageToServer]");

    std::this_thread::sleep_for(std::chrono::microseconds(100));
    testSendMessageFromServer(server, PORT, prefix+"[testSendMessageFromServer]");

    std::this_thread::sleep_for(std::chrono::microseconds(100));
    testPingPong(server, PORT, prefix+"[testPingPong]");

    server.stop();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    lwsdebug(prefix) << "Test PASSED!";
}
