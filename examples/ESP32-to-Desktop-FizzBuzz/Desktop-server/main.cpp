#include <iostream>
#include <thread>
#include <chrono>
#include <lightweightsecuretcp.h>

std::string fizzBuzz(int number)
{
    std::string result;
    if (number % 3 == 0)
        result += "Fizz";
    if (number % 5 == 0)
        result += "Buzz";
    if (number % 3 != 0 && number % 5 != 0)
        result += std::string((number % 4) + 2, ' ') + "(:";
    return result;
}

int main() {
    const int PORT = 12345;

    // Optional: enable debug
    LWSDebugSettings::instance().setEnabled(true);

    // Set handshake key
    Key handshakeKey({0x12345678, 0x9ABCDEF0, 0x13572468, 0xACEBDF01,
                      0x11223344, 0x55667788, 0x99AABBCC, 0xDDEEFF00});

    LightweightSecureTCP::setHandshakeKey(handshakeKey);

    LightweightSecureServer server(PORT);
    server.start();
    server.setKeepAliveConfig(1000, 5000);    
    server.setOnHandshakeSuccess([&](HandshakeResult, Session &session)
                                 { session.setOnMessageReceived([&](const std::string &message)
                                                                  {
                                                                      lwsdebug("[SERVER]") << "Received: " << message;
                                                                      int received = std::stoi(message);
                                                                      int next = received + 1;
                                                                      std::string text = fizzBuzz(next);

                                                                      std::string reply = std::to_string(next);
                                                                      if (!text.empty()) reply += " " + text;

                                                                      lwsdebug("[SERVER]") << "Sending: " << reply;

                                                                      std::this_thread::sleep_for(std::chrono::milliseconds(500));
                                                                      session.sendMessage(reply);
                                                                  });
                                 });
    lwsdebug("[Main]") << "Server is running on port " << PORT << std::endl; 
    while(true){
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }   
    server.stop();

    return 0;
}
