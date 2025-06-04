#pragma once

#include <array>
#include <cstdint>

class RandomGenerator {
public:
    // Initializes the random generator (on non-ESP32 platforms, it seeds once)
    static void initialize();

    // Returns a random byte (0..255)
    static uint8_t randomByte();

    // Returns a random 32-bit unsigned integer
    static uint32_t randomUint32();

    // Returns a random 64-bit unsigned integer
    static uint64_t randomUint64();

    // Returns a random 256-bit key (8 x uint32_t)
    static std::array<uint32_t, 8> randomKey();
};
