#include "test.h"
#include "../src/utils/randomgenerator.h"
#include <cassert>
#include <iomanip>
#include <unordered_set>

void testRandomGenerator() {
    const std::string prefix = "\033[32m~~~ [testRandomGenerator]";
    lwsdebug(prefix) << "Running tests...";

    // Test: Random Byte (Diversity Test)
    std::unordered_set<uint8_t> uniqueBytes;
    for (int i = 0; i < 1000; i++) {
        uniqueBytes.insert(RandomGenerator::randomByte());
    }

    lwsdebug(prefix) << "First unique byte: " << static_cast<int>(*uniqueBytes.begin());
    lwsdebug(prefix) << "uniqueBytes size: " << uniqueBytes.size();
    ASSERT_AND_LOG(uniqueBytes.size() > 150);

    // Test: Random Uint32 (Uniqueness)
    uint32_t uint32_a = RandomGenerator::randomUint32();
    uint32_t uint32_b = RandomGenerator::randomUint32();
    lwsdebug(prefix) << "uint32_a: " << uint32_a;
    lwsdebug(prefix) << "uint32_b: " << uint32_b;
    ASSERT_AND_LOG(uint32_a != uint32_b);

    // Test: Random Uint64 (Uniqueness)
    uint64_t uint64_a = RandomGenerator::randomUint64();
    uint64_t uint64_b = RandomGenerator::randomUint64();
    lwsdebug(prefix) << "uint64_a: " << uint64_a;
    lwsdebug(prefix) << "uint64_b: " << uint64_b;
    ASSERT_AND_LOG(uint64_a != uint64_b);

    // Test: Random Key (8x uint32_t)
    std::array<uint32_t, 8> key1 = RandomGenerator::randomKey();
    std::array<uint32_t, 8> key2 = RandomGenerator::randomKey();
    lwsdebug(prefix) << "Generated random key 1:";
    for (uint32_t val : key1) {
        lwsdebug(prefix) << "  " << std::hex << std::setw(8) << std::setfill('0') << val;
    }

    lwsdebug(prefix) << "Generated random key 2:";
    for (uint32_t val : key2) {
        lwsdebug(prefix) << "  " << std::hex << std::setw(8) << std::setfill('0') << val;
    }
    ASSERT_AND_LOG(key1 != key2);

    lwsdebug(prefix) << "Test PASSED!";
}

