---
title: LightweightSecureTCP
sidebar_position: 1
---

# LightweightSecureTCP

This class provides global configuration and initialization functions for the LightweightSecureTCP library.  
It is intended to be used at application startup.

---

## 🔐 Static Methods

### static void setHandshakeKey(const Key& key)

Sets the global encryption key used in the handshake process.

```cpp
LightweightSecureTCP::setHandshakeKey(myKey);
```

Must be called **before any client or server starts**.
Internally delegates to XTEA256::setHandshakeKey.

---

### static void setNumberOfRounds(int numberOfRounds)

Sets how many encryption rounds are used during handshake encryption.

```cpp
LightweightSecureTCP::setNumberOfRounds(64);
```

Default is typically 32, but you can increase it for stronger encryption.
Internally delegates to XTEA256::setNumberOfRounds.

---

### static void enableDebug(bool enable)

Enables or disables all debug logging.

```cpp
LightweightSecureTCP::enableDebug(true);
```

This toggles the internal debug flag and affects whether lwsdebug(...) prints anything.

---

## 📝 Related

- [Key](./encryption-key) → used in setHandshakeKey
- [lwsdebug()](./utils-lwsdebug) → structured debug logger
