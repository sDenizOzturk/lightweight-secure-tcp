---
title: Setting Server
sidebar_position: 2
---

# Setting Up a Server

## ⚙️ Basic Example

To create a server, initialize `LightweightSecureServer` with a port number and call `start()`.

```cpp
#include "lightweightsecuretcp.h"

const int PORT = 44444;
LightweightSecureServer server(PORT);

server.start();
```

---

You must call `LightweightSecureTCP::setHandshakeKey(...)` **before** starting the server.
Otherwise, the process will terminate with an error.

---

## 🤝 After Handshake

Once the handshake is successful, the server receives a `Session` object:

```cpp
server.setOnHandshakeSuccess([](HandshakeResult result, Session& session) {
    // Send a message to the client
    session.sendMessage("Hello");

    // Listen for incoming messages
    session.setOnMessageReceived([](const std::string& message) {
        std::cout << "[Server] Received: " << message << std::endl;
    });

    // Optionally disconnect
    session.disconnect();
});
```

This `Session` object is valid until the client disconnects or keep-alive times out.

---

## ❌ Error Handling

```cpp
server.setOnError([](const std::string& error) {
    std::cout << "[Server] Error: " << error << std::endl;
});
```

---

## 🛑 Stopping the Server

To stop accepting connections and close the socket:

```cpp
server.stop();
```

---

## ✅ Notes

- You can configure keep-alive before calling `start()` using `setKeepAliveConfig(...)`
- All messaging must go through the `Session` object returned in the handshake callback
- Once `session.disconnect()` is called, the connection is closed on both sides
