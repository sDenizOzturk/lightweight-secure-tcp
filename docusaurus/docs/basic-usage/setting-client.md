---
title: Setting Client
sidebar_position: 3
---

# Setting Up a Client

## ⚙️ Basic Example

To connect to a server, initialize `LightweightSecureClient` with the server's IP address and port number, then call `connectToServer()`.

```cpp
#include "lightweightsecuretcp.h"

const std::string SERVER_IP = "127.0.0.1";
const int PORT = 44444;

LightweightSecureClient client(SERVER_IP, PORT);
client.connectToServer();
```

---

You must call `LightweightSecureTCP::setHandshakeKey(...)` **before** creating the client.
The key must exactly match the one used by the server.

---

## 📩 Receiving Messages

You can listen for incoming messages by setting the `onMessageReceived` callback:

```cpp
client.setOnMessageReceived([](const std::string& message) {
    std::cout << "[Client] Received: " << message << std::endl;
});
```

---

## ✉️ Sending Messages

Once connected, you can send messages to the server:

```cpp
client.sendMessage("Hello from client!");
```

---

## ❌ Error Handling

```cpp
client.setOnError([](const std::string& error) {
    std::cout << "[Client] Error: " << error << std::endl;
});
```

---

## 🔌 Disconnecting

To cleanly disconnect from the server:

```cpp
client.disconnect();
```

---

## ✅ Notes

- All messaging must happen **after a successful handshake**
- You can configure keep-alive before calling `connectToServer()` using `setKeepAliveConfig(...)`
- Use `isConnected()` to check if the client is still connected
