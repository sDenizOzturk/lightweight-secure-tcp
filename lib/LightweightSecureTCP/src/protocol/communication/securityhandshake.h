#pragma once
#include "handshakeresult.h"
#include <string>
#include "./lwssocket.h"

class SecurityHandshake
{
    const std::string PREFIX_CLIENT = "[SecurityHandshake][Client]";
    const std::string PREFIX_SERVER = "[SecurityHandshake][Server]";
public:
    SecurityHandshake() = default;
    HandshakeResult serverHandshake(LWSSocket& clientSocket);
    HandshakeResult clientHandshake(LWSSocket& serverSocket);
};

