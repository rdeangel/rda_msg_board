# GEMINI.md - Project Context & Rules

## Project Description
ESP8266/ESP32-based Message Board firmware (`rda_msg_board`). Controls MAX7219 LED matrices to display MQTT messages. Features a web interface for configuration and firmware updates. Originally developed for Arduino IDE, migrated to PlatformIO for better dependency management and modern development workflow.

## Architecture Overview

### Four-Tier Configuration System
1. **Web Credentials** (`/web_config.json` in LittleFS)
   - HTTP Basic Auth username/password
   - Custom device hostname (or default `RDA-MSG-XXXXXX`)
   
2. **MQTT Settings** (`/mqtt_config.json` in LittleFS)
   - Enable/disable MQTT
   - Server address and port
   - Anonymous or authenticated mode
   - Topic prefix with wildcard support
   - Connection/disconnection alerts
   - **MQTT Messages Toggle**: Enable/disable display of incoming MQTT messages (while keeping connection active)

3. **Message Defaults** (`/defaults_config.json` in LittleFS)
   - Message-specific fallbacks (Repeat, Buzzer count, Scroll delay, Brightness)
   - UTF-8 ASCII conversion

4. **Timer Settings** (`/timer.config` in LittleFS)
   - Timer vs Stopwatch mode
   - Duration, brightness, buzzer settings
   - Auto-repeat and alert configuration
   - **Note**: Can be disabled on ESP8266 via `-DDISABLE_TIMER_FEATURE` build flag to save memory

5. **General Settings** (`/general.config` in LittleFS)
   - Global device-wide parameters
   - Global Buzzer enable/disable switch (`buzzerEnable`)
   - **Brightness Override**: Global display brightness override (`brightnessOverrideEnable`, `brightnessOverrideValue`)

6. **Sleep Mode Settings** (`/sleep_mode.config` in LittleFS)
   - Scheduled display dimming/blackout
   - Start/End time configuration
   - **Weekend Mode**: Optional alternate start/end times for Saturday and Sunday

### Core Components
- **WiFiManager**: Captive portal for initial WiFi setup (AP: `RDA-MSG-XXXXXX`, Password: `wifi-setup`) - Multi-platform
- **Web Server**: HTTP server on port 80 with Basic Auth (platform-abstracted)
  - ESP8266: ESP8266WebServer
  - ESP32: WebServer
- **PubSubClient**: MQTT client with 2048-byte buffer for large messages - Multi-platform
- **MD_Parola**: LED matrix scrolling text effects with UTF-8 support - Multi-platform
- **LittleFS**: Flash filesystem for persistent configuration storage - Multi-platform
- **EasyButton**: Optional FLASH button handling (ESP8266 only, disabled by default)
- **Timer/Stopwatch**: Precision millisecond timer with MQTT/API control - Multi-platform (can be disabled on ESP8266 via `-DDISABLE_TIMER_FEATURE` build flag to save memory; Weather is disabled on ESP8266 by default due to heap constraints)
- **Chirp Library**: Pre-defined musical alerts (Gentle Dawn, Cheerful, Urgent, For Elise, Mario Bros, etc.) for timer and system events.

## HTTP Endpoints

All endpoints require HTTP Basic Authentication (default: `admin`/`msgboard`).

### Message Endpoints
- `GET /` - Main web interface page
- `GET /arg` - Send message via URL parameters (MSG, REP, BUZ, DEL, BRI, ASC)
- `POST /api` - Send message via JSON body
- `POST /savegeneral` - Update general settings (BuzzerEnable, BrightnessOverrideEnable, BrightnessOverrideValue)
- `POST /api/clock` - Update clock configuration via JSON
- `POST /api/timer` - Update timer configuration/control via JSON

### Configuration Endpoints  
- `GET /deviceconfig` - Device credentials configuration page
- `POST /changecredentials` - Update username/password/hostname
- `GET /mqttconfig` - MQTT configuration page
- `POST /applymqttconfig` - Update MQTT settings
- `GET /generalvars` - XML data for general settings modal
- `GET /exportconfig` - Export complete config as JSON
- `POST /importconfig` - Import config from JSON backup

### Defaults Management
- `GET /setdefault?type=REP&value=10` - Set individual default value (REP/BUZ/DEL/BRI)
- `GET /resetdefaults` - Reset all defaults to hardcoded values

### System Management
- `GET /system` - System/firmware update page
- `GET /reboot` - Reboot device
- `GET /clockvars` - XML data for clock config page
- `POST /saveclock` - Update clock settings
- `GET /factoryreset` - Factory reset (clear all config, reboot to AP mode)

### AJAX Data Endpoints
- `GET /mainpagevars` - XML data for main page (clientid, defaults, version)
- `GET /changecredvars` - XML data for credentials page
- `GET /mqttpagevars` - XML data for MQTT config page  
- `GET /updatevars` - XML data for system page

### OTA Upload
- `POST /submitupdate` - Firmware upload handler (multipart form data)

## MQTT Topic Subscription Pattern

For topic prefix: `rdadotmatrix/generic`

### Subscribed Topics
```
rdadotmatrix                    # Root topic - plain messages
rdadotmatrix/json               # Root topic - JSON messages  
rdadotmatrix/generic            # Configured prefix - plain messages
rdadotmatrix/generic/json       # Configured prefix - JSON messages
RDA-MSG-ABCDEF                  # Device hostname - plain messages
RDA-MSG-ABCDEF/json             # Device hostname - JSON messages
```

### Wildcard Support
- **Hash (`#`)**: `rdadotmatrix/generic/#` subscribes to all subtopics
- **Plus (`+`)**: Single-level wildcard in topic path

### Message Handling
- **Plain messages**: Topics NOT ending in `/json` use defaults for all parameters except MSG
- **JSON messages**: Topics ending in `/json` parse JSON for MSG, REP, BUZ, DEL, BRI, ASC fields
- **Published status**: Device publishes to `RDA-MSG-ABCDEF/status: "Connected"` on MQTT connect

## Code Structure

### Key Files
- `src/main.cpp`: Main entry point, setup WiFi, initialize subsystems, main loop
- `src/web_server.cpp`: HTTP server routing and request handling (Refactored)
- `src/config_manager.cpp`: Persistence layer for loading/saving all configurations (New)
- `src/web_data.cpp`: Web page variable generation and formatting (New)
- `src/web_pages_*.cpp`: Embedded HTML/CSS/JS pages (Config, Main, Status)
- `src/mqtt.cpp`: MQTT connection, topic subscription, callback handling
- `src/mqtt_discovery.cpp`: Home Assistant MQTT discovery facade (Refactored)
- `src/mqtt_discovery_*.cpp`: Domain-specific discovery modules (Core, Sensors, Clock)
- `src/functions.cpp`: Message processing, display control, UTF-8 conversion, factory reset
- `src/globals.cpp`: Global variable definitions
- `include/config.h`: Hardware pins, buffer sizes, default credentials, feature flags

### Configuration Management Pattern
Each config type follows:
1. **Load**: `loadXXXConfiguration()` reads from LittleFS JSON file
2. **Save**: `saveXXXConfiguration()` writes to LittleFS JSON file  
3. **Init**: `initXXXStoreConfig()` loads or creates defaults on boot
4. **Apply**: Update runtime variables from config struct
5. **Print**: Debug output via Serial (if DEBUG=1)

**Config Objects**:
- `webConfig`: Username, Password, Hostname
- `mqttConfig`: Server, Port, Prefix, etc.
- `defaultsConfig`: REP, BUZ, DEL, BRI, ASC
- `generalConfig`: buzzerEnable, brightnessOverrideEnable, brightnessOverrideValue
- `clockConfig`: enabled, tzString, brightness, transition, etc.
- `timerConfig`: enabled, mode, durationSeconds, alertChirp, etc.
- `sleepModeConfig`: enabled, onTime, offTime, muteOnly, weekendEnabled, weekendOnTime, weekendOffTime

### Flow: Setup → Loop
**Setup**:
1. Generate `clientId` from chip ID
   - ESP8266: `RDA-MSG-` + last 6 hex digits of chip ID
   - ESP32: `RDA-MSG-` + derived from MAC address
2. Initialize LittleFS
3. Load all configurations (web, MQTT, defaults)
4. Start WiFiManager (AP mode if no WiFi, or connect to saved network)
5. Initialize web server with all routes
6. Display startup message on LED matrix
7. Initialize MQTT if enabled

**Loop**:
1. Update mDNS (ESP8266 only - not required on ESP32)
2. Handle HTTP server requests
3. Check MQTT connection (reconnect if needed)
4. Process message scrolling and display
5. Read FLASH button (if enabled - ESP8266 only)

## Hardware Configuration

**Platform-specific pin assignments** (in `include/config.h`):

**ESP8266:**
```cpp
#define MAX_DEVICES 4           // Number of MAX7219 modules (change to 8, 12, etc.)
#define CLK_PIN D5              // GPIO 14 - SPI Clock
#define DATA_PIN D7             // GPIO 13 - SPI MOSI
#define CS_PIN D8               // GPIO 15 - Chip Select (D4 for D1 Mini)
#define BUZZER_PIN D1           // GPIO 5 - Buzzer output (D2 for D1 Mini)
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
```

**ESP32:**
```cpp
#define MAX_DEVICES 4           // Number of MAX7219 modules (change to 8, 12, etc.)
#define CLK_PIN 18              // VSPI CLK
#define DATA_PIN 23             // VSPI MOSI
#define CS_PIN 5                // VSPI CS
#define BUZZER_PIN 4            // General purpose GPIO
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
```

Optional features:
```cpp
#define DEBUG 1                 // Enable serial debug output
#define ENABLE_FLASH_BUTTON 0   // Enable GPIO0 FLASH button (ESP8266 only, disabled by default)
```

## Default Values

### Hardcoded Defaults (fallback)
```cpp
repeatDefault = "10"            // Message scroll repeat count
buzzerDefault = "10"            // Buzzer chirp count
scrollDelayDefault = "35"       // Delay per scroll step (ms)
ledBrightnessDefault = "7"      // Brightness 0-15
asciiConvDefault = "1"          // UTF-8 conversion enabled
```

### WiFi Setup Credentials
```cpp
SSID: clientId                  // RDA-MSG-XXXXXX (unique device identifier)
                                // ESP8266: Last 6 hex digits of chip ID
                                // ESP32: Derived from MAC address
Password: "wifi-setup"          // Hardcoded in main.cpp
AP IP: 192.168.4.1             // Default WiFiManager AP IP
```

### HTTP Credentials
```cpp
DEFAULT_WEB_USER: "admin"       // Defined in config.h
DEFAULT_WEB_PASS: "msgboard"     // Defined in config.h
```

## Libraries & Dependencies

Managed by PlatformIO (from `platformio.ini`) - All multi-platform compatible:
```ini
MD_MAX72XX@^3.5.1              # LED matrix driver (ESP8266 & ESP32)
MD_Parola@^3.7.5               # Scrolling text effects (ESP8266 & ESP32)
WiFiManager@^2.0.17            # WiFi config portal (ESP8266 & ESP32)
EasyButton@^2.0.3              # Button debouncing (ESP8266 only, excluded from ESP32 builds)
ArduinoJson@^7.4.2             # JSON parsing/serialization (ESP8266 & ESP32)
PubSubClient@^2.8              # MQTT client (ESP8266 & ESP32)
```

**Note**: EasyButton is excluded from ESP32 builds via `lib_ignore` due to dependency requirements when `ENABLE_FLASH_BUTTON=0` (default).

## Build Instructions

To build and upload the firmware, use the following command (or the `/test_build` workflow):

**PlatformIO CLI** (requires full path on Windows):
```bash
# ESP8266 builds
platformio run --target upload --environment d1_mini
platformio run --target upload --environment nodemcuv2

# ESP32 builds
platformio run --target upload --environment esp32dev

# CI/Release builds (6 env variants)
# ESP8266 variants
platformio run --environment nodemcu_4m
platformio run --environment nodemcu_8m
platformio run --environment d1_mini_4m
platformio run --environment d1_mini_8m
# ESP32 variants
platformio run --environment esp32_4m
platformio run --environment esp32_8m
```

**VS Code PlatformIO Extension**:
1. Select environment from PlatformIO toolbar
2. Checkmark icon = Build
3. Arrow icon = Upload

**Serial Monitor**: 115200 baud (must close before uploading)

### Environments

**ESP8266:**
- `nodemcuv2`: NodeMCU 1.0 (ESP12E Module) - Development
- `d1_mini`: WeMos D1 Mini - Development
- `nodemcu_4m`: NodeMCU 1.0 CI/Release build (4 modules)
- `nodemcu_8m`: NodeMCU 1.0 CI/Release build (8 modules)
- `d1_mini_4m`: D1 Mini CI/Release build (4 modules)
- `d1_mini_8m`: D1 Mini CI/Release build (8 modules)

**ESP32:**
- `esp32dev`: ESP32 DevKit v1 - Development
- `esp32_4m`: ESP32 DevKit v1 CI/Release build (4 modules)
- `esp32_8m`: ESP32 DevKit v1 CI/Release build (8 modules)

## Tech Stack
- **Platform**: PlatformIO / Arduino Framework for ESP8266 & ESP32
- **Language**: C++ (Embedded)
- **Web Interface**: Embedded HTML/CSS/JS in C++ raw strings (`src/web_pages.cpp`)
- **Hardware**:
  - **ESP8266**: D1 Mini, NodeMCU 1.0
  - **ESP32**: ESP32 DevKit v1
  - **Display**: MAX7219 Dot Matrix Modules (4, 8, or more)
- **Storage**: LittleFS flash filesystem (multi-platform)
- **Networking**:
  - **WiFi**: Platform-native libraries (WiFi on ESP32, ESP8266WiFi on ESP8266)
  - **WiFiManager**: Multi-platform WiFi configuration portal
  - **Web Server**: Platform-abstracted (ESP8266WebServer / WebServer)
  - **MQTT**: WiFiClient + PubSubClient (multi-platform)

## Important Code Patterns

### UTF-8 Character Conversion
```cpp
// functions.cpp: utf8Ascii()
// Converts UTF-8 multi-byte sequences to extended ASCII ISO-8859-1
// Handles characters like: €, ñ, ü, à, ©, °, etc.
// Special handling for Euro symbol (€) and ellipsis (…)
```

### Message Parameter Handling
All message functions follow pattern:
1. Check if parameter provided (e.g., `newRepeatAvailable`)
2. If yes: Use provided value
3. If no: Use stored default value
4. Parse and apply to display

### Config Save/Load Safety
- Files opened in read/write modes with error checking
- JSON deserialization errors handled gracefully
- Missing fields use defaults
- Config printed to Serial for debugging

### Factory Reset Process
1. Delete all config files from LittleFS
2. Disconnect from WiFi (clears saved credentials)
3. Delay for filesystem flush
4. ESP.restart() to reboot into AP mode

## Key Files & Locations

- `.agent/workflows/`: Contains executable workflow scripts
- `images/`: Screenshots for documentation (wiring.gif, homepage.jpg, etc.)
- `node_red_flow.json`: NodeRed integration example with RSS feed handling

## Migration Notes (Arduino IDE → PlatformIO)

### What Changed
- **Build system**: Arduino IDE `.ino` → PlatformIO `platformio.ini` + separated `.cpp`/`.h` files
- **Library management**: Manual downloads → Auto-managed in `platformio.ini`
- **Multiple boards**: Single `.ino` → Multiple environments in `platformio.ini`
- **Version strings**: Hardcoded → Build flags per environment
- **IDE**: Arduino IDE → VS Code + PlatformIO extension

### What Stayed the Same
- All core functionality preserved
- HTTP endpoints unchanged
- MQTT topic patterns unchanged  
- Web interface identical
- Configuration storage compatible
- Pin assignments configurable

### New Features
- `/exportconfig` and `/importconfig` endpoints for backup/restore
- `/setdefault` and `/resetdefaults` for custom defaults management
- Improved JSON handling with ArduinoJson v7
- Better serial debug output organization

## Adding New Configuration

When adding new configuration parameters to the project, follow this checklist to ensure complete integration:

1.  **Define**: Add the field to the relevant struct in `include/config.h` (using `char` array `Holder` for persistence).
2.  **Declare**: Add runtime global variables in `include/globals.h` and `src/globals.cpp`.
3.  **Persistence**: Update `load...Configuration` and `save...Configuration` in the relevant manager file (e.g., `src/mqtt.cpp`).
4.  **Initialize**: Update `init...StoreConfig` to set defaults.
5.  **Web UI**: Add the input field to the relevant `src/web_pages.cpp` file and update AJAX/Form handling.
6.  **Web Data**: Update the XML generator in `src/web_data.cpp` to serve the current value.
7.  **Web Server**: Update the form handler in `src/web_server.cpp` to parse and apply the value.
8.  **Backup**: **CRITICAL** - Update `/exportconfig` and `/importconfig` handlers in `src/web_server.cpp` to include the new field in JSON backups.
9.  **Home Assistant**: If applicable, update MQTT discovery and command handlers in `src/mqtt_discovery_sensors.cpp`.
