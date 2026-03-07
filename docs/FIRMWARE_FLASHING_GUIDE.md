# Firmware Flashing Guide

This guide explains how to upload precompiled firmware binaries to ESP8266 and ESP32 boards using various tools for Windows and Linux.

> **Looking to build from source?** See the [README](../README.md) for PlatformIO instructions.

---

## Quick Start (Recommended for Beginners)

If you're new to ESP devices, start with these user-friendly GUI tools:

**Windows:**
- **[ESPHome Flasher](https://github.com/esphome/esphome-flasher/releases)** - Simple, no installation required
- **[NodeMCU PyFlasher](https://github.com/marcelstoer/nodemcu-pyflasher/releases)** - Self-contained, works for ESP8266
- **[ESP Flash Download Tool](https://www.espressif.com/en/support/download/other-tools)** - Official Espressif tool

**Linux:**
- **[esptool.py](https://github.com/espressif/esptool)** - Universal command-line tool (pip install)
- **[NodeMCU PyFlasher](https://github.com/marcelstoer/nodemcu-pyflasher/releases)** - Linux binaries available
- **[ESP Tool (Web-based)](https://espressif.github.io/esptool-js/)** - Works in browser (Chrome/Firefox)

---

## Windows Tools

### 1. ESPHome Flasher (Easiest GUI)

**Best for:** Beginners, one-click flashing

**Download:** https://github.com/esphome/esphome-flasher/releases

**Steps:**
1. Download the latest `.exe` from releases
2. Connect your ESP board via USB
3. Run `ESPHome-Flasher-Setup-x.x.x.exe`
4. Select your port (COM3, COM4, etc.)
5. Click "Browse" and select the `.bin` firmware file
6. Click "Flash ESP" to upload

**Pros:** No installation required, auto-detects port, very simple
**Cons:** Windows only

---

### 2. NodeMCU PyFlasher (ESP8266 Focus)

**Best for:** ESP8266 boards, standalone tool

**Download:** https://github.com/marcelstoer/nodemcu-pyflasher/releases

**Steps:**
1. Download `nodemcu-pyflasher-x.x.x.exe` or `nodemcu-pyflasher-x.x.x-win64.exe`
2. No installation required - just double-click to run
3. Select your COM port
4. Click "..." to browse for firmware `.bin` file
5. Click "Flash NodeMCU" to upload

**Pros:** Self-contained (includes Python), ESP8266 optimized
**Cons:** Windows/Mac only, ESP8266-focused

---

### 3. ESP Flash Download Tool (Official Espressif)

**Best for:** Advanced users, multiple files, debugging

**Download:** https://www.espressif.com/en/support/download/other-tools

**Steps:**
1. Download `flash_download_tools_vX.X.X.exe`
2. Extract and run the application
3. Click "Developer Mode" button
4. Choose ESP8266 DownloadTool or ESP32 DownloadTool
5. Add your `.bin` file(s) and set address (usually `0x0` or `0x1000`)
6. Select COM port and baud rate (115200 recommended)
7. Click "START" to flash

**Address Guide:**
- **ESP8266:** `0x0` (most firmware images)
- **ESP32:** `0x1000` (typical for ESP32 firmware)

**Pros:** Official tool, supports multiple files, advanced options
**Cons:** Complex interface, more options than needed for simple flashing

---

### 4. Thonny IDE (Multi-Platform)

**Best for:** MicroPython firmware, or if you want an IDE

**Download:** https://thonny.org/

**Steps:**
1. Install Thonny IDE
2. Connect your ESP board via USB
3. Open Thonny, go to **Tools → Options → Interpreter**
4. Select "MicroPython (ESP8266)" or "MicroPython (ESP32)"
5. Choose correct port (COM port on Windows)
6. Click "Install or update MicroPython"
7. Select your firmware `.bin` file
8. Click "Install" to flash

**Pros:** Works on Windows/Linux/Mac, simple UI, includes IDE
**Cons:** Primarily for MicroPython (can still flash other firmware)

---

## Linux Tools

### 1. esptool.py (Universal Command-Line)

**Best for:** All platforms, scripting, automation

**Install:**
```bash
# Using pip (recommended)
pip install esptool

# Using apt (Debian/Ubuntu)
sudo apt-get install esptool
```

**Find your serial port:**
```bash
# List serial devices
ls /dev/ttyUSB* /dev/ttyACM*
# You'll see something like: /dev/ttyUSB0
```

**Flash ESP8266:**
```bash
# Erase flash (optional, recommended for clean install)
esptool.py --port /dev/ttyUSB0 --chip esp8266 erase_flash

# Flash firmware (replace with your file)
esptool.py --port /dev/ttyUSB0 --chip esp8266 write_flash --flash_mode dio --flash_size detect 0x0 firmware.bin
```

**Flash ESP32:**
```bash
# Erase flash (optional)
esptool.py --port /dev/ttyUSB0 --chip esp32 erase_flash

# Flash firmware (typical address: 0x1000)
esptool.py --port /dev/ttyUSB0 --chip esp32 write_flash -z 0x1000 firmware.bin
```

**Flash with multiple files (ESP32 with bootloader):**
```bash
esptool.py --port /dev/ttyUSB0 --chip esp32 write_flash \
  0x1000 bootloader.bin \
  0x8000 partitions.bin \
  0xe000 boot_app0.bin \
  0x10000 firmware.bin
```

**Pros:** Universal, works on all platforms, can be scripted, actively maintained
**Cons:** Command-line only, requires Python

---

### 2. ESP Tool (Web-Based)

**Best for:** No installation, quick one-off flashes

**Access:** https://espressif.github.io/esptool-js/

**Requirements:**
- Chrome or Firefox browser (Safari not supported)
- Web Serial API support

**Steps:**
1. Open the link in Chrome/Firefox
2. Connect your ESP board via USB
3. Click "Connect" and select your port
4. Choose baud rate (115200 recommended)
5. Click "Browse" and select your `.bin` file
6. Click "Program" to flash

**Pros:** No installation, works on Linux/Mac/Windows, web-based
**Cons:** Requires Chrome/Firefox, slower than native tools

---

### 3. NodeMCU PyFlasher (Linux)

**Best for:** GUI on Linux, ESP8266 boards

**Download:** https://github.com/marcelstoer/nodemcu-pyflasher/releases

**Steps:**
1. Download the Linux binary (AppImage or tarball)
2. Make it executable:
   ```bash
   chmod +x nodemcu-pyflasher-x.x.x-linux.AppImage
   ```
3. Run it:
   ```bash
   ./nodemcu-pyflasher-x.x.x-linux.AppImage
   ```
4. Select your port (e.g., `/dev/ttyUSB0`)
5. Browse for firmware `.bin` file
6. Click "Flash NodeMCU"

**Pros:** GUI interface, self-contained
**Cons:** ESP8266-focused, older project

---

### 4. BrewFlasher (Cross-Platform GUI)

**Best for:** Desktop GUI, multiple platforms

**Download:** https://github.com/thorrak/brewflasher/releases

**Steps:**
1. Download for your platform (Windows/Linux/Mac)
2. Run the application
3. Connect ESP board
4. Select port and firmware file
5. Click "Flash"

**Pros:** Multi-platform, GUI, simple interface
**Cons:** Less known, fewer updates

---

## Cross-Platform Tools

### Thonny IDE

See instructions in Windows section (works identically on Linux/Mac)

**Install on Linux:**
```bash
# Ubuntu/Debian
sudo apt install thonny

# Or download from https://thonny.org/
```

---

## Troubleshooting

### Board not detected

**Symptoms:** "Port not found", "Could not connect", or no COM/USB port listed

**Solutions:**
1. **Check USB cable:** Use a data cable, not just a charging cable
2. **Install drivers:**
   - **Windows:** CP2102 or CH340 drivers (check your board)
   - **Linux:** Usually works out of the box, but may need: `sudo apt install cp210x-dkms`
3. **Check permissions (Linux):**
   ```bash
   # Add user to dialout group
   sudo usermod -a -G dialout $USER
   # Log out and back in
   ```
4. **Try different USB port** (USB 2.0 preferred over 3.0 for older boards)

---

### Flash failed / Timeout

**Symptoms:** "Timed out waiting for packet header", "Failed to connect"

**Solutions:**
1. **Press BOOT/FLASH button:**
   - Connect USB cable
   - Press and hold BOOT button (ESP32) or FLASH button (ESP8266)
   - Click "Flash" or run command
   - Release button when flashing starts
2. **Try lower baud rate:**
   ```bash
   # esptool.py example
   esptool.py --port /dev/ttyUSB0 --baud 460800 ...
   ```
3. **Erase flash first:**
   ```bash
   esptool.py --port /dev/ttyUSB0 --chip esp32 erase_flash
   ```
4. **Power cycle:** Unplug USB, wait 5 seconds, reconnect

---

### Wrong board selected

**Symptoms:** Flash succeeds but device doesn't boot or shows incorrect behavior

**Solutions:**
1. Verify correct chip:
   - **ESP8266:** Use `--chip esp8266`
   - **ESP32:** Use `--chip esp32`
2. Check firmware file matches board:
   - `esp8266_4m_*.bin` → ESP8266 with 4 modules
   - `esp32_4m_*.bin` → ESP32 with 4 modules
3. Flash address matters:
   - **ESP8266:** Use `0x0`
   - **ESP32:** Use `0x1000`

---

### Permission denied (Linux)

**Symptoms:** "Permission denied: '/dev/ttyUSB0'"

**Solutions:**
```bash
# Quick fix (temporary)
sudo esptool.py --port /dev/ttyUSB0 ...

# Permanent fix
sudo usermod -a -G dialout $USER
# Then log out and back in
```

---

## Common Flash Commands Reference

### ESP8266

```bash
# Simple flash (esptool.py)
esptool.py --port /dev/ttyUSB0 --chip esp8266 write_flash --flash_mode dio --flash_size detect 0x0 firmware.bin

# With specific baud rate
esptool.py --port /dev/ttyUSB0 --chip esp8266 --baud 460800 write_flash --flash_mode dio 0x0 firmware.bin

# Erase then flash
esptool.py --port /dev/ttyUSB0 --chip esp8266 erase_flash
esptool.py --port /dev/ttyUSB0 --chip esp8266 write_flash 0x0 firmware.bin

# Read flash (backup)
esptool.py --port /dev/ttyUSB0 --chip esp8266 read_flash 0x0 0x400000 backup.bin
```

### ESP32

```bash
# Simple flash (esptool.py)
esptool.py --port /dev/ttyUSB0 --chip esp32 write_flash -z 0x1000 firmware.bin

# With bootloader, partitions, and app
esptool.py --port /dev/ttyUSB0 --chip esp32 write_flash \
  0x1000 bootloader.bin \
  0x8000 partitions.bin \
  0xe000 boot_app0.bin \
  0x10000 firmware.bin

# Erase flash
esptool.py --port /dev/ttyUSB0 --chip esp32 erase_flash

# Read flash (backup)
esptool.py --port /dev/ttyUSB0 --chip esp32 read_flash 0x0 0x400000 backup.bin
```

---

## Port Naming by OS

### Windows
- Format: `COM1`, `COM3`, `COM4`, etc.
- Find in: Device Manager → Ports (COM & LPT)

### Linux
- Format: `/dev/ttyUSB0`, `/dev/ttyUSB1`, `/dev/ttyACM0`, etc.
- List: `ls /dev/ttyUSB* /dev/ttyACM*`

### macOS
- Format: `/dev/cu.usbserial-*`, `/dev/tty.usbserial-*`
- List: `ls /dev/cu.*` or `ls /dev/tty.usb*`

---

## Baud Rates

Higher baud rates = faster flashing, but may be less stable:

- **115200** - Default, most stable
- **460800** - Fast, usually stable
- **921600** - Fastest, may be unstable on some boards

**Recommendation:** Start with 115200, increase if stable.

---

## Firmware Files for RDA MSG Board

Precompiled firmware for this project is available in [GitHub Releases](https://github.com/rdeangel/rda_msg_board/releases):

**File naming:**
- `rda_msg_board_esp8266_4m_v1.0.0.bin` - ESP8266, 4 modules
- `rda_msg_board_esp8266_8m_v1.0.0.bin` - ESP8266, 8 modules
- `rda_msg_board_esp32_4m_v1.0.0.bin` - ESP32, 4 modules
- `rda_msg_board_esp32_8m_v1.0.0.bin` - ESP32, 8 modules

**Download:** Choose the file matching your board type and module count.

---

## Quick Decision Guide

| Scenario | Recommended Tool |
|----------|-----------------|
| **First time flashing** | ESPHome Flasher (Windows) / esptool.py (Linux) |
| **No installation allowed** | ESP Tool (web-based) |
| **Need to flash many boards** | esptool.py (scriptable) |
| **Prefer GUI on Linux** | NodeMCU PyFlasher or BrewFlasher |
| **Flash MicroPython** | Thonny IDE |
| **Advanced debugging** | ESP Flash Download Tool (Official) |
| **Already use Arduino IDE** | ESP8266/ESP32 board package upload |

---

## Additional Resources

- **esptool Documentation:** https://github.com/espressif/esptool
- **Espressif Official Tools:** https://www.espressif.com/en/support/download/other-tools
- **Thonny IDE:** https://thonny.org/
- **ESPHome Flasher:** https://github.com/esphome/esphome-flasher

---

*Last Updated: 2026-02-25*
*Project: RDA MSG Board*
