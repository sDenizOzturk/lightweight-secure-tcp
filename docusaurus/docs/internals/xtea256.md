---
title: XTEA256 Encryption
sidebar_position: 5
---

# XTEA256 Encryption

## 🔐 Overview

XTEA256 is a lightweight symmetric encryption algorithm used in LightweightSecureTCP.  
It operates on **64-bit blocks** using a **256-bit key**, and is optimized for embedded systems.

---

## ⚙️ Key Properties

- Encrypts 64-bit blocks (2 × 32-bit unsigned integers)
- Uses a 256-bit symmetric key (8 × 32-bit integers)
- Performs 32 rounds of encryption by default (configurable)
- All 8 parts of the key are used in every round
- Pure C++ implementation, no external dependencies

---

## 🔄 Key Usage

- 🤝 **Handshake packets** are encrypted with a static shared key
- 🔑 **Session packets** are encrypted with a dynamically generated session key

You must call:

```cpp
LightweightSecureTCP::setHandshakeKey(myKey);
```

before creating any server or client.

---

## 🔁 Number of Rounds

By default, 32 rounds are performed, but you can configure this:

```cpp
LightweightSecureTCP::setNumberOfRounds(64);
```

Higher round counts increase encryption complexity but consume more CPU cycles.

---

## 🧠 Why XTEA256?

- Lightweight and fast (suitable for ESP32)
- Predictable block size (easy to integrate with TCP)
- Strong enough for session-level encryption in embedded contexts

---

Encryption is handled automatically by the library.
You only need to set the handshake key and, optionally, the number of rounds.
