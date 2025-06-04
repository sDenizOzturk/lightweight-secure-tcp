---
title: Setting Handshake Key
sidebar_position: 1
---

# Setting the Handshake Key

Before starting any server or client, you must define a 256-bit encryption key.  
This key is used during the handshake to establish a secure session.  
The **same key must be initialized on every device** that will participate in a handshake.

The `Key` constructor accepts exactly one parameter: a `std::array<uint32_t, 8>`.

---

## 🔐 Example

```cpp
const Key handshakeKey({
    0xA1B2C3D4, 0x12345678, 0xDEADBEEF, 0x0F0F0F0F,
    0xCAFEBABE, 0xFEEDFACE, 0x00000000, 0xFFFFFFFF
});
LightweightSecureTCP::setHandshakeKey(handshakeKey);
```
