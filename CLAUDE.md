# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

ESP8266/ESP32-based LED matrix message board firmware with web interface, HTTP REST API, and MQTT support. Displays scrolling messages on MAX7219 LED modules with configurable parameters (repeat count, scroll speed, brightness, buzzer alerts). Features persistent configuration via LittleFS, OTA firmware updates, WiFiManager captive portal, and Home Assistant MQTT Discovery integration.

Originally developed for Arduino IDE, now uses PlatformIO for dependency management and multi-platform builds.

## Build & Development Commands

### Building and Uploading Firmware

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

### Release Management

Update version in `platformio.ini` `[common]` section first, then:

```bash
# Linux/Mac/Git Bash
./release.sh "Release notes"
./release.sh --force
```

Automatically creates git tag, pushes to GitHub, and triggers CI builds for all 4 platform/module variants.

### Testing APIs

**HTTP REST API**:
```bash
# GET with URL parameters
curl --user admin:msgboard -X GET -G 'http://192.168.1.100/arg' \
  --data-urlencode "MSG=Test Message" \
  --data-urlencode "REP=5" \
  --data-urlencode "BUZ=3"

# POST with JSON
curl --user admin:msgboard -X POST http://192.168.1.100/api \
  -H 'Content-Type: application/json' \
  -d '{"MSG":"Test","REP":1,"BRI":10}'

# Export config backup
curl --user admin:msgboard http://192.168.1.100/exportconfig > backup.json

# Import config
curl --user admin:msgboard -X POST http://192.168.1.100/importconfig \
  -H 'Content-Type: application/json' -d @backup.json
```

**MQTT Testing**:
```bash
# Plain text message (uses defaults)
mosquitto_pub -h broker.local -t "rdadotmatrix/generic" -m "Test message"

# JSON message with parameters
mosquitto_pub -h broker.local -t "rdadotmatrix/generic/json" \
  -m '{"MSG":"Alert","REP":5,"BRI":10}'

# Stop current message
mosquitto_pub -h broker.local -t "rdadotmatrix/json" -m '{"MSG":""}'
```

## Architecture

### Platform Support

**Multi-platform codebase** supporting both ESP8266 and ESP32:
- Platform-specific abstractions via `PlatformWebServer` typedef
- Conditional compilation based on `ESP8266` or `ESP32` defines
- Pin assignments differ by platform (see Pin Configuration section)
- ESP32 exclusive features: MQTT TLS/SSL

### Module Organization

**Web Interface Layer**:
- `web_server.cpp` - HTTP route handlers, authentication, request dispatch
- `config_manager.cpp` - LittleFS persistence for JSON configs
- `web_data.cpp` - XML/JSON response generation for AJAX
- `web_pages_*.cpp` - HTML/CSS/JS as raw string literals

**MQTT Layer**:
- `mqtt.cpp` - PubSubClient wrapper, connection management, TLS support (ESP32)
- `mqtt_discovery_*.cpp` - Home Assistant MQTT Discovery entities
  - `core`: Base topics, device registry
  - `sensors`: Text, Light, Switch entities
  - `clock`: Clock-specific entities
  - `timer`: Timer/stopwatch entities (disabled on ESP8266 by default via `-DDISABLE_TIMER_FEATURE`)

**Core Logic**:
- `functions.cpp` - Message processing, display control, NTP time sync
- `utf8_utils.cpp` - UTF-8 to extended ASCII conversion
- `buzzer_utils.cpp` - Audio feedback patterns
- `timer.cpp` - Countdown timer/stopwatch logic (disabled on ESP8266 by default via `-DDISABLE_TIMER_FEATURE`)
- `main.cpp` - Entry point, initialization sequence

### Configuration System

**Four Configuration Files** (stored in LittleFS flash):
1. `/web_config.json` - Web credentials (username/password) and device hostname
2. `/mqtt_config.json` - MQTT server, port, auth, TLS, topic prefix
3. `/defaults_config.json` - Default message parameters (REP, BUZ, DEL, BRI, ASC)
4. `/general.config` - Global buzzer enable/disable, brightness override

**Configuration Flow**:
1. Boot: `main.cpp` calls `init*StoreConfig()` from `config_manager.cpp`
2. Load: JSON files read from LittleFS into global structs
3. Runtime: Modules read from global config structs (defined in `globals.h/cpp`)
4. Update: Web UI → `web_server.cpp` → `config_manager::saveConfiguration()` → Updates JSON + globals

### Pin Configuration

**ESP8266 Defaults** (overridable via build flags):
- `CLK_PIN`: D5 (GPIO 14) - SPI Clock
- `DATA_PIN`: D7 (GPIO 13) - SPI MOSI
- `CS_PIN`: D8 (GPIO 15) - Chip Select
- `BUZZER_PIN`: D1 (GPIO 5)

**ESP32 Defaults** (uses VSPI):
- `CLK_PIN`: GPIO 18 - VSPI CLK
- `DATA_PIN`: GPIO 23 - VSPI MOSI
- `CS_PIN`: GPIO 5 - VSPI CS
- `BUZZER_PIN`: GPIO 4

Override in `platformio.ini` build_flags:
```ini
build_flags =
    -DCLK_PIN=18
    -DCS_PIN=5
    -DMAX_DEVICES=8
```

### Key Constants

**Hardware**:
- `MAX_DEVICES` - Number of MAX7219 modules (default: 4, override via `-DMAX_DEVICES=8`)
- `HARDWARE_TYPE` - MD_MAX72XX::FC16_HW

**Configuration**:
- `DEFAULT_WEB_USER` / `DEFAULT_WEB_PASS` - Initial web credentials (admin/msgboard)
- `ENABLE_FLASH_BUTTON` - Factory reset button on GPIO0 (default: 0, ESP8266 only)
- `DISABLE_TIMER_FEATURE` - Exclude timer/stopwatch code (define to disable)

**Debugging**:
- `DEBUG` - Enable serial output (define as 1)
- `PRINT_CALLBACK` - Display callback debugging

## Important Code Locations

### HTTP Endpoints

All routes defined in `web_server.cpp:httpWebDirDef()`:
- `/` - Main message board interface
- `/arg` - GET with URL-encoded parameters
- `/api` - POST with JSON payload
- `/deviceconfig` - Device credentials & hostname
- `/mqttconfig` - MQTT configuration
- `/generalvars` - General settings (XML response)
- `/savegeneral` - Save general settings
- `/clockvars` - Clock settings (XML response)
- `/saveclock` - Save clock settings
- `/system` - OTA upload, reboot, factory reset
- `/exportconfig` - Download config backup (JSON)
- `/importconfig` - Restore config from JSON
- `/setdefault` - Save individual default value
- `/resetdefaults` - Reset to hardcoded defaults
- `/reboot` - Reboot device
- `/factoryreset` - Wipe all config

### Message Processing Pipeline

1. **HTTP Input**: `web_server.cpp` routes → `functions.cpp:onMessageCallHttp()` or `onMessageCallJson()`
2. **MQTT Input**: `mqtt.cpp:mqttCallBack()` → parses plain/JSON → calls `onMessageCallJson()`
3. **Display**: `functions.cpp:scrollTextParola()` - Main display loop
4. **UTF-8 Conversion**: `utf8_utils.cpp:utf8Ascii()` - Character mapping for international chars

### MQTT Topics

Device subscribes to (where prefix = configured topic, device = RDA-MSG-XXXXXX):
```
rdadotmatrix              # Root - plain messages
rdadotmatrix/json         # Root - JSON messages
{prefix}                  # Configured prefix - plain
{prefix}/json             # Configured prefix - JSON
{device}                  # Device-specific - plain
{device}/json             # Device-specific - JSON
```

Supports wildcards: `#` (multi-level), `+` (single-level)

Device publishes:
```
{device}/status           # "Connected" on connect
```

### Home Assistant Discovery

MQTT Discovery topics published at startup (when MQTT enabled):
```
homeassistant/text/{device}_message/config        # Message input
homeassistant/number/{device}_repeat/config       # Repeat count
homeassistant/number/{device}_buzzer/config       # Buzzer chirps
homeassistant/number/{device}_speed/config        # Scroll delay
homeassistant/light/{device}_brightness/config    # LED brightness
homeassistant/switch/{device}_display_power/config # Display on/off
homeassistant/switch/{device}_buzzer_power/config  # Global buzzer enable
... (additional clock, timer, sensor entities)
```

Published by `mqtt_discovery_*.cpp` modules on MQTT connection.

## Common Development Tasks

### Changing Pin Assignments

**Option 1**: Edit `include/config.h` defaults

**Option 2**: Override via `platformio.ini` build_flags:
```ini
[env:d1_mini]
build_flags =
    ${env.build_flags}
    -DCS_PIN=D4
    -DBUZZER_PIN=D1
```

### Changing LED Module Count

Add to environment's `build_flags` in `platformio.ini`:
```ini
build_flags =
    -DMAX_DEVICES=8  # 4, 8, 12, etc.
```

### Enabling Debug Output

Add to `build_flags` in `platformio.ini`:
```ini
build_flags =
    -DDEBUG=1
```

Or define in `include/config.h`:
```cpp
#define DEBUG 1
```

### Disabling Optional Features

**Timer/Stopwatch** (disabled on ESP8266 by default, can be enabled):
```ini
build_flags =
    -DDISABLE_TIMER_FEATURE
```

**Flash Button** (ESP8266):
Edit `include/config.h`:
```cpp
#define ENABLE_FLASH_BUTTON 1  // 0 = disabled (default)
```

### Modifying Web Pages

HTML content stored as raw strings in `src/web_pages_*.cpp`:
- `web_pages_main.cpp` - Main dashboard (`/`)
- `web_pages_config.cpp` - Configuration modals (device, MQTT, clock)
- `web_pages_status.cpp` - System/status page

Pages use AJAX with XML responses (generated by `web_data.cpp`).

### Adding New Configuration Values

1. Add field to config struct in `include/globals.h`
2. Update `config_manager.cpp` save/load functions (JSON serialization)
3. Add form field in appropriate `web_pages_*.cpp` file
4. Add save handler in `web_server.cpp`
5. Generate XML response in `web_data.cpp` if needed

## Known Issues & Limitations

### Build Environment
- PlatformIO CLI not in global PATH on Windows (use full path or VS Code extension)
- Serial monitor must be closed before uploading firmware

### Hardware
- ESP32 EasyButton library excluded via `lib_ignore` (ENABLE_FLASH_BUTTON conflicts)
- Multiple LED modules may require external 5V power supply (USB often insufficient)
- Flash button reset feature is ESP8266-only (disabled by default)

### Network
- WiFi credentials changes require manual reconnection (WiFiManager by design)
- mDNS hostname resolution may not work on all networks (use IP as fallback)

### Configuration
- MQTT buffer limited to 4096 bytes (configurable via `MQTT_MAX_PACKET_SIZE` build flag)
- Very long messages may cause display buffer issues (keep messages reasonable length)

### Platform Differences
- **Timer/Stopwatch**: Multi-platform, disabled on ESP8266 by default via `-DDISABLE_TIMER_FEATURE` build flag
- **MQTT TLS/SSL**: ESP32 only (requires `WiFiClientSecure`)
- **Flash Button**: ESP8266 only (ESP32 could support but not implemented)

## Default Credentials & Access

**WiFi Setup Mode** (on first boot or after factory reset):
- SSID: `RDA-MSG-XXXXXX` (XXXXXX = last 6 hex chars of chip ID)
- Password: `wifi-setup`
- Portal: `http://192.168.4.1` (auto-opens on most devices)

**Web Interface**:
- Username: `admin`
- Password: `msgboard`
- Change via `/deviceconfig` page immediately after setup

**mDNS Hostname**:
- Default: `http://RDA-MSG-XXXXXX.local`
- Customizable via Device Config page

## Documentation Files

Detailed documentation in `docs/` directory:
- `ARCHITECTURE.md` - Module structure and layer separation
- `HARDWARE_REFERENCE.md` - Pin assignments, resource allocation
- `HOME_ASSISTANT_INTEGRATION.md` - MQTT Discovery setup guide
- `HTTP_API_EXAMPLES.md` - Comprehensive REST API reference
- `MQTT_EXAMPLES.md` - MQTT message formats and CLI examples
- `MQTT_TLS_IMPLEMENTATION.md` - ESP32 SSL/TLS technical details

`README.md` contains complete user-facing documentation including:
- Hardware wiring diagrams
- Setup instructions
- API reference
- Home Assistant integration (automatic and manual)
- NodeRed integration example
- Troubleshooting guide

## Version Management

**Single source of truth**: `platformio.ini` `[common]` section:
```ini
[common]
version = v0.9.2
```

All environments inherit via `${common.version}` in build flags:
```ini
-DVERSION=\"${common.version}-esp8266\"
```

Version displayed on web interface System page and in HTTP responses.
