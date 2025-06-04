---
title: Session
sidebar_position: 3
---

# Session

Represents a secure communication session between the server and a connected client.  
Created after a successful handshake, this class handles messaging, disconnection, and event callbacks. `onSessionEnded()` if it was set.

---

## Methods

### isActive

**Return type:** `bool`

Returns `true` if the session is still alive and not closed.

---

### sessionKey

**Return type:** `const Key&`

Returns the encryption key negotiated during the handshake.

---

### sendMessage

**Return type:** `void`

**Parameters:**

- `const std::string& message`

Encrypts and sends a message over the socket using the session key.  
Triggers `onDataSent()` if sending is successful.  
Triggers `onError()` if sending fails.

---

### disconnect

**Return type:** `void`

Sends a disconnect packet and shuts down the session.  
Also calls `onSessionEnded()`.

---

## Event Callback Setters

### setOnMessageReceived

**Return type:** `void`

**Parameters:**

- `std::function<void(const std::string&)> callback`

Called when a valid message is received.

---

### setOnSessionEnded

**Return type:** `void`

**Parameters:**

- `std::function<void()> callback`

Called when the session is closed (either locally or by the client).

---

### setOnError

**Return type:** `void`

**Parameters:**

- `std::function<void(const std::string& errorMessage)> callback`

Called when an error occurs during communication.

---

### setOnDataSent

**Return type:** `void`

**Parameters:**

- `std::function<void(const std::string&)> callback`

Called after a message has been successfully sent.

---

## Notes

- The session automatically terminates if the connection is lost, the remote disconnects, or keep-alive checks fail.
- All callbacks are optional.
- Session state can be queried with `isActive()`.
