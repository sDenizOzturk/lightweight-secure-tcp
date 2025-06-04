---
title: Session Lifecycle
sidebar_position: 6
---

# Session Lifecycle

A `Session` represents the secure communication channel created on the **server side**  
after a successful handshake with a client.

It exists only while the connection is active and automatically handles encryption, timeouts, and clean disconnection.

---

## 🔄 Lifecycle Stages

1. **Created**

   - Initiated after handshake
   - A session key is negotiated
   - The server gains a `Session` object

2. **Active**

   - Messages can be securely exchanged
   - KeepAlive packets are sent periodically
   - Events such as message received or data sent are triggered

3. **Terminated**
   - Ends when:
     - The client disconnects
     - The server calls disconnect
     - A timeout or error occurs
   - After termination, the session is no longer valid

---

## 🚫 No Client Session

The client does **not** have a `Session` object.

Instead, the client class itself manages the connection lifecycle internally:

- It connects, performs handshake, exchanges messages, and disconnects
- From the client's perspective, the entire client instance **is** the session

---

## 🧠 Internals

- The session key is used to encrypt all messages after handshake
- The library ensures no packets are sent if the session ends
- You don’t need to manage raw sockets or timers manually

---

The session system provides a lightweight, event-driven structure for secure server-side communication.
