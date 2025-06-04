#include "test.h"
#include "../src/core/lightweightsecureserver.h"
#include "../src/core/lightweightsecureclient.h"
#include <thread>

void testServerEvents(const std::string &prefix) {
    bool started             = false;
    bool stopped             = false;
    bool handshakeStarted    = false;
    bool handshakeSuccess    = false;
    bool errorOccurred       = false;

    const int PORT = 11111;
    LightweightSecureServer server(PORT);

    server.setOnServerStarted([&]() {
        started = true;
        lwsdebug(prefix) << "Server started";
    });

    server.setOnServerStopped([&]() {
        stopped = true;
       // lwsdebug(prefix) << "Server stopped";
    });

    server.setOnHandshakeStarted([&](int sock, const std::string& ip) {
        handshakeStarted = true;
        lwsdebug(prefix) << "Client connected from: " << ip << " (fd=" << sock << ")";
    });

    server.setOnHandshakeSuccess([&](const HandshakeResult& handshakeResult, Session&) {
        handshakeSuccess = handshakeResult.isSuccessful();
        lwsdebug(prefix) << "Handshake success: " << (handshakeResult.isSuccessful() ? "yes" : "no");
    });

    server.start();

    LightweightSecureClient client("127.0.0.1", PORT);
    client.connectToServer();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    client.disconnect();


    LightweightSecureServer serverDublicate(PORT);

    serverDublicate.setOnError([&](const std::string& msg) {
        errorOccurred = true;
        lwsdebug(prefix) << "Error occurred: " << msg;
    });
    serverDublicate.start();

    server.stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    ASSERT_AND_LOG(started);
    ASSERT_AND_LOG(handshakeStarted);
    ASSERT_AND_LOG(handshakeSuccess);
    ASSERT_AND_LOG(stopped);
    ASSERT_AND_LOG(errorOccurred);
    lwsdebug(prefix) << "PASSED!";
}

void testClientEvents(const std::string &prefix) {
    bool connected          = false;
    bool disconnected       = false;
    bool handshakeSuccess   = false;
    bool errorOccurred      = false;
    bool messageReceived    = false;
    bool dataSent           = false;

    const int PORT = 11112;
    LightweightSecureServer server(PORT);

    server.setOnHandshakeSuccess([&](const HandshakeResult&, Session& session) {
        session.sendMessage("Hello");
    });

    server.start();

    LightweightSecureClient client("127.0.0.1", PORT);

    client.setOnConnected([&]() {
        connected = true;
        lwsdebug(prefix) << "Client connected";
    });

    client.setOnDisconnected([&]() {
        disconnected = true;
        lwsdebug(prefix) << "Client disconnected";
    });

    client.setOnHandshakeSuccess([&](const HandshakeResult& result) {
        handshakeSuccess = result.isSuccessful();
        lwsdebug(prefix) << "Client handshake success";
    });

    client.setOnError([&](const std::string& msg) {
        errorOccurred = true;
        lwsdebug(prefix) << "Client error: " << msg;
    });

    client.setOnMessageReceived([&](const std::string& msg) {
        if (msg == "Hello") {
            messageReceived = true;
            lwsdebug(prefix) << "Client received: " << msg;
        }
    });

    client.setOnDataSent([&](const std::string& msg) {
        if (msg == "TestMessage") {
            dataSent = true;
            lwsdebug(prefix) << "Client sent: " << msg;
        }
    });

    client.connectToServer();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    client.sendMessage("TestMessage");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    client.disconnect();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    server.stop();

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    client.sendMessage("TestMessage");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    ASSERT_AND_LOG(connected);
    ASSERT_AND_LOG(disconnected);
    ASSERT_AND_LOG(handshakeSuccess);
    ASSERT_AND_LOG(messageReceived);
    ASSERT_AND_LOG(dataSent);
    ASSERT_AND_LOG(errorOccurred);

    lwsdebug(prefix) << "PASSED!";
}

void testSessionEvents(const std::string& prefix) {
    bool sessionEnded       = false;
    bool messageReceived    = false;
    bool dataSent           = false;
    bool errorOccurred     = false;

    const int PORT = 11113;
    LightweightSecureServer server(PORT);
    server.setOnHandshakeSuccess([&](const HandshakeResult& r, Session& session) {
        session.setOnMessageReceived([&](const std::string& msg) {
            messageReceived = (msg == "Hello");
            lwsdebug(prefix) << "Message received: " << msg;
        });
        session.setOnDataSent([&](const std::string& msg) {
            dataSent = (msg == "Hi");
            lwsdebug(prefix) << "Data sent: " << msg;
        });
        session.setOnError([&](const std::string& err) {
            errorOccurred = true;
            lwsdebug(prefix) << "Error: " << err;
        });
        session.setOnSessionEnded([&]() {
            sessionEnded = true;
            lwsdebug(prefix) << "Session ended";
            session.sendMessage("Hi");
        });

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        session.sendMessage("Hi again :)");
    });

    server.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    LightweightSecureClient client("127.0.0.1", PORT);
    client.connectToServer();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    client.sendMessage("Hello");
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    client.disconnect();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    server.stop();


    ASSERT_AND_LOG(messageReceived);
    ASSERT_AND_LOG(dataSent);
    ASSERT_AND_LOG(sessionEnded);
    ASSERT_AND_LOG(errorOccurred);

    lwsdebug(prefix) << "testSessionEvents PASSED!";
}


void testEventHandlers() {

    const std::string prefix = "\033[32m~~~ [testEventHandlers]";

    lwsdebug(prefix) << "Testing Event Handlers...";

    testServerEvents(prefix+"[testServerEvents]");
    testClientEvents(prefix+"[testClientEvents]");
    testSessionEvents(prefix+"[testSessionEvents]");

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    lwsdebug(prefix) << "Test PASSED!";

}
