#include "securityhandshake.h"
#include "utils/debugger.h"
#include "protocol/communication/transporter.h"
#include <sys/socket.h>
#include <cstring>
#include "utils/randomgenerator.h"
#include "handshakeresult.h"

HandshakeResult SecurityHandshake::serverHandshake(LWSSocket& clientSocket) {
    lwsdebug(PREFIX_SERVER) << "Starting server handshake...";

    HandshakeResult handshakeResult;

    uint64_t num1 = RandomGenerator::randomUint64();
    uint64_t num2 = RandomGenerator::randomUint64();
    uint32_t nonce = RandomGenerator::randomUint32();
    uint64_t sum = num1 + num2;
    uint64_t _xor = num1 ^ num2;
    uint32_t transformedNonce = nonce ^ *reinterpret_cast<uint32_t*>(&sum) ^ *reinterpret_cast<uint32_t*>(&_xor);

    Transporter transporter(clientSocket);
    handshakeResult.setIpAddress(clientSocket.peerAddress());

    if (!transporter.sendHandshakeRequest(nonce, num1, num2)) {
        lwsdebug(PREFIX_SERVER) << "Failed to send handshake request";
        return handshakeResult;
    }

    auto responsePacket = transporter.receiveHandshakeResponse();
    if (!responsePacket.isValid()) {
        lwsdebug(PREFIX_SERVER) << "No handshake response";
        return handshakeResult;
    }

    uint32_t rNonce = responsePacket.getNonce();
    if (rNonce != transformedNonce) {
        lwsdebug(PREFIX_SERVER) << "Nonce mismatch" << " rNonce: " << rNonce << " nonce: " << nonce;
        return handshakeResult;
    }

    uint64_t rSum = responsePacket.getSum();
    if (sum != rSum) {
        lwsdebug(PREFIX_SERVER) << "Sum mismatch" << " rSum: " << rSum << " sum: " << sum;
        return handshakeResult;
    }

    uint64_t rXor = responsePacket.getXor();
    if (sum != rSum) {
        lwsdebug(PREFIX_SERVER) << "XOR mismatch" << " rXor: " << rXor << " _xor: " << _xor;
        return handshakeResult;
    }


    auto successPacket = transporter.sendHandshakeSuccess();

    handshakeResult.setSessionKey(Key(successPacket.sessionKey()));

    if (!successPacket.isValid()) {
        lwsdebug(PREFIX_SERVER) << "Failed to send success response";
        return handshakeResult;
    }

    handshakeResult.setSuccess(true);

    lwsdebug(PREFIX_SERVER) << "Server handshake successful!";
    return handshakeResult;
}

HandshakeResult SecurityHandshake::clientHandshake(LWSSocket& serverSocket) {
    lwsdebug(PREFIX_CLIENT) << "Starting client handshake...";

    HandshakeResult handshakeResult;

    Transporter transporter(serverSocket);
    handshakeResult.setIpAddress(serverSocket.peerAddress());
    auto requestPacket = transporter.receiveHandshakeRequest();

    if (!requestPacket.isValid()) {
        lwsdebug(PREFIX_CLIENT) << "No handshake request";
        handshakeResult.setErrorMessage("No handshake request");
        return handshakeResult;
    }

    uint32_t nonce = requestPacket.getNonce();
    uint64_t num1 = requestPacket.getNum1();
    uint64_t num2 = requestPacket.getNum2();
    uint64_t sum = num1 + num2;
    uint64_t _xor = num1 ^ num2;    
    uint32_t transformedNonce = nonce ^ *reinterpret_cast<uint32_t*>(&sum) ^ *reinterpret_cast<uint32_t*>(&_xor);

    if (!transporter.sendHandshakeResponse(transformedNonce, sum, _xor)) {
        lwsdebug(PREFIX_CLIENT) << "Failed to send handshake response";
        handshakeResult.setErrorMessage("Failed to send handshake response");
        return handshakeResult;
    }

    auto successPacket = transporter.receiveHandshakeSuccess();

    handshakeResult.setSessionKey(Key(successPacket.sessionKey()));

    if (!successPacket.isValid()) {
        lwsdebug(PREFIX_CLIENT) << "No handshake successful packet";
        handshakeResult.setErrorMessage("No handshake successful packet");
        return handshakeResult;
    }

    handshakeResult.setSuccess(true);

    lwsdebug(PREFIX_CLIENT) << "Client handshake successful!";
    return handshakeResult;
}
