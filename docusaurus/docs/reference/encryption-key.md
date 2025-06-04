---
title: Key
sidebar_position: 5
---

# Key

Represents a 256-bit encryption key used during the handshake process.

---

### Key

**Parameters:**

- `const std::array<uint32_t, 8>& initKey`

Creates a key using a predefined 256-bit value.  
Use this when setting your own encryption key.

---

### Key

Creates a random encryption key.  
**Not recommended for production use.**  
Prefer explicitly defined keys to ensure deterministic behavior.  
Used internally by the library when needed (e.g. for default construction).

---

### toString

**Return type:** `std::string`

Returns a hexadecimal string representation of the key.  
Useful for debugging and logging purposes.
