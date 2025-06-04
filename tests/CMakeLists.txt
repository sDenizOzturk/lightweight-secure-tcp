# tests/CMakeLists.txt
add_executable(test_runner
    main.cpp
    test.h
    test_xtea256.cpp
    test_tcpcommunication.cpp
    test_keepalive.cpp
    test_eventhandlers.cpp
    test_handshake.cpp
    test_Packets.cpp
    test_randomgenerator.cpp
)

target_link_libraries(test_runner PRIVATE LightweightSecureTCP )

set_target_properties(test_runner PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
)
