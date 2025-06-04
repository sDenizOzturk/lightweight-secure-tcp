#include "test.h"
#include "../src/protocol/Packet/regularmessagePacket.h"
#include "../src/protocol/Packet/handshakerequestPacket.h"
#include "../src/protocol/Packet/handshakeresponsePacket.h"
#include "../src/protocol/Packet/handshakesuccessPacket.h"
#include "../src/protocol/Packet/disconnectPacket.h"
#include <cassert>
#include <vector>

void testRegularMessagePacket() {
    const std::string prefix = "\033[33m~~~ [testPackets][testRegularMessagePacket]";
    std::array<uint32_t, 8> sessionKey = { 0x11223344, 0x55667788, 0x99AABBCC, 0xDDEEFF00,
                                          0x12345678, 0x9ABCDEF0, 0x13572468, 0xACEBDF01 };
    // Normal Case: Standard Message**
    std::string testMessage = "Hello, Packet System!";
    RegularMessagePacket originalPacket(testMessage, sessionKey);
    std::vector<uint8_t> serializedData = originalPacket.serialize();
    RegularMessagePacket deserializedPacket(sessionKey);
    deserializedPacket.deserialize(serializedData);

    assert(deserializedPacket.isValid() && "Deserialized Packet should be valid!");
    assert(deserializedPacket.getLength() == testMessage.size() && "Message length mismatch!");
    assert(deserializedPacket.getMessage() == testMessage && "Message content mismatch!");

    lwsdebug(prefix) << "Standard case passed!";

    // Edge Case: Empty Message**
    std::string emptyMessage = "";
    RegularMessagePacket emptyPacket(emptyMessage, sessionKey);
    std::vector<uint8_t> emptySerialized = emptyPacket.serialize();
    RegularMessagePacket emptyDeserialized(sessionKey);
    emptyDeserialized.deserialize(emptySerialized);

    lwsdebug(prefix) << emptyDeserialized.getLength();

    assert(!emptyDeserialized.isValid() && "Empty message Packet should not be valid!");
    assert(emptyDeserialized.getLength() == 0 && "Empty message length should be 0!");
    assert(emptyDeserialized.getMessage().empty() && "Empty message should be empty!");

    lwsdebug(prefix) << "Empty message case passed!";

    // Edge Case: Maximum Length Message**
    std::string maxMessage(65528, 'A');  // Maximum 16-bit unsigned value
    RegularMessagePacket maxPacket(maxMessage, sessionKey);
    std::vector<uint8_t> maxSerialized = maxPacket.serialize();
    RegularMessagePacket maxDeserialized(sessionKey);
    maxDeserialized.deserialize(maxSerialized);

    assert(maxDeserialized.isValid() && "Max-length Packet should be valid!");
    assert(maxDeserialized.getLength() == maxMessage.size() && "Max message length mismatch!");
    assert(maxDeserialized.getMessage() == maxMessage && "Max message content mismatch!");

    lwsdebug(prefix) << "Maximum length message case passed!";

    // Edge Case: Exceeding Maximum Length (65529 bytes)
    std::string tooLongMessage(65529, 'B');
    RegularMessagePacket tooLongPacket(tooLongMessage, sessionKey);
    std::vector<uint8_t> tooLongSerialized = tooLongPacket.serialize();
    RegularMessagePacket tooLongDeserialized(sessionKey);
    tooLongDeserialized.deserialize(tooLongSerialized);

    // Ensure the Packet is invalid due to exceeding length
    assert(!tooLongDeserialized.isValid() && "Packet with 65529 bytes should be invalid!");

    lwsdebug(prefix) << "Exceeding maximum length message case correctly marked as invalid!";


    // Edge Case: Non-ASCII Characters**
    std::string unicodeMessage = "こんにちは、世界！🌍";  // "Hello, World!" in Japanese with emoji
    RegularMessagePacket unicodePacket(unicodeMessage, sessionKey);
    std::vector<uint8_t> unicodeSerialized = unicodePacket.serialize();
    RegularMessagePacket unicodeDeserialized(sessionKey);
    unicodeDeserialized.deserialize(unicodeSerialized);

    assert(unicodeDeserialized.isValid() && "Unicode Packet should be valid!");
    assert(unicodeDeserialized.getLength() == unicodeMessage.size() && "Unicode message length mismatch!");
    assert(unicodeDeserialized.getMessage() == unicodeMessage && "Unicode message content mismatch!");

    lwsdebug(prefix) << "Unicode message case passed!";

    // Corrupt Data: Invalid Header**
    std::vector<uint8_t> corruptData = serializedData;
    corruptData[0] = 0xFF; // Change header to an invalid value
    RegularMessagePacket corruptPacket(sessionKey);
    corruptPacket.deserialize(corruptData);
    assert(!corruptPacket.isValid() && "Corrupt Packet should be invalid!");

    lwsdebug(prefix) << "Corrupt header case passed!";

    // Corrupt Data: Truncated Packet (Partial Data)**
    std::vector<uint8_t> truncatedData = serializedData;
    truncatedData.pop_back(); // Remove last byte
    RegularMessagePacket truncatedPacket(sessionKey);
    truncatedPacket.deserialize(truncatedData);
    assert(!truncatedPacket.isValid() && "Truncated Packet should be invalid!");

    lwsdebug(prefix) << "Truncated Packet case passed!";

    lwsdebug(prefix) << "All RegularMessagePacket test cases passed successfully!";
}

// Test HandshakeRequestPacket (Nonce + 2 Random Numbers)
void testHandshakeRequestPacket() {
    const std::string prefix = "\033[33m~~~ [testPackets][testHandshakeRequestPacket]";

    uint32_t nonce = 12345;
    uint64_t num1 = 987654321, num2 = 123456789;

    HandshakeRequestPacket originalPacket(nonce, num1, num2);
    std::vector<uint8_t> serializedData = originalPacket.serialize();
    HandshakeRequestPacket deserializedPacket;
    deserializedPacket.deserialize(serializedData);

    assert(deserializedPacket.isValid() && "Deserialized HandshakeRequestPacket should be valid!");
    assert(deserializedPacket.getNonce() == nonce && "Nonce mismatch!");
    assert(deserializedPacket.getNum1() == num1 && "Num1 mismatch!");
    assert(deserializedPacket.getNum2() == num2 && "Num2 mismatch!");

    lwsdebug(prefix) << "HandshakeRequestPacket test passed!";
}

// Test HandshakeResponsePacket (Nonce + Sum)
void testHandshakeResponsePacket() {
    const std::string prefix = "\033[33m~~~ [testPackets][testHandshakeResponsePacket]";

    uint32_t nonce = 54321;
    uint64_t sum = 1111111111;
    uint64_t _xor = 2222222222;

    HandshakeResponsePacket originalPacket(nonce, sum, _xor);
    std::vector<uint8_t> serializedData = originalPacket.serialize();
    HandshakeResponsePacket deserializedPacket;
    deserializedPacket.deserialize(serializedData);

    assert(deserializedPacket.isValid() && "Deserialized HandshakeResponsePacket should be valid!");
    assert(deserializedPacket.getNonce() == nonce && "Nonce mismatch!");
    assert(deserializedPacket.getSum() == sum && "Sum mismatch!");

    lwsdebug(prefix) << "HandshakeResponsePacket test passed!";
}

// Test HandshakeSuccessPacket (No Data, Just Type)
void testHandshakeSuccessPacket() {
    const std::string prefix = "\033[33m~~~ [testPackets][testHandshakeSuccessPacket]";

    // Step 1: Create the original Packet
    HandshakeSuccessPacket originalPacket;
    std::vector<uint8_t> serializedData = originalPacket.serialize();

    // Step 2: Ensure the serialized data is not empty
    assert(!serializedData.empty() && "Serialized HandshakeSuccessPacket should not be empty!");

    // Step 3: Deserialize and check validity
    HandshakeSuccessPacket deserializedPacket;
    bool isDeserialized = deserializedPacket.deserialize(serializedData);

    assert(isDeserialized && "Deserialized HandshakeSuccessPacket should be valid!");

    lwsdebug(prefix) << "HandshakeSuccessPacket test passed!";
}

// Test DisconnectPacket (No Data, Just Type)
void testDisconnectPacket() {
    const std::string prefix = "\033[33m~~~ [testPackets][testDisconnectPacket]";

    DisconnectPacket originalPacket;
    std::vector<uint8_t> serializedData = originalPacket.serialize();
    DisconnectPacket deserializedPacket(serializedData);

    assert(deserializedPacket.isValid() && "Deserialized DisconnectPacket should be valid!");

    lwsdebug(prefix) << "DisconnectPacket test passed!";
}

void testPackets() {
    lwsdebug("[testPackets]") << "Testing Packets...";

    testRegularMessagePacket();
    testHandshakeRequestPacket();
    testHandshakeResponsePacket();
    testHandshakeSuccessPacket();
    testDisconnectPacket();

    lwsdebug("[testPackets]") << "Test PASSED!";
}
