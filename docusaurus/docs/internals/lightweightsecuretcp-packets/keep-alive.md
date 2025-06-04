---
title: Keep Alive Packet
sidebar_position: 5
---

# Keep Alive Packet

![Only TYPE Field](./images/only-type.png)

---

## 📦 Size

1 byte

- `TYPE`: 8 bits (`0x05`)

---

## 🔁 Sent By

**Both client and server**, periodically during an active session.

---

## 🧪 Contents

- Contains only a single byte: the `TYPE` field
- It is **not encrypted**

---

## 🔄 Purpose

This packet is used to prevent idle disconnections.

If no packets (including keep-alives) are received within the **inactivity timeout**, the session will automatically close.

Used in both directions to confirm that the other side is still alive.
