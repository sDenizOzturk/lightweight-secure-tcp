#include "test.h"
#include "../src/encryption/xtea256.h"
#include <cstdint>
#include <vector>

void testXTEA256() {
    const std::string prefix = "\033[34m~~~ [testXTEA256]";
    lwsdebug(prefix) << "Testing testXTEA256...";

    std::string plaintext = "This is a test message for XTEA 256 encryption.";
    lwsdebug(prefix) << "plaintext: " << plaintext;

    uint8_t paddingLength;

    std::vector<uint8_t> encrypted = XTEA256::encrypt(XTEA256::toRawValues(plaintext),&paddingLength);
    lwsdebug(prefix) << "encrypted text: " << XTEA256::toString(encrypted);

    std::vector<uint8_t> decrypted = XTEA256::decrypt(encrypted,paddingLength);
    lwsdebug(prefix) << "decrypted text: " << XTEA256::toString(decrypted);

    assert(plaintext == XTEA256::toString(decrypted));
    lwsdebug(prefix) << "Test PASSED!";
}
