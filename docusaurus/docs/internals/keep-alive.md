---
title: KeepAlive Mechanism
sidebar_position: 3
---

# KeepAlive Mechanism

The KeepAlive system ensures that an active session stays alive and responsive.  
It prevents unintentional timeouts due to inactivity by periodically sending small packets.

---

## 🔁 How It Works

- After a successful handshake, **both client and server** start sending KeepAlive packets
- Each KeepAlive is sent at a fixed interval (e.g. every 5 seconds)
- If **no message or KeepAlive** is received from the other side within the **inactivity timeout** window, the connection is closed

---

## 🧠 Configuration

You can configure both interval and timeout using `setKeepAliveConfig(...)`:

```cpp
server.setKeepAliveConfig(keepAliveIntervalMs, inactivityTimeoutMs);
client.setKeepAliveConfig(keepAliveIntervalMs, inactivityTimeoutMs);
```

- `keepAliveIntervalMs`: how often to send a KeepAlive (must be < timeout)
- `inactivityTimeoutMs`: max allowed silence before disconnection
  Use `-1` to disable the timeout check completely

---

## 📦 What Is Sent?

A KeepAlive packet contains **only 1 byte**:

- `TYPE`: 0x05
- It is **not encrypted**

These are small and lightweight, they do not carry any actual message content.

---

## 🚨 What If It Fails?

If the other side becomes unresponsive:

- Session is closed automatically
- `onSessionEnded()` is triggered on the receiver side
- `onDisconnected()` is triggered on the client

This helps detect network issues or crashed peers without needing manual pings.
