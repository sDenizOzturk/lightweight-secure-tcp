#include "test.h"
#include "../include/lightweightsecuretcp.h"
#include <thread>

#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


void testHandshakeConsistency(LightweightSecureServer &server, const int port, const std::string &prefix){
    const int targetHandshakes = 100;
    int successfullHandshakes = 0;

    server.setOnHandshakeSuccess([&](HandshakeResult, Session&) { successfullHandshakes++; });


    for(int i=1;i<=targetHandshakes;i++){
        lwsdebug(prefix) << "Attempt " + std::to_string(i);
        //LightweightSecureTCP::enableDebug(false);
        LightweightSecureClient client("127.0.0.1", port);        
        client.setOnHandshakeSuccess([&](HandshakeResult) { successfullHandshakes++; });
        client.connectToServer();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        client.disconnect();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        //LightweightSecureTCP::enableDebug(true);
        assert(successfullHandshakes == i*2);
    }
    lwsdebug(prefix) << "Test PASSED!";
}

int connectToServer(const int port, const std::string &prefix) {

    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        lwsdebug(prefix) << "Error creating socket";
        assert(false);
        return -1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    std::string serverIp = "127.0.0.1";
    inet_pton(AF_INET, serverIp.c_str(), &serverAddr.sin_addr);

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        lwsdebug(prefix) << "Connection failed";
        assert(false);
        return -1;
    }

    lwsdebug(prefix) << "Connected to server " << serverIp << ":" << port << std::endl;

    return clientSocket;
}

void testFailedHandshakes(LightweightSecureServer &server, const int port, const std::string &prefix) {

    int handshakeFailed = 0;
    server.setOnHandshakeFailed([&](HandshakeResult) {handshakeFailed++;});
    int clientSocket_disconnect = connectToServer(port, prefix);
    int clientSocket_sendMessage = connectToServer(port, prefix);
    int clientSocket_idle = connectToServer(port, prefix);


    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    lwsdebug(prefix) <<  "Checking disconnection on during handshake...";
    if (clientSocket_disconnect != -1) close(clientSocket_disconnect);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    ASSERT_AND_LOG(handshakeFailed==1);

    lwsdebug(prefix) << "Checking sending message handshake...";
    send(clientSocket_sendMessage, "INVALID_HANDSHAKE", 17, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    ASSERT_AND_LOG(handshakeFailed==2);

    lwsdebug(prefix) << "Checking sending no message during handshake...";
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    ASSERT_AND_LOG(handshakeFailed==3);

    if (clientSocket_sendMessage != -1) close(clientSocket_sendMessage);
    if (clientSocket_idle != -1) close(clientSocket_idle);
}

void testHandshake(){
    const std::string prefix = "\033[36m~~~ [testHandshake]";
    lwsdebug(prefix) << "Testing Handshakes...";
    const int PORT = 22221;
    LightweightSecureServer server(PORT);

    server.start();

    std::this_thread::sleep_for(std::chrono::microseconds(100));

    testHandshakeConsistency(server, PORT, prefix+"[testHandshakeConsistency]");

    std::this_thread::sleep_for(std::chrono::microseconds(100));

    testFailedHandshakes(server, PORT, prefix+"[testFailedHandshake]");

    server.stop();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    lwsdebug(prefix) << "Test PASSED!";
}
