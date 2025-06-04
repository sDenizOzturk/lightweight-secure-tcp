#pragma once

#include "../../encryption/key.h"
#include <string>

class HandshakeResult
{
public:
    HandshakeResult();
    HandshakeResult(bool success, const Key& sessionKey, const std::string& errorMessage = "", const std::string& ipAddress = "");

    bool isSuccessful() const;
    const Key& sessionKey() const;
    const std::string& errorMessage() const;
    const std::string& ipAddress() const;

    void setSuccess(bool newSuccess);
    void setSessionKey(const Key &newSessionKey);
    void setErrorMessage(const std::string &newErrorMessage);
    void setIpAddress(const std::string& newIpAddress);

private:
    bool m_success;
    Key m_sessionKey;
    std::string m_errorMessage;
    std::string m_ipAddress;
};

