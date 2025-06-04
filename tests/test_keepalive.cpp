#include "test.h"
#include <lightweightsecuretcp.h>

#include <thread>

void testServerIdleTimeout(LightweightSecureServer& server, int port, const std::string& prefix) {
    lwsdebug(prefix) << "Testing server-side timeout (client idle)...";

    server.setKeepAliveConfig(500, 1500); // short timeout for test

    bool disconnected = false;

    server.setOnHandshakeSuccess([&](HandshakeResult, Session &session) {
        session.setOnSessionEnded([&]() {
            disconnected = true;
            lwsdebug(prefix) << "Session ended (as expected)";
        });
    });

    LightweightSecureClient client("127.0.0.1", port);
    client.setKeepAliveConfig(100, -1);  // client won't send keepalive

    client.setOnDisconnected([&]() {
        lwsdebug(prefix) << "Client disconnected (not expected)";
    });

    if (!client.connectToServer()) {
        throw std::runtime_error("Client could not connect to server.");
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(3000));

    if (!disconnected) {
        throw std::runtime_error("Server did not disconnect the idle client as expected.");
    }

    client.disconnect();
    lwsdebug(prefix) << "Server idle timeout test passed";


    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    server.start();
}


void testClientIdleTimeout(LightweightSecureServer& server, int port, const std::string& prefix) {
    lwsdebug(prefix) << "Testing client-side timeout (server idle)...";

    server.setKeepAliveConfig(100, -1);  // server won't send keepalive (interval 0)

    LightweightSecureClient client("127.0.0.1", port);
    client.setKeepAliveConfig(500, 1500);  // client will expect activity within 1.5s

    bool disconnected = false;

    client.setOnDisconnected([&]() {
        disconnected = true;
        lwsdebug(prefix) << "Client disconnected (as expected)";
    });

    server.setOnHandshakeSuccess([&](HandshakeResult, Session &session) {
        session.setOnSessionEnded([&]() {
            lwsdebug(prefix) << "Session ended (not expected)";
        });
    });

    if (!client.connectToServer()) {
        throw std::runtime_error("Client could not connect to server.");
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(3000));

    if (!disconnected) {
        throw std::runtime_error("Client did not disconnect from idle server as expected.");
    }

    client.disconnect();
    lwsdebug(prefix) << "Client idle timeout test passed";

    //  std::this_thread::sleep_for(std::chrono::milliseconds(100));
    //server.start();
}

void testConnectionIdleTimeout(LightweightSecureServer& server, int port, const std::string& prefix) {
    lwsdebug(prefix) << "Testing idle connection should stay alive...";

    server.setKeepAliveConfig(5000,30000);

    bool disconnected = false;
    bool messageReceived = false;

    server.setOnHandshakeSuccess([&](HandshakeResult, Session &session) {
        session.setOnSessionEnded([&]() {
            disconnected = true;
            lwsdebug(prefix) << "Session ended (should NOT happen during idle)";
        });
        session.setOnMessageReceived([&](const std::string& message) {
            messageReceived = (message == "hello-after-idle");
            lwsdebug(prefix) << "Server received message: " << message;
        });
    });

    LightweightSecureClient client("127.0.0.1", port);

    client.setOnDisconnected([&]() {
        disconnected = true;
        lwsdebug(prefix) << "Client disconnected (should NOT happen during idle)";
    });


    bool connected = client.connectToServer();
    if (!connected) {
        throw std::runtime_error("Client could not connect to server.");
    }

    // Wait for idle duration
    for (int i = 8; i > 0; --i) {
        lwsdebug(prefix) << i << "...";
        if (i == 2) {
            lwsdebug(prefix) << "Sending message after 6 seconds of idle";
            client.sendMessage("hello-after-idle");
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));

        if (disconnected) {
            throw std::runtime_error("Disconnected during idle — timeout behavior is incorrect.");
        }
    }

    if (!messageReceived) {
        throw std::runtime_error("Message did not received by server.");
    }

    client.disconnect();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    lwsdebug(prefix) << "Idle timeout test passed (connection remained stable)";
}

void testKeepAlive() {
    const std::string prefix = "\033[32m~~~ [testKeepAlive]";

    lwsdebug(prefix) << "Testing Keep Alive...";
    const int PORT = 33333;
    LightweightSecureServer server(PORT);
    server.start();

    std::this_thread::sleep_for(std::chrono::microseconds(100));
    testClientIdleTimeout(server, PORT, prefix + "[testClientIdleTimeout]");

    std::this_thread::sleep_for(std::chrono::microseconds(100));
    testServerIdleTimeout(server, PORT, prefix + "[testServerIdleTimeout]");

    std::this_thread::sleep_for(std::chrono::microseconds(100));
    testConnectionIdleTimeout(server, PORT, prefix + "[testConnectionIdleTimeout]");

    server.stop();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    lwsdebug(prefix) << "Test PASSED!";
}
