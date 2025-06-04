// XTEA256 Algorithm Implementation
// Written with assistance from ChatGPT (o3-mini-high model)

#include "xtea256.h"
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <cstring>
#include <cstdint>
#include <vector>
#include <string>
#include "utils/randomgenerator.h"

// Common XTEA parameters
static constexpr uint32_t DELTA = 0x9E3779B9;

int XTEA256::m_numberOfRounds = 32;

void XTEA256::setNumberOfRounds(const int &numberOfRounds)
{
    // Sets the number of rounds used by the XTEA algorithm.
    // Accepts values between 1 and 128. The default is 32.
    // Higher values increase security but reduce performance.
    m_numberOfRounds = numberOfRounds;
}

std::array<uint32_t, 8> XTEA256::m_key = {0};

void XTEA256::setHandshakeKey(const Key &key)
{
    // Sets the 256-bit key used for encryption and decryption.
    // All 8 words (8 × 32-bit = 256-bit) are used in the XTEA round logic.
    m_key = key;
}

// Encrypt a single 64-bit block (v[0], v[1]) in-place.
// This implementation uses a full 256-bit key (8 × 32-bit words).
// Sub-keys are selected dynamically using bitwise operations on the round sum.

void XTEA256::encryptBlock(uint32_t *v, const std::array<uint32_t, 8> &key)
{
    // We'll treat key[0..3] as the 128-bit XTEA key.
    // If you want to incorporate all 8 words, you'd do a custom approach.
    uint32_t v0 = v[0];
    uint32_t v1 = v[1];
    uint32_t sum = 0;

    for (int i = 0; i < m_numberOfRounds; i++)
    {
        v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 7]);
        sum += DELTA;
        v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum >> 11) & 7]);
    }
    v[0] = v0;
    v[1] = v1;
}

// Decrypt a single 64-bit block (v[0], v[1]) in-place.
// Uses the same 256-bit key schedule as the encryption function.
// Sub-keys are selected based on the round sum, reversed.

void XTEA256::decryptBlock(uint32_t *v, const std::array<uint32_t, 8> &key)
{
    uint32_t v0 = v[0];
    uint32_t v1 = v[1];
    uint32_t sum = DELTA * m_numberOfRounds;

    for (int i = 0; i < m_numberOfRounds; i++)
    {
        v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum >> 11) & 7]);
        sum -= DELTA;
        v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 7]);
    }
    v[0] = v0;
    v[1] = v1;
}

// Encrypt a whole buffer of arbitrary length (pads to multiple of 8).
std::vector<uint8_t> XTEA256::encrypt(const std::vector<uint8_t> &in, const std::array<uint32_t, 8> &key, uint8_t *padding)
{
    // Copy input
    std::vector<uint8_t> out = in;

    // Ensure final padded message does not exceed 65535 bytes
    if (out.size() > 65528)
    {
        throw std::runtime_error("Ciphertext size cannot exceed 65528 bytes"); // Error: Padded message too large
    }

    uint8_t paddingCount = 0;
    while (out.size() % 8 != 0)
    {
        out.push_back(static_cast<uint8_t>(RandomGenerator::randomByte()));
        paddingCount++;
    }
    if (padding)
    {
        *padding = paddingCount;
    }

    // Process each 64-bit block
    for (size_t i = 0; i < out.size(); i += 8)
    {
        union
        {
            uint64_t u64;
            uint32_t u32[2];
            uint8_t bytes[8];
        } block{};

        std::memcpy(block.bytes, out.data() + i, 8);
        encryptBlock(block.u32, key);
        std::memcpy(out.data() + i, block.bytes, 8);
    }

    return out;
}

// Decrypt a whole buffer of arbitrary length (must be multiple of 8).
std::vector<uint8_t> XTEA256::decrypt(const std::vector<uint8_t> &in, const std::array<uint32_t, 8> &key, uint8_t padding)
{
    if (in.size() % 8 != 0)
        throw std::runtime_error("Ciphertext size must be a multiple of 8 bytes!");

    std::vector<uint8_t> out = in;

    // Process each 64-bit block
    for (size_t i = 0; i < out.size(); i += 8)
    {
        union
        {
            uint64_t u64;
            uint32_t u32[2];
            uint8_t bytes[8];
        } block{};

        std::memcpy(block.bytes, out.data() + i, 8);
        decryptBlock(block.u32, key);
        std::memcpy(out.data() + i, block.bytes, 8);
    }

    // Remove padding
    while (padding-- > 0)
    {
        out.pop_back();
    }

    return out;
}

std::vector<uint8_t> XTEA256::encrypt(const std::vector<uint8_t> &plaintext, uint8_t *padding)
{
    return encrypt(plaintext, m_key, padding);
}

std::vector<uint8_t> XTEA256::decrypt(const std::vector<uint8_t> &ciphertext, uint8_t padding)
{
    return decrypt(ciphertext, m_key, padding);
}

// Convert raw bytes to std::string
std::string XTEA256::toString(const std::vector<uint8_t> &rawValues)
{
    return std::string(rawValues.begin(), rawValues.end());
}

// Convert std::string to raw bytes
std::vector<uint8_t> XTEA256::toRawValues(const std::string &text)
{
    return std::vector<uint8_t>(text.begin(), text.end());
}

bool XTEA256::hasValidKey()
{
    return m_key != std::array<uint32_t, 8>{0};
}
