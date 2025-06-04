# FizzBuzz - ESP32 Localhost Example

This example demonstrates a full loopback communication between a `LightweightSecureTCP` server and client on a single ESP32 device.

- Server and client run concurrently
- Messages are exchanged using the FizzBuzz logic
- Message format is dynamic and human-friendly (`"15 FizzBuzz"`, `"7  :)"`, etc.)

## ⚙️ Requirements

- ESP32 board (tested on `esp32dev`)
- [PlatformIO](https://platformio.org/)
- Wi-Fi access point (SSID/PASSWORD)

## 🚀 Getting Started

1. Copy `example.env.h` to `.env.h` and set your Wi-Fi credentials:

```cpp
#define WIFI_SSID "YourSSID"
#define WIFI_PASSWORD "YourPassword"
```

2. Make sure your board is correctly set in `platformio.ini`:
```ini
[env:esp32dev]
platform = espressif32
board = esp32dev        ; ← change this if you're using a different ESP32 variant
framework = espidf
```

3. Build and flash:

```bash
pio run -t upload
```

4. Open the serial monitor:

```bash
pio device monitor
```


5. You should see alternating FizzBuzz messages between server and client:

```
...
[CLIENT]: Handshake completed.
[SERVER]: Received: 1
[SERVER]: Sending: 2     (:
[CLIENT]: Received: 2     (:
[CLIENT]: Sending: 3 Fizz
[SERVER]: Received: 3 Fizz
[SERVER]: Sending: 4   (:
[CLIENT]: Received: 4   (:
[CLIENT]: Sending: 5 Buzz
[SERVER]: Received: 5 Buzz
[SERVER]: Sending: 6 Fizz
[CLIENT]: Received: 6 Fizz
[CLIENT]: Sending: 7      (:
[SERVER]: Received: 7      (:
[SERVER]: Sending: 8   (:
[CLIENT]: Received: 8   (:
[CLIENT]: Sending: 9 Fizz
[SERVER]: Received: 9 Fizz
[SERVER]: Sending: 10 Buzz
[CLIENT]: Received: 10 Buzz
[CLIENT]: Sending: 11      (:
[SERVER]: Received: 11      (:
[SERVER]: Sending: 12 Fizz
[CLIENT]: Received: 12 Fizz
[CLIENT]: Sending: 13    (:
[SERVER]: Received: 13    (:
[SERVER]: Sending: 14     (:
[CLIENT]: Received: 14     (:
[CLIENT]: Sending: 15 FizzBuzz
[SERVER]: Received: 15 FizzBuzz
[SERVER]: Sending: 16   (:
[CLIENT]: Received: 16   (:
...
```

## 🧠 How it works

- `app_main()` initializes Wi-Fi, sets up a server and a client
- Server listens on a loopback IP (ESP’s own IP)
- On handshake, both parties start exchanging FizzBuzz messages
- Logic is synchronized with `vTaskDelay()` and simple state tracking

## 🛠 Customization

- To change timing, adjust `vTaskDelay(pdMS_TO_TICKS(...))`
- To tweak output format, modify `fizzBuzz(int number)` function

## 📦 Based on

- LightweightSecureTCP (custom lightweight TCP messaging library)
- ESP-IDF via PlatformIO
