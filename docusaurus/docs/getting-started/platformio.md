---
title: PlatformIO (ESP32)
sidebar_position: 2
---

# Using LightweightSecureTCP in your PlatformIO project

---

## 🧩 Step 1 — Download the Repository

Clone (or download) the repository:

```bash
git clone https://github.com/sDenizOzturk/lightweight-secure-tcp.git
```

Copy `lib` folder to your project:

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
# ...other PlatformIO files
```

---

Unlike the CMake setup, **you don’t need to modify any CMakeLists.txt files.**  
PlatformIO will automatically detect and build the library.
