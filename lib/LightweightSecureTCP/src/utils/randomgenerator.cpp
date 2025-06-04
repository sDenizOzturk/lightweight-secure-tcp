#include "randomgenerator.h"

#if defined(ESP32)
#include "esp_system.h" // ESP32 hardware RNG
#else
#include <random>
#include <mutex>
#include <atomic>
#endif

namespace
{
#if !defined(ESP32)
// We will seed std::mt19937 exactly once using std::random_device
static std::mt19937 g_mt; // Mersenne Twister
static std::once_flag g_initFlag;

// This function seeds g_mt with real entropy
void seedGenerator() {
    std::random_device rd;
    g_mt.seed(rd());
}
#endif
} // unnamed namespace

void RandomGenerator::initialize()
{
#if !defined(ESP32)
    // Seed the generator exactly once
    std::call_once(g_initFlag, seedGenerator);
#endif
}

uint8_t RandomGenerator::randomByte()
{
#if defined(ESP32)
    // ESP32 hardware RNG
    return static_cast<uint8_t>(esp_random() & 0xFF);
#else
    // Ensure it's seeded once
    initialize();
    static std::uniform_int_distribution<uint8_t> dist(0, 255);
    return dist(g_mt);
#endif
}

uint32_t RandomGenerator::randomUint32()
{
#if defined(ESP32)
    return esp_random();
#else
    initialize();
    static std::uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);
    return dist(g_mt);
#endif
}

uint64_t RandomGenerator::randomUint64()
{
#if defined(ESP32)
    // Combine two 32-bit values from esp_random()
    uint64_t high = static_cast<uint64_t>(esp_random());
    uint64_t low  = static_cast<uint64_t>(esp_random());
    return (high << 32) | low;
#else
    initialize();
    static std::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);
    return dist(g_mt);
#endif
}

std::array<uint32_t, 8> RandomGenerator::randomKey()
{
    initialize();
    std::array<uint32_t, 8> key;
    for (uint32_t &word : key) {
        word = randomUint32();
    }
    return key;
}
