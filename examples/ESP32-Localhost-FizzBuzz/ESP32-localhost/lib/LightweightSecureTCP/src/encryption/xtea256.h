#pragma once

#include <array>
#include <string>
#include <vector>
#include <cstdint>
#include "key.h"

class XTEA256 {
public:
    static void setNumberOfRounds(const int &numberOfRounds);
    static void setHandshakeKey(const Key &key);

    static std::vector<uint8_t> encrypt(const std::vector<uint8_t>& plaintext, const std::array<uint32_t, 8> &key, uint8_t* padding = nullptr);
    static std::vector<uint8_t> decrypt(const std::vector<uint8_t>& ciphertext, const std::array<uint32_t, 8> &key, uint8_t padding = 0);

    static std::vector<uint8_t> encrypt(const std::vector<uint8_t>& plaintext, uint8_t* padding = nullptr);
    static std::vector<uint8_t> decrypt(const std::vector<uint8_t>& ciphertext, uint8_t padding = 0);

    static std::string toString(const std::vector<uint8_t>& rawValues);
    static std::vector<uint8_t> toRawValues(const std::string &text);

    static bool hasValidKey();
private:
    static std::array<uint32_t, 8> m_key;
    static int m_numberOfRounds;

    static void encryptBlock(uint32_t* v, const std::array<uint32_t, 8> &key);
    static void decryptBlock(uint32_t* v, const std::array<uint32_t, 8> &key);
};
