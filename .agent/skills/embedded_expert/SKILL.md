---
name: Embedded Expert (ESP32/ESP8266)
description: Specialized knowledge for developing robust, non-blocking firmware for Espressif microcontrollers, focusing on hardware safety, memory efficiency, and network stability.
---

# Embedded Expert (ESP32/ESP8266)

This skill provides a comprehensive set of rules and best practices for developing firmware for the `rda_msg_board` project and general ESP-based applications. It ensures stability, prevents hardware damage, and optimizes performance for low-resource environments.

## 1. Critical Hardware Rules (The "Do Not Brick" List)

### Strapping Pins & Boot Modes
These pins are checked during the boot sequence to determine the startup mode. Using them incorrectly can prevent the board from starting or entering flash mode.

*   **ESP8266 Specific**:
    *   **GPIO 0**: Must be HIGH at boot for normal operation (LOW for Flash mode).
    *   **GPIO 2**: Must be HIGH at boot. Often connected to internal LED.
    *   **GPIO 15**: Must be LOW at boot.
*   **ESP32 Specific**:
    *   **GPIO 0, 2, 5, 12, 15**: These determine boot mode, voltage levels, and JTAG behavior. Avoid using them for external triggers or high-current loads without isolation.

### Input-Only Pins (ESP32)
*   **GPIO 34, 35, 36, 39** are **Input Only**. They do NOT have internal pull-up or pull-down resistors. Use external resistors if necessary.

### Power & Logic Levels
*   **3.3V Logic**: All I/O pins are 3.3V logic. **DO NOT** connect 5V sensors/signals directly. Use logic level shifters.
*   **Power Consumption**: LED Matrices (MAX7219) draw significant current. If the display glitches or the ESP reboots when WiFi connects, it is likely a **brownout**.
    *   *Solution*: Ensure the 5V rail for the matrix is adequately decoupled and the ESP has a stable 3.3V source. Software mitigation: limit max brightness or stagger initialization.

## 2. Coding Standards (Non-Blocking & Stability)

### Non-Blocking Logic
*   **Strictly forbid `delay()`**: In the main `loop()`, especially when WiFi or MQTT is active, `delay()` will cause network timeouts and WDT resets.
*   **Use `millis()`**: Implement state machines and timers using `millis()` (e.g., `if (millis() - lastTime > interval) { ... }`).
*   **Yielding**: For long-running loops (like complex message parsing), use `yield()` or `vTaskDelay(1)` to allow the background tasks (WiFi, TCP/IP stack) to run.

### Interrupt Handling (ISRs)
*   **`IRAM_ATTR`**: (ESP8266/ESP32) All ISR functions **MUST** be marked with `IRAM_ATTR` (e.g., `void IRAM_ATTR onTimer()`). This keeps the code in RAM, preventing crashes if an interrupt occurs while writing to Flash.
*   **Volatile Variables**: Variables shared between an ISR and the main loop must be declared `volatile`.
*   **Minimal Logic**: Keep ISRs extremely short (set a boolean flag, increment a counter). **NEVER** use `Serial.print()`, `delay()`, or complex operations inside an ISR.

### FreeRTOS & Multitasking (ESP32 Specific)
*   **Core Pinning**:
    *   **Core 0**: Reserved for WiFi and System tasks.
    *   **Core 1**: Best for Application logic (Arduino `loop()` runs here by default).
*   **Stack Sizing**: When creating tasks (`xTaskCreate`), estimate stack usage carefully. Start with 2048 or 4096 bytes and monitor `uxTaskGetStackHighWaterMark` if unsure.
*   **Priorities**: Ensure your tasks do not starve the Idle task (priority 0) or the Timer task. Yielding (`vTaskDelay`) is crucial.

## 3. Memory & Storage Management

### Flash Memory Health
*   **Wear Leveling**: Flash memory has limited write cycles (~10,000 to 100,000). **NEVER** write to LittleFS or EEPROM in a tight loop.
*   **Commit Strategy**: Use a "dirty flag" pattern. Only save configuration if a value has actually changed.
*   **Struct Alignment**: Accessing unaligned data can cause exceptions. When writing structs to Flash/LittleFS, ensure 4-byte alignment or use `__attribute__((packed))` if necessary (though packed structs can be slower).

### RAM Optimization
*   **ESP8266 Strings**: Use `F()` macro for static strings (e.g., `Serial.print(F("Booting..."))`) to save RAM.
*   **ESP32 PSRAM**: If available, use `ps_malloc` for large buffers (display framebuffers, large JSON docs).
*   **Fragmentation**: Avoid frequent allocation/deallocation of `String` objects. Use static `char[]` buffers where possible.

## 4. WiFi & Network Handling

### Robust Reconnection
*   `WiFi.begin()` can be blocking. Use non-blocking checks (`WiFi.status()`) or existing `WiFiManager` patterns.
*   **Power vs Responsiveness**:
    *   `WiFi.setSleep(false)`: Disables power saving. Increases power consumption (~80mA -> ~140mA) but drastically reduces latency for Web Server and ping times. Recommended for this message board if powered by mains.

### Secrets Setup
*   **NEVER** hardcode credentials. Use `config_manager` (LittleFS) or `secrets.h` (git-ignored).

## 5. Security & Input Validation

*   **Input Sanitization**: Always validate data coming from external sources (MQTT, Web).
    *   **MQTT**: Check payload length before copying to buffers.
    *   **Web**: Sanitize strings to prevent buffer overflows or HTML injection if displayed back to the user.
*   **Buffer Safety**: Use `snprintf` instead of `sprintf`, and `strlcpy` instead of `strcpy`.

## 6. Debugging Procedures

*   **Exception Decoding**:
    *   If the device crashes with a stack dump (`Backtrace: 0x400d1f...`), use the **Point Exception Decoder** filter in PlatformIO or the standalone tool to map addresses to lines of code.
*   **Reset Reasons**: Log `ESP.getResetReason()` at boot to understand if crashes are Watchdog, Brownout, or Exception based.
*   **Debug Macro**: Wrap all debug prints in `#if DEBUG ... #endif`.

## 7. Project-Specific Integration

*   **Four-Tier Config**: Respect `config.h` hierarchy (Web -> MQTT -> Defaults -> General). Update all 4 layers (Struct, Load/Save, Web XML, Web Handler) when adding settings.
*   **Display Buffer**: Check animation status before pushing new messages unless urgent.
