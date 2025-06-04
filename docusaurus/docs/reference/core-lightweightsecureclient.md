---
title: LightweightSecureClient
sidebar_position: 4
---

# LightweightSecureClient

A secure TCP client that connects to a remote server, performs handshake, sends messages, and handles asynchronous communication.

---

## Constructor

### LightweightSecureClient

**Return type:** — (constructor)

**Parameters:**

- `const std::string& serverIp` — IP address of the server to connect to
- `int port` — TCP port of the server

Initializes the socket and performs internal validation.  
Terminates the program with `exit()` if encryption key is not set.

---

## Destructor

### ~LightweightSecureClient

**Return type:** `void`

Disconnects the socket and triggers `onError("Client destroyed")` if defined.

---

## Methods

### setKeepAliveConfig

**Return type:** `void`

**Parameters:**

- `int keepAliveIntervalMs`
- `int inactivityTimeoutMs`

Sets the keep-alive and timeout configuration.  
Throws `std::invalid_argument` if values are invalid or inconsistent.

---

### connectToServer

**Return type:** `bool`

Attempts to connect to the server and perform handshake.  
Returns `true` on success, `false` otherwise.  
Triggers:

- `onConnected()` if successful
- `onError("Connection failed")` if not

---

### disconnect

**Return type:** `void`

Sends a disconnect packet if connected and then closes the socket.  
Triggers `onDisconnected()` if defined.

---

### sendMessage

**Return type:** `void`

**Parameters:**

- `const std::string& message`

Encrypts and sends a message to the server.  
Triggers `onDataSent()` if successful, `onError()` if not.

---

### isConnected

**Return type:** `bool`

Returns `true` if the socket is connected and active.

---

## Event Callback Setters

### setOnConnected

**Return type:** `void`  
**Parameters:** `std::function<void()> callback`

Called when the client successfully connects to the server.

---

### setOnDisconnected

**Return type:** `void`  
**Parameters:** `std::function<void()> callback`

Called when the connection is lost or disconnected manually.

---

### setOnMessageReceived

**Return type:** `void`  
**Parameters:** `std::function<void(const std::string&)> callback`

Called when a valid message is received from the server.

---

### setOnHandshakeSuccess

**Return type:** `void`  
**Parameters:** `std::function<void(const HandshakeResult&)> callback`

Called when the handshake with the server succeeds.

---

### setOnHandshakeFailed

**Return type:** `void`  
**Parameters:** `std::function<void(const HandshakeResult&)> callback`

Called when the handshake with the server fails.

---

### setOnError

**Return type:** `void`  
**Parameters:** `std::function<void(const std::string&)> callback`

Called when any error occurs during connection or communication.

---

### setOnDataSent

**Return type:** `void`  
**Parameters:** `std::function<void(const std::string&)> callback`

Called after a message is successfully sent to the server.

---

## Related

- [LightweightSecureServer](./core-lightweightsecureserver)
- [LightweightSecureTCP](./lightweightsecuretcp)
- [Session](./core-session)
