#pragma once

#include <cassert>

#include "../src/utils/lwsdebug.h"

#define VAR_NAME(var) (#var)

#define ASSERT_AND_LOG(var) \
assert(var); \
    lwsdebug(prefix) << VAR_NAME(var) << " PASSED!";

void testRandomGenerator();
void testXTEA256();
void testPackets();
void testHandshake();
void testEventHandlers();
void testKeepAlive();
void testTCPCommunication();


