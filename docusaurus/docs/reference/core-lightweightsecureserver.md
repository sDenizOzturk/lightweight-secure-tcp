---
title: LightweightSecureServer
sidebar_position: 2
---

# LightweightSecureServer

A secure TCP server class that accepts incoming connections, performs handshake, and triggers user-defined events via callbacks.

---

## Constructor

### LightweightSecureServer

**Return type:** `-` (constructor)

**Parameters:**

- `int port` — TCP port to listen on

Initializes the internal server socket and random generator. Terminates the program if a handshake key is not set.

---

## Destructor

### ~LightweightSecureServer

**Return type:** `void`

Automatically calls `stop()` to clean up the socket and threads.

---

## Configuration

### setKeepAliveConfig

**Return type:** `void`

**Parameters:**

- `int keepAliveIntervalMs`
- `int inactivityTimeoutMs`

Configures keep-alive frequency and inactivity timeout. Throws `std::invalid_argument` if values are invalid.

---

## Lifecycle

### start

**Return type:** `void`

Begins listening on the given port, accepts clients, and performs handshake in background threads.

---

### stop

**Return type:** `void`

Stops the server, disconnects the socket, and ends all active threads.

---

## Event Callback Setters

### setOnServerStarted

**Return type:** `void`

**Parameters:**

- `std::function<void()> callback`

Called when the server is successfully started and listening.

---

### setOnServerStopped

**Return type:** `void`

**Parameters:**

- `std::function<void()> callback`

Called when the server stops (either manually or on shutdown).

---

### setOnHandshakeStarted

**Return type:** `void`

**Parameters:**

- `std::function<void(int clientSocket, const std::string& peerAddress)> callback`

Called when a client connects, before the handshake begins.

---

### setOnHandshakeSuccess

**Return type:** `void`

**Parameters:**

- `std::function<void(const HandshakeResult& result, Session& session)> callback`

Called when a handshake completes successfully.  
You can interact with the session object in this callback.

---

### setOnHandshakeFailed

**Return type:** `void`

**Parameters:**

- `std::function<void(const HandshakeResult& result)> callback`

Called when a client fails to complete the handshake.

---

### setOnError

**Return type:** `void`

**Parameters:**

- `std::function<void(const std::string& errorMessage)> callback`

Called when socket creation, binding, or listening fails.

---

## Related

- [LightweightSecureTCP](./lightweightsecuretcp)
- [LightweightSecureClient](./core-lightweightsecureclient)
