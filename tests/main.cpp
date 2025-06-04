#include "test.h"
#include "../src/utils/lwsdebug.h"
#include <cstdint>
#include "../src/encryption/xtea256.h"

#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <exception>
#include <iostream>
#include "../include/lightweightsecuretcp.h"

#define ENABLE_TERMINATE_HANDLER false

#if ENABLE_TERMINATE_HANDLER
// Print a backtrace to stderr
static void print_backtrace() {
    void* trace[32];
    int n = backtrace(trace, 32);
    backtrace_symbols_fd(trace, n, STDERR_FILENO);
}

// Handler for signals like SIGSEGV or SIGABRT
static void crashHandler(int sig) {
    std::cerr << "\n=== Caught fatal signal: " << sig << " ===\n";
    print_backtrace();
    _exit(1); // Use _exit to avoid cleanup, which may crash again
}

// Handler for uncaught C++ exceptions
static void terminateHandler() {
    std::cerr << "\n=== Uncaught exception (std::terminate called) ===\n";
    print_backtrace();
    _exit(1);
}
#endif


int main(int argc, char** argv) {
#if ENABLE_TERMINATE_HANDLER
    // Install signal handlers
    signal(SIGSEGV, crashHandler);
    signal(SIGABRT, crashHandler);

    // Install terminate handler for uncaught exceptions
    std::set_terminate(terminateHandler);
#endif
    signal(SIGPIPE, SIG_IGN); // Ignore SIGPIPE to avoid send() crashes

    // Optional: force global debug prints
    LightweightSecureTCP::enableDebug(true);

    // Setting static key for initial XTEA use
    Key handhakeKey({
        0x12345678, 0x9ABCDEF0, 0x13572468, 0xACEBDF01,
        0x11223344, 0x55667788, 0x99AABBCC, 0xDDEEFF00
    });
    LightweightSecureTCP::setHandshakeKey(handhakeKey);

    // Run test suites
    testRandomGenerator();
    testXTEA256();
    testPackets();
    testHandshake();
    testEventHandlers();
    testKeepAlive();
    testTCPCommunication();

    lwsdebug("") << "✅";
    lwsdebug("\033[35m") << "ALL";
    lwsdebug("\033[34m") << "TESTS";
    lwsdebug("\033[33m") << "PASSED";
    lwsdebug("\033[32m") << "SUCCESSFULLY!";
    lwsdebug("") << "✅";

    return 0;
}
