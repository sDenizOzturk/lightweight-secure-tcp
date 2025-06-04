#include "handshakeresult.h"

HandshakeResult::HandshakeResult() : m_success(false) {}

HandshakeResult::HandshakeResult(bool success, const Key& sessionKey, const std::string& errorMessage, const std::string &ipAddress)
    : m_success(success), m_sessionKey(sessionKey), m_errorMessage(errorMessage), m_ipAddress(ipAddress) {}

bool HandshakeResult::isSuccessful() const {
    return m_success;
}

const Key& HandshakeResult::sessionKey() const {
    return m_sessionKey;
}

const std::string& HandshakeResult::errorMessage() const {
    return m_errorMessage;
}

const std::string &HandshakeResult::ipAddress() const
{
    return m_ipAddress;
}

void HandshakeResult::setSuccess(bool newSuccess)
{
    m_success = newSuccess;
}

void HandshakeResult::setSessionKey(const Key &newSessionKey)
{
    m_sessionKey = newSessionKey;
}

void HandshakeResult::setErrorMessage(const std::string &newErrorMessage)
{
    m_errorMessage = newErrorMessage;
}

void HandshakeResult::setIpAddress(const std::string &newIpAddress)
{
    m_ipAddress = newIpAddress;
}
