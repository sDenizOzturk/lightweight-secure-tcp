---
title: CMake (Desktop)
sidebar_position: 1
---

# Using LightweightSecureTCP in your CMake project

This guide explains how to integrate **LightweightSecureTCP** into your own CMake-based desktop application.

---

## 🧩 Step 1 — Download the Repository

Clone (or download) the repository:

```bash
git clone https://github.com/sDenizOzturk/lightweight-secure-tcp.git
```

Copy lib folder to your project:

```bash
cp -r lightweight-secure-tcp/lib/LightweightSecureTCP/ your-project/lib/
```

---

## 📁 Step 2 — Add the Folder to Your Project

Place the folder like this:

```
your-project/
├── lib/
│   └── LightweightSecureTCP/
│   └── # ...other libraries
# ...other project files & folders
```

---

## ⚙️ Step 3 — Modify `CMakeLists.txt`

Add the following lines to your `CMakeLists.txt`:

```cmake
add_subdirectory(lib/LightweightSecureTCP)

target_link_libraries(your_target PRIVATE LightweightSecureTCP)
```

> Replace `your_target` with the name of your executable or application target.

---

## ✅ That’s All

LightweightSecureTCP is now part of your project.  
No additional setup or external dependencies are required.

---

For embedded usage, see the [PlatformIO setup](./platformio).
