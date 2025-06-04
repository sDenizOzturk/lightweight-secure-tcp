---
sidebar_position: 1
---

# Introduction

# LightweightSecureTCP

**LightweightSecureTCP** is a modern, lightweight, and secure TCP messaging library written in C++.  
It provides a simple protocol for reliable, encrypted communication between embedded systems and desktop applications.

> Requires a C++17-compatible compiler and standard library

---

## Why LightweightSecureTCP?

Standard TCP does not offer encryption, authentication, or structured messaging.  
Libraries like TLS or mbedTLS can be too heavy for constrained environments such as microcontrollers.

LightweightSecureTCP fills this gap by offering:

- A lightweight protocol with **secure handshake** and **message validation**
- Cross-platform support: **ESP32**, **Linux/macOS/Windows**, **Qt**
- Easy-to-integrate C++ API with no external dependencies

---

## Key Features

- 🔐 **Secure Handshake**: Ensures identity verification before message transfer
- 💡 **Lightweight Protocol**: Suitable for embedded and desktop environments
- 🔁 **KeepAlive & Timeout Detection**: Ensures connection liveness
- ✉️ **Structured Messaging**: All communication uses validated packets
- 🔌 **Multi-platform Ready**: Works with CMake, PlatformIO, and Qt projects

---

## Example Use Cases

- Secure messaging between an ESP32 and a desktop app
- Localhost testing of socket-based applications
- Qt-based GUI apps with secure networking needs
- Lightweight alternative to TLS sockets for embedded firmware

---

Let’s get started with the [Getting Started](getting-started/cmake) guide →
