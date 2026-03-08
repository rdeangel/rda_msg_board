# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

ESP8266/ESP32-based LED matrix message board firmware with web interface, HTTP REST API, and MQTT support. Displays scrolling messages on MAX7219 LED modules with configurable parameters (repeat count, scroll speed, brightness, buzzer alerts). Features persistent configuration via LittleFS, OTA firmware updates, WiFiManager captive portal, and Home Assistant MQTT Discovery integration.

Originally developed for Arduino IDE, now uses PlatformIO for dependency management and multi-platform builds.

## Build & Development Commands

### Building and Uploading Firmware

**PlatformIO CLI** (not in global PATH — use full path or `~/.platformio/penv/bin/platformio`):
```bash
# ESP8266 builds
~/.platformio/penv/bin/platformio run --target upload --environment d1_mini
~/.platformio/penv/bin/platformio run --target upload --environment nodemcuv2

# ESP32 builds
~/.platformio/penv/bin/platformio run --target upload --environment esp32dev

# CI/Release builds (6 env variants)
# ESP8266 variants
~/.platformio/penv/bin/platformio run --environment nodemcu_4m
~/.platformio/penv/bin/platformio run --environment nodemcu_8m
~/.platformio/penv/bin/platformio run --environment d1_mini_4m
~/.platformio/penv/bin/platformio run --environment d1_mini_8m
# ESP32 variants
~/.platformio/penv/bin/platformio run --environment esp32_4m
~/.platformio/penv/bin/platformio run --environment esp32_8m
```

**VS Code PlatformIO Extension**:
1. Select environment from PlatformIO toolbar
2. Checkmark icon = Build
3. Arrow icon = Upload

**Serial Monitor**: 115200 baud (must close before uploading)

### Release Management

Update version in `platformio.ini` `[common]` section first, then:

```bash
# Preview what would happen — no commits or pushes made
./release.sh --dry-run

# Create release (commits, tags, updates CHANGELOG, pushes to origin + github)
./release.sh "Release notes"

# Force-recreate an existing tag (e.g. after last-minute fixes)
./release.sh --force

# Skip CHANGELOG regeneration
./release.sh --no-changelog
```

Automatically creates annotated git tag, updates `CHANGELOG.md` from conventional commits, pushes to both Forgejo (`origin`) and GitHub (`github`) remotes, triggers CI builds for all 6 platform/module variants.

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
  - `sensors`: Text, Light, Switch entities + `publishAllClockStates()`
  - `clock`: Clock entities + recurrent alarm entities
  - `timer`: Timer/stopwatch entities (disabled on ESP8266 by default via `-DDISABLE_TIMER_FEATURE`)
  - `sleep`: Sleep mode entities
  - `weather`: Weather entities (disabled on all ESP8266 builds via `-DDISABLE_WEATHER_FEATURE`)

**Core Logic**:
- `functions.cpp` - Message processing, display control, NTP time sync, clock rendering
- `utf8_utils.cpp` - UTF-8 to extended ASCII conversion
- `buzzer_utils.cpp` - Audio feedback patterns (chirp library)
- `timer.cpp` - Countdown timer/stopwatch logic
- `main.cpp` - Entry point, initialization sequence

**Font Files** (PROGMEM, flash only — no RAM cost):
- `include/MatrixLight8_font.h` - Compact 8px Matrix Light font
- `include/MatrixLight6_font.h` - Compact 6px Matrix Light font

### Configuration System

**Configuration Files** (stored in LittleFS flash):
1. `/web_config.json` - Web credentials (username/password) and device hostname
2. `/mqtt_config.json` - MQTT server, port, auth, TLS, topic prefix
3. `/defaults_config.json` - Default message parameters (REP, BUZ, DEL, BRI, ASC)
4. `/general.config` - Global buzzer enable/disable, brightness override
5. `/clock.config` - Clock settings (NTP, timezone, face, transitions, AM/PM, date alternation)
6. `/timer.config` - Timer/stopwatch settings (disabled on ESP8266 by default)
7. `/sleep_mode.config` - Sleep schedule settings
8. `/alarm.config` - Recurrent alarm settings

**Configuration Flow**:
1. Boot: `main.cpp` calls `init*StoreConfig()` from `config_manager.cpp`
2. **Deferred loading on ESP8266**: only `initDefaultsStoreConfig()` and `initGeneralStoreConfig()` run before `wm.autoConnect()`. All other configs (web, MQTT, clock, timer, etc.) load after WiFiManager finishes — this preserves heap for the WiFiManager captive portal page.
3. Load: JSON files read from LittleFS into global structs
4. Runtime: Modules read from global config structs (defined in `globals.h/cpp`)
5. Update: Web UI → `web_server.cpp` → `config_manager::saveConfiguration()` → Updates JSON + globals

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

**Feature Flags** (define to disable feature):
- `DISABLE_TIMER_FEATURE` - Exclude timer/stopwatch code (commented out by default — timer IS enabled)
- `DISABLE_WEATHER_FEATURE` - Exclude weather code (**enabled by default on all ESP8266 builds**)
- `DISABLE_SLEEP_MODE_FEATURE` - Exclude sleep mode code
- `DISABLE_ALARM_FEATURE` - Exclude recurrent alarm code
- `DISABLE_HA_CLOCK_ADVANCED` - Skip 7 verbose HA clock discovery entities (**enabled by default on all ESP8266 builds** to reduce heap pressure; entities still configurable via web UI)

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

### Clock Features

The clock subsystem (`functions.cpp`) supports:
- **Clock Faces**: `DEFAULT` (built-in font), `MATRIX_LIGHT` (MatrixLight8 PROGMEM font), `MATRIX_LIGHT_6` (MatrixLight6 PROGMEM font)
- **AM/PM Mode**: 12-hour display with AM/PM suffix (`clockConfig.clockAmPm`)
- **Date Alternation**: 3-state cycle: time → day-of-week → date, controlled by `clockAlternateState` (int 0/1/2) and `clockConfig.dateAlternate`/`dateAlternateSeconds`
- **Date Formats**: `TIME_ONLY`, `TIME_SECONDS` (4-module); `TIME_DATE`, `FULL_DATE`, `TIME_FULL_DATE`, `CUSTOM`, `TIME_SECONDS` (8-module)
- **POSIX Timezone**: Full POSIX TZ string support (e.g. `EST5EDT,M3.2.0,M11.1.0`)
- **NTP Resync**: Configurable interval in hours

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
homeassistant/text/{device}_message/config         # Message input
homeassistant/number/{device}_repeat/config        # Repeat count
homeassistant/number/{device}_buzzer/config        # Buzzer chirps
homeassistant/number/{device}_speed/config         # Scroll delay
homeassistant/light/{device}_brightness/config     # LED brightness
homeassistant/switch/{device}_display_power/config # Display on/off
homeassistant/switch/{device}_buzzer_power/config  # Global buzzer enable
homeassistant/switch/{device}_clock_enable/config  # Clock on/off
homeassistant/number/{device}_clock_brightness/config
homeassistant/select/{device}_clock_face/config    # DEFAULT/MATRIX_LIGHT/MATRIX_LIGHT_6
homeassistant/select/{device}_clock_date_format/config
homeassistant/switch/{device}_clock_date_alternate_enable/config
homeassistant/switch/{device}_clock_ampm/config
# ESP32 only (DISABLE_HA_CLOCK_ADVANCED not set):
homeassistant/number/{device}_clock_transition_speed/config
homeassistant/select/{device}_clock_transition_effect/config
homeassistant/text/{device}_clock_ntp_server/config
homeassistant/text/{device}_clock_custom_tz/config
... (timer, alarm, sleep entities)
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

### Disabling Optional Features

```ini
build_flags =
    -DDISABLE_TIMER_FEATURE
    -DDISABLE_WEATHER_FEATURE    # already on by default for ESP8266
    -DDISABLE_SLEEP_MODE_FEATURE
    -DDISABLE_ALARM_FEATURE
    -DDISABLE_HA_CLOCK_ADVANCED  # already on by default for ESP8266
```

### Re-enabling Advanced HA Clock Entities on ESP8266

Remove `-DDISABLE_HA_CLOCK_ADVANCED` from the environment's `build_flags` in `platformio.ini`. Note: this increases heap pressure and may cause intermittent WiFiManager portal rendering failures.

### Modifying Web Pages

HTML content stored as raw strings in `src/web_pages_*.cpp`:
- `web_pages_main.cpp` - Main dashboard (`/`)
- `web_pages_config.cpp` - Configuration modals (device, MQTT, clock)
- `web_pages_status.cpp` - System/status page

Pages use AJAX with XML responses (generated by `web_data.cpp`).

### Adding New Configuration Values

1. Add field to config struct in `include/config.h`
2. Update `config_manager.cpp` save/load functions (JSON serialization)
3. Add form field in appropriate `web_pages_*.cpp` file
4. Add save handler in `web_server.cpp`
5. Generate XML response in `web_data.cpp` if needed
6. Update `/exportconfig` and `/importconfig` handlers in `web_server.cpp`
7. Add HA discovery entity in `mqtt_discovery_*.cpp` if needed

## Known Issues & Limitations

### Build Environment
- PlatformIO CLI not in global PATH — use `~/.platformio/penv/bin/platformio`
- Serial monitor must be closed before uploading firmware

### Hardware
- ESP32 EasyButton library excluded via `lib_ignore` (ENABLE_FLASH_BUTTON conflicts)
- Multiple LED modules may require external 5V power supply (USB often insufficient)
- Flash button reset feature is ESP8266-only (disabled by default)

### Network
- WiFi credentials changes require manual reconnection (WiFiManager by design)
- mDNS hostname resolution may not work on all networks (use IP as fallback)

### ESP8266 Memory
- Static RAM at ~70% (57KB / 81KB) leaving ~24KB heap. WiFiManager portal page needs contiguous heap to render completely
- `DISABLE_HA_CLOCK_ADVANCED` and `DISABLE_WEATHER_FEATURE` are **on by default** for all ESP8266 builds to relieve heap pressure
- `haLastMessage` buffer is 256 bytes on ESP8266 (vs MSG_SIZE=1024 on ESP32)
- Config loading is deferred: only defaults + general configs load before WiFiManager; all others (web, MQTT, clock, etc.) load after

### Configuration
- MQTT buffer limited to 1024 bytes (`MQTT_MAX_PACKET_SIZE=1024` in common build flags)
- Very long messages may cause display buffer issues (keep messages reasonable length)

### Platform Differences
- **Timer/Stopwatch**: Multi-platform, timer IS enabled by default on ESP8266 (un-comment `-DDISABLE_TIMER_FEATURE` to disable)
- **MQTT TLS/SSL**: ESP32 only (requires `WiFiClientSecure`)
- **Flash Button**: ESP8266 only (ESP32 could support but not implemented)
- **Advanced HA Clock Entities**: ESP32 only by default (gated by `DISABLE_HA_CLOCK_ADVANCED` on ESP8266)

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
version = v1.2.0
```

All environments inherit via `${common.version}` in build flags:
```ini
-DVERSION=\"${common.version}-esp8266\"
```

Version displayed on web interface System page and in HTTP responses.
