---
title: lwsdebug
sidebar_position: 6
---

# lwsdebug

A lightweight logging utility for structured and prefixed debug output.  
Enabled or disabled globally via `LightweightSecureTCP::enableDebug(true)`.

---

### lwsdebug

**Return type:** `LWSDebugLine`

**Parameters:**

- `const std::string& prefix` _(optional)_

Creates a temporary debug line object.  
Any output is printed only if debugging is enabled.

---

## Usage

```cpp
lwsdebug("[Server]") << "Started on port " << port << std::endl;
```

or

```cpp
const std::string PREFIX = "[XTEA]";
lwsdebug(PREFIX) << "Encrypted: " << encrypted;
```
