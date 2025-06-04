# FizzBuzz – ESP32 ↔ Desktop Example

This example demonstrates secure bidirectional communication between an ESP32 device and a desktop server using the **LightweightSecureTCP** library.

- ESP32 acts as the **client**
- Desktop application acts as the **server**
- Messages follow the familiar **FizzBuzz** sequence
- Communication is encrypted and structured using the library’s handshake protocol

---

## ⚙️ Requirements

### ESP32 Side
- ESP32 board (tested with `esp32dev`)
- [PlatformIO](https://platformio.org/)
- Wi-Fi access point
- Wi-Fi credentials configured in `.env.h`:

```cpp
#define WIFI_SSID        "YourSSID"
#define WIFI_PASSWORD    "YourPassword"
#define DESKTOP_SERVER_IP "192.168.X.X"
```

> The IP address must point to your desktop machine on the same local network.

---

### Desktop Side
- CMake 3.16 or later  
- A C++17-compatible compiler  
- LightweightSecureTCP source included via `add_subdirectory(...)`

---

## 🚀 Getting Started

### 1. Start the Desktop Server

```bash
mkdir build && cd build
cmake ..
make
./DesktopServer
```

Expected output:
```
[SERVER] Listening on port 12345
[SERVER] New connection from 192.168.X.X
[SERVER] Handshake complete
```

---

### 2. Upload the ESP32 Client

```bash
pio run -t upload
pio device monitor
```

Expected serial output:
```
[CLIENT] Connected to Wi-Fi.
[CLIENT] Handshake complete.
[CLIENT] Sending: 1
[CLIENT] Received: 2    (:
...
```

---

## 🧠 How It Works

- The ESP32 connects to the desktop server via TCP
- A symmetric key handshake is performed
- Messages are exchanged using FizzBuzz logic
- Keep-alive packets are sent periodically
- If no message or keep-alive is received within the timeout period, the connection is closed automatically

---

## 🔧 Customization

- Adjust delays using `vTaskDelay(pdMS_TO_TICKS(...))`
- Modify the handshake key with `LightweightSecureTCP::setHandshakeKey(...)`
- Update the FizzBuzz logic in the relevant task source file

---

## 📦 Technologies Used

- ESP-IDF (via PlatformIO)  
- CMake + C++17  
- LightweightSecureTCP (custom secure messaging library over TCP)
