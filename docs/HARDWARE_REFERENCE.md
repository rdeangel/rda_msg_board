# Hardware Resources & Limitations

## Platform Comparison: ESP8266 vs ESP32

This document tracks hardware resources, current usage, and limitations for both supported platforms.

---

## Flash Memory

### ESP8266 (D1 Mini / NodeMCU)
- **Total Flash:** 4MB (typical)
- **App Partition:** ~1MB (default scheme)
- **Current Usage:** ~300KB (30%)
- **Available:** ~700KB
- **Status:** 🟢 **Excellent headroom** - can add substantial features

### ESP32 (DevKit v1)
- **Total Flash:** 4MB (typical)
- **App Partition:** 1.31MB (default scheme)
- **Current Usage:** 1.23MB (93.8%)
- **Available:** 81KB
- **Status:** 🟡 **Tight but functional** - limited room for large libraries

**Comparison:** ESP8266 has **8.6x more flash headroom** (700KB vs 81KB)

---

## RAM (Static Allocation)

### ESP8266
- **Total RAM:** 81,920 bytes (~80KB)
- **Static Used:** 57,352 bytes (70%)
- **Available:** 24,568 bytes (~24KB)
- **Status:** 🟡 **Moderate** - careful memory management required

### ESP32
- **Total RAM:** 327,680 bytes (~320KB)
- **Static Used:** 67,900 bytes (20.7%)
- **Available:** 259,780 bytes (~260KB)
- **Status:** 🟢 **Excellent** - plenty of room for variables and buffers

**Comparison:** ESP32 has **10.6x more RAM headroom** (260KB vs 24KB)

---

## Heap Memory (Runtime Dynamic Allocation)

### ESP8266
- **Typical Free Heap:** 30-40KB
- **Notes:** Limited - avoid large dynamic allocations
- **Best Practices:**
  - Use stack allocation when possible
  - Avoid String class for large strings
  - Free memory promptly after use

### ESP32
- **Typical Free Heap:** 200-250KB
- **Notes:** Generous - can handle large dynamic allocations
- **Best Practices:**
  - Still monitor heap fragmentation
  - Use PSRAM for very large buffers (if available)

**Comparison:** ESP32 has **6.3x more heap** (220KB vs 35KB)

---

## CPU & Performance

| Feature | ESP8266 | ESP32 |
|---------|---------|-------|
| **CPU Speed** | 80/160 MHz | 240 MHz |
| **Cores** | 1 (single core) | 2 (dual core) |
| **Architecture** | Xtensa L106 (32-bit) | Xtensa LX6 (32-bit) |
| **FPU** | ❌ No | ✅ Yes |
| **Performance** | Baseline | ~2-3x faster |

---

## Connectivity

| Feature | ESP8266 | ESP32 |
|---------|---------|-------|
| **WiFi** | 802.11 b/g/n (2.4GHz) | 802.11 b/g/n (2.4GHz) |
| **Bluetooth** | ❌ No | ✅ BT 4.2 + BLE |
| **Ethernet** | ❌ No | ✅ Via PHY (optional) |
| **TLS/SSL** | ⚠️ Limited (BearSSL) | ✅ Full (mbedTLS) |

---

## Peripherals & I/O

| Feature | ESP8266 | ESP32 |
|---------|---------|-------|
| **GPIO Pins** | 11 usable | 34 usable |
| **ADC** | 1x 10-bit (1 channel) | 2x 12-bit (18 channels) |
| **DAC** | ❌ No | ✅ 2x 8-bit |
| **Touch Sensors** | ❌ No | ✅ 10 capacitive |
| **PWM Channels** | 4 (software) | 16 (hardware LEDC) |
| **Hardware SPI** | 1 | 4 (VSPI, HSPI, etc.) |
| **Hardware I2C** | 1 (software) | 2 (hardware) |
| **UART** | 1.5 (TX only on 2nd) | 3 full |
| **CAN Bus** | ❌ No | ✅ Yes (TWAI) |

---

## Storage Options

| Feature | ESP8266 | ESP32 |
|---------|---------|-------|
| **External Flash** | 512KB - 16MB | 4MB - 16MB |
| **PSRAM** | ❌ No | ✅ Optional (up to 8MB) |
| **SD Card** | ✅ Via SPI | ✅ Via SPI/SDIO |
| **Filesystem** | LittleFS, SPIFFS | LittleFS, SPIFFS, FAT |

---

## ESP32 Partition Schemes Explained

The ESP32's 4MB flash is divided into **partitions**. Different partition schemes allocate space differently.

### Default Partition (Current)
```
┌─────────────────────────────────────────────────────┐
│ Bootloader │ NVS │ OTA_0 │ OTA_1 │ SPIFFS │
│   (16KB)   │(20KB)│1.31MB │1.31MB │ 1.5MB  │
└─────────────────────────────────────────────────────┘
Total: 4MB
```

- **OTA_0:** Primary app partition (1.31MB) ← **Currently using 1.23MB (93.8%)**
- **OTA_1:** Secondary app partition for OTA updates (1.31MB)
- **SPIFFS:** File storage (1.5MB) - used for LittleFS
- **NVS:** Non-volatile storage for WiFi credentials, etc.

**OTA (Over-The-Air) Updates:**
- Firmware uploaded to OTA_1 while running from OTA_0
- After upload, device reboots and switches to OTA_1
- Allows wireless firmware updates without USB cable
- **Requires 2 app partitions** (doubles flash usage)

### Huge App Partition
```
┌─────────────────────────────────────────────────────┐
│ Bootloader │ NVS │   APP   │      SPIFFS      │
│   (16KB)   │(20KB)│  3MB    │      ~1MB        │
└─────────────────────────────────────────────────────┘
Total: 4MB
```

- **APP:** Single large app partition (3MB) ← **2.3x larger than default!**
- **SPIFFS:** Reduced file storage (~1MB)
- **No OTA_1:** Only one app partition

**What "No OTA" Means:**
- ❌ **Cannot do wireless firmware updates**
- ✅ **Must use USB cable** to upload new firmware
- ✅ **Gain 1.7MB more app space** (3MB vs 1.31MB)

**What We're "Borrowing" From:**
- **OTA_1 partition** (1.31MB) → Reclaimed for APP
- **SPIFFS space** (~500KB) → Reclaimed for APP
- **Total gain:** ~1.7MB additional app space

### Other Partition Options

| Partition | App Space | OTA | SPIFFS | Use Case |
|-----------|-----------|-----|--------|----------|
| **default** | 1.31MB | ✅ Yes | 1.5MB | Current - balanced |
| **minimal** | 1.9MB | ❌ No | 192KB | More app, less storage |
| **huge_app** | 3MB | ❌ No | ~1MB | Maximum app space |
| **min_spiffs** | 1.9MB | ✅ Yes | 192KB | OTA with more app space |

---

## Current Project Usage Summary

```
ESP8266 (D1 Mini):
├─ Flash: ~300KB / ~1MB (30%) → 700KB free
├─ RAM: 57KB / 82KB (70%) → 24KB free
├─ Heap: ~35KB typical
└─ Status: ✅ Plenty of room for features

ESP32 (DevKit):
├─ Flash: 1.23MB / 1.31MB (93.8%) → 81KB free
├─ RAM: 68KB / 328KB (20.7%) → 260KB free
├─ Heap: ~220KB typical
└─ Status: ✅ Functional but tight on flash
```

---

## Available Space for Additional Code

### ESP8266: ~700KB Flash Available
**Can Add:**
- ✅ Multiple new libraries (50-100KB each)
- ✅ Additional web pages
- ✅ New features (sensors, displays, protocols)
- ✅ OTA updates with room to spare
- ✅ Substantial feature expansion

**Limitations:**
- ⚠️ RAM is tight (24KB free) - watch dynamic allocations
- ⚠️ Heap is limited (35KB) - avoid large buffers

### ESP32: ~81KB Flash Available (Default Partition)
**Can Add:**
- ✅ Small features (~5-10KB each)
- ✅ HA TLS sensor (~2KB)
- ✅ Configuration additions
- ✅ Minor enhancements

**Cannot Add (Without Repartitioning):**
- ❌ Large libraries (HTTPS client, image processing, etc.)
- ❌ Major new features requiring >50KB

**Solution:** Use `huge_app.csv` partition for 3MB app space

---

## Optimization Strategies Applied

### ESP32 Size Optimizations (Current)
```ini
build_flags =
    -Os                    # Optimize for size (vs -O2 for speed)
    -ffunction-sections    # Separate functions for dead code elimination
    -fdata-sections        # Separate data for dead code elimination
    -Wl,--gc-sections      # Remove unused sections at link time
    -fno-exceptions        # Disable C++ exceptions (~20KB saved)
    -DCORE_DEBUG_LEVEL=0   # Disable core debug output
```

**Result:** Reduced from 102.7% (overflow) → 93.8% (fits with headroom)

---

## Recommendations

### For ESP8266
- 🟢 **Continue as-is** - excellent flash headroom
- 🟡 **Monitor RAM usage** - already at 70%
- ✅ Can add features without concern

### For ESP32
- 🟡 **Current state:** Functional with 81KB headroom
- **Option 1:** Keep default partition (safe for small additions)
- **Option 2:** Switch to `huge_app.csv` if planning major features
  ```ini
  # In platformio.ini [env:esp32dev]
  board_build.partitions = huge_app.csv  # 3MB app space, no OTA
  ```
- **Trade-off:** Lose wireless updates, gain 1.7MB app space

### When to Use Huge App Partition

**Use `huge_app.csv` if:**
- ✅ Adding large libraries (>100KB)
- ✅ Implementing complex features
- ✅ Don't need OTA updates (USB cable acceptable)
- ✅ Need maximum app space

**Keep default partition if:**
- ✅ Want OTA update capability
- ✅ Current 81KB headroom is sufficient
- ✅ Prefer wireless firmware updates

---

## Key Takeaway

**ESP8266:** Flash-comfortable, RAM-limited
- Best for: Simple applications with moderate features
- Strength: Lots of flash headroom (700KB)
- Weakness: Limited RAM (24KB free)

**ESP32:** RAM-rich, flash-constrained (default partition)
- Best for: Complex applications, TLS, multi-threading
- Strength: Abundant RAM (260KB free) and heap (220KB)
- Weakness: Tight flash with default partition (81KB free)
- Solution: Use `huge_app.csv` for 3MB app space (no OTA)

---

## Glossary: Hardware Terms Explained

### Core Components

#### **GPIO (General Purpose Input/Output)**
- **What it is:** Programmable pins that can be configured as inputs or outputs
- **Purpose:** Interface with external devices (LEDs, buttons, sensors, etc.)
- **Example:** Reading a button press, controlling an LED, triggering a relay
- **In this project:** Used for MAX7219 CS pin, buzzer control

#### **ADC (Analog-to-Digital Converter)**
- **What it is:** Converts analog voltage (0-3.3V) to digital values
- **Purpose:** Read analog sensors (temperature, light, potentiometers, etc.)
- **Resolution:** 
  - ESP8266: 10-bit (0-1023 values)
  - ESP32: 12-bit (0-4095 values)
- **Example:** Reading a light sensor to auto-adjust LED brightness
- **In this project:** Not currently used, but available for future sensors

#### **DAC (Digital-to-Analog Converter)**
- **What it is:** Converts digital values to analog voltage output
- **Purpose:** Generate analog signals (audio, control voltage, etc.)
- **Resolution:** ESP32 only, 8-bit (0-255 values → 0-3.3V)
- **Example:** Generating audio tones, controlling motor speed smoothly
- **In this project:** Not used (ESP8266 doesn't have DAC)

#### **PWM (Pulse Width Modulation)**
- **What it is:** Rapidly switches a pin on/off to simulate analog output
- **Purpose:** Control LED brightness, motor speed, servo position
- **How it works:** Varies the "duty cycle" (% of time pin is HIGH)
  - 0% duty = always off (0V)
  - 50% duty = half brightness
  - 100% duty = always on (3.3V)
- **Example:** Dimming an LED, controlling fan speed
- **In this project:** Could be used for LED brightness control (currently using MAX7219's built-in brightness)

### Communication Protocols

#### **SPI (Serial Peripheral Interface)**
- **What it is:** High-speed synchronous serial communication (master-slave)
- **Speed:** Very fast (up to 80MHz on ESP32)
- **Pins:** 4 wires (MOSI, MISO, CLK, CS)
  - **MOSI:** Master Out Slave In (data from ESP to device)
  - **MISO:** Master In Slave Out (data from device to ESP)
  - **CLK:** Clock signal (synchronization)
  - **CS:** Chip Select (choose which device to talk to)
- **Example:** SD cards, displays, MAX7219 LED drivers
- **In this project:** ✅ **Used for MAX7219 LED matrix control**

#### **I2C (Inter-Integrated Circuit)**
- **What it is:** Multi-device serial communication (master-slave)
- **Speed:** Slower than SPI (100kHz - 400kHz typical)
- **Pins:** 2 wires (SDA, SCL)
  - **SDA:** Serial Data (bidirectional)
  - **SCL:** Serial Clock (synchronization)
- **Advantage:** Multiple devices on same 2 wires (addressed by ID)
- **Example:** OLED displays, sensors (BME280, RTC modules)
- **In this project:** Not currently used, but available

#### **UART (Universal Asynchronous Receiver-Transmitter)**
- **What it is:** Asynchronous serial communication (point-to-point)
- **Speed:** Configurable (9600 - 921600 baud typical)
- **Pins:** 2 wires (TX, RX)
  - **TX:** Transmit (send data)
  - **RX:** Receive (receive data)
- **Example:** GPS modules, serial debugging, Bluetooth modules
- **In this project:** ✅ **Used for USB serial debugging (115200 baud)**

### Advanced Features

#### **Touch Sensors (Capacitive)**
- **What it is:** Detects touch without physical button press
- **How it works:** Measures capacitance change when finger approaches
- **Availability:** ESP32 only (10 touch-capable pins)
- **Example:** Touch buttons, proximity detection
- **In this project:** Not used, but could replace physical buttons

#### **FPU (Floating Point Unit)**
- **What it is:** Hardware accelerator for decimal math (float/double)
- **Purpose:** Fast calculations with decimal numbers
- **Availability:** ESP32 only
- **Benefit:** ~10x faster floating-point math vs software emulation
- **Example:** Complex calculations, graphics, signal processing

#### **CAN Bus (Controller Area Network)**
- **What it is:** Robust communication protocol for automotive/industrial use
- **Purpose:** Connect multiple devices in noisy environments
- **Availability:** ESP32 only (TWAI - Two-Wire Automotive Interface)
- **Example:** Car diagnostics (OBD-II), industrial automation
- **In this project:** Not applicable

#### **PSRAM (Pseudo-Static RAM)**
- **What it is:** External RAM chip connected to ESP32
- **Size:** Up to 8MB additional RAM
- **Purpose:** Store large buffers, images, data structures
- **Availability:** ESP32 only (optional, not on all boards)
- **Example:** Image processing, large data caching
- **In this project:** Not available on standard ESP32 DevKit

### Storage & Memory

#### **Flash Memory**
- **What it is:** Non-volatile storage (persists after power off)
- **Purpose:** Store firmware, web pages, configuration files
- **Speed:** Slower than RAM, but permanent
- **In this project:** ✅ **Stores firmware code, LittleFS files**

#### **RAM (Random Access Memory)**
- **What it is:** Volatile memory (lost when powered off)
- **Purpose:** Store variables, buffers, program execution
- **Speed:** Very fast access
- **Types:**
  - **Static RAM:** Global variables, allocated at compile time
  - **Heap:** Dynamic allocations (malloc, new, String class)
  - **Stack:** Function calls, local variables

#### **NVS (Non-Volatile Storage)**
- **What it is:** Key-value storage in flash
- **Purpose:** Store WiFi credentials, settings, calibration data
- **Availability:** Both platforms
- **In this project:** Used by WiFiManager for WiFi credentials

#### **SPIFFS / LittleFS**
- **What it is:** Filesystem for flash memory
- **Purpose:** Store files (configs, web pages, logs)
- **Difference:** LittleFS is newer, more reliable than SPIFFS
- **In this project:** ✅ **Used for config files and CA certificates**

### Connectivity

#### **OTA (Over-The-Air Updates)**
- **What it is:** Wireless firmware updates via WiFi
- **How it works:** Upload new firmware to secondary partition, reboot
- **Requirement:** Two app partitions (doubles flash usage)
- **Benefit:** Update devices without physical access
- **In this project:** ✅ **Supported with default partition**

#### **TLS/SSL (Transport Layer Security)**
- **What it is:** Encryption for secure network communication
- **Purpose:** Protect data from eavesdropping (HTTPS, MQTTS)
- **Availability:** 
  - ESP8266: Limited (BearSSL, resource-intensive)
  - ESP32: Full support (mbedTLS, hardware acceleration)
- **In this project:** ✅ **ESP32 MQTT TLS support implemented**

#### **BLE (Bluetooth Low Energy)**
- **What it is:** Low-power Bluetooth for IoT devices
- **Purpose:** Short-range wireless communication with phones/devices
- **Availability:** ESP32 only
- **Example:** Beacon advertising, sensor data transmission
- **In this project:** Not used

### Practical Examples

| Feature | Real-World Use Case |
|---------|---------------------|
| **GPIO** | Turn on/off a relay to control a lamp |
| **ADC** | Read soil moisture sensor (0-1023 = dry to wet) |
| **DAC** | Generate audio tone for a speaker |
| **PWM** | Dim an LED from 0-100% brightness |
| **SPI** | Control an LED matrix (this project!) |
| **I2C** | Read temperature from BME280 sensor |
| **UART** | Receive GPS coordinates from GPS module |
| **Touch** | Create a touch-sensitive control panel |
| **CAN** | Read car engine data (OBD-II) |

---

*Last Updated: 2026-01-07*
*Project: RDA Message Board*
*Platforms Tested: ESP8266 (nodemcuv2,D1 Mini), ESP32 (DevKit v1)*
