# AGENT.md - Agent Notes & Memory

## Current Context
- **Status**: Active development
- **Version**: v1.2.0
- **Target Hardware**:
  - **ESP8266**: NodeMCU 1.0, WeMos D1 Mini
  - **ESP32**: ESP32 DevKit v1
- **Multi-Platform**: Supports both ESP8266 and ESP32 with unified codebase

## Project Summary

ESP8266/ESP32-based LED matrix message board with web interface, HTTP API, and MQTT support. Displays scrolling messages with configurable parameters. Features persistent configuration, OTA updates, clock with multiple font faces, and Home Assistant integration. Uses platform-specific abstractions for seamless cross-platform compatibility.

## Recent Changes

### v1.4.0 (March 2026) — Non-Blocking Operations & Recurrent Alarm Display
**ESP32 FreeRTOS Task Architecture** — Display loop on Core 1 now runs uninterrupted:
- **Buzzer Task** (`buzzer_task.cpp`): `playChirpByName()` posts to queue on ESP32 (non-blocking); ESP8266 path unchanged (blocking)
- **HTTP Task** (`http_task.cpp`): All HTTP server handling moved to Core 0; `handleHttpServer()` is a no-op on ESP32, preventing page loads from blocking the display
- **Crypto Fetch Task** (`crypto.cpp`): HTTPS fetch via binary semaphore; writes to shadow buffer; main loop swaps on completion; 16KB stack
- **Weather Fetch Task** (`weather.cpp`): HTTP fetch via binary semaphore; writes to `WeatherShadow` struct; 12KB stack
- **Recurrent Alarm Display** (`MODE_RECURRENT_ALARM`): New display mode shows configurable brief indicator (default `"* * *"`); on ESP32 clock continues during chirp, on ESP8266 display freezes but shows indicator
- **New config field**: `recurrentAlarmConfig.displayMessage` — user-configurable recurrent alarm display indicator

### v1.2.0 (March 2026)
- **AM/PM Mode**: 12-hour clock display with AM/PM suffix (`clockConfig.clockAmPm`, `clock_ampm` HA entity)
- **Date Alternation**: 3-state cycle (time → day-of-week → date) using `clockAlternateState` int (0/1/2), replacing `showingDate` bool; configurable interval via `dateAlternateSeconds`
- **Matrix Light 6px Font**: New compact 6px font (`MatrixLight6Font`) in PROGMEM alongside existing `MatrixLight8Font`
- **Clock Face Select**: MQTT/HA control of clock font via `clockConfig.clockFace` (DEFAULT / MATRIX_LIGHT / MATRIX_LIGHT_6)
- **DISABLE_HA_CLOCK_ADVANCED**: New flag auto-enabled on all ESP8266 builds; gates 7 verbose HA clock entities to reduce heap pressure and stabilise WiFiManager captive portal
- **haLastMessage reduced**: 1024 → 256 bytes on ESP8266 (saves ~768 bytes BSS)
- **release.sh fixes**: `--dry-run` no longer makes local commits; fixed malformed `push_all()` function

### v1.0.0 (earlier 2026)
- Recurrent alarm with disable-weekends option
- Sleep mode with weekend schedules
- Timer/Stopwatch feature
- Config export/import (`/exportconfig`, `/importconfig`)
- Advanced clock settings (POSIX timezone, transition effects, date formats)
- ArduinoJson v7

## Architecture Overview

### Configuration Tiers
1. **Web Credentials** (`/web_config.json`) - HTTP auth & hostname
2. **MQTT Settings** (`/mqtt_config.json`) - Server, topics, TLS, authentication
3. **Message Defaults** (`/defaults_config.json`) - Repeat, buzzer, speed, brightness, ASCII
4. **General Settings** (`/general.config`) - Global buzzer enable, brightness override
5. **Clock Settings** (`/clock.config`) - NTP, timezone, face, transitions, AM/PM, date alternation
6. **Timer Settings** (`/timer.config`) - Countdown/stopwatch config
7. **Sleep Mode** (`/sleep_mode.config`) - Scheduled on/off times, weekend mode
8. **Alarm** (`/alarm.config`) - Recurrent alarm with interval and chirp selection

**Deferred loading on ESP8266**: Only configs 3 & 4 load before WiFiManager to preserve heap for the captive portal page. All others load after WiFiManager completes.

### Key Components
- **Web Server**: Platform-abstracted server on port 80, Basic Auth
  - ESP8266: ESP8266WebServer
  - ESP32: WebServer
- **MQTT Client**: PubSubClient, `MQTT_MAX_PACKET_SIZE=1024`
- **Display Driver**: MD_Parola + MD_MAX72xx, hardware SPI, UTF-8 support
- **Fonts**: Built-in Parola font (DEFAULT), MatrixLight8 & MatrixLight6 (both PROGMEM)
- **Storage**: LittleFS (compatible on both platforms)
- **WiFi**: WiFiManager v2.0.17 captive portal

## Complete Feature List

### Web Interface
✅ Main message board page (`/`)
✅ Device credentials configuration (`/deviceconfig`)
✅ MQTT configuration (`/mqttconfig`)
✅ System/firmware update page (`/system`)
✅ HTTP Basic Authentication
✅ AJAX data updates (XML responses)
✅ Responsive design with JavaScript
✅ In-page confirmation modals

### HTTP API
✅ GET `/arg` - URL-encoded parameters
✅ POST `/api` - JSON message body
✅ POST `/savegeneral` - Update general settings
✅ GET `/generalvars` - XML for general modal
✅ GET `/exportconfig` - Export configuration (JSON)
✅ POST `/importconfig` - Import configuration (JSON)
✅ GET `/setdefault` - Save individual default value
✅ GET `/resetdefaults` - Reset to hardcoded defaults
✅ GET `/reboot` - Reboot device
✅ GET `/clockvars` - XML for clock modal
✅ POST `/saveclock` - Update clock settings
✅ GET `/factoryreset` - Factory reset
✅ POST `/submitupdate` - OTA firmware upload

### MQTT Features
✅ Topic subscriptions with wildcards (`#`, `+`)
✅ Plain text messages (uses defaults)
✅ JSON messages (custom parameters)
✅ Anonymous or authenticated modes
✅ Connection/disconnection alerts
✅ Status publishing (`RDA-MSG-ABCDEF/status`)
✅ Device-specific topics
✅ Home Assistant MQTT Discovery (all platforms)
✅ MQTT TLS/SSL (ESP32 only)

### Clock Features
✅ NTP time sync with configurable server and POSIX timezone
✅ Clock faces: DEFAULT, MATRIX_LIGHT (8px), MATRIX_LIGHT_6 (6px)
✅ AM/PM (12-hour) mode
✅ Date alternation: 3-state cycle (time → day-of-week → date)
✅ Date formats: TIME_ONLY, TIME_SECONDS (4-module); + TIME_DATE, FULL_DATE, CUSTOM etc. (8-module)
✅ Configurable transition effects and speeds
✅ Recurrent alarm with interval, chirp selection, disable-weekends option

### Message Parameters
✅ MSG - Message text
✅ REP - Repeat count (default: 10, 0 = infinite)
✅ BUZ - Buzzer chirps (default: 10)
✅ DEL - Scroll delay ms (default: 35)
✅ BRI - Brightness 0-15 (default: 7)
✅ ASC - UTF-8 conversion (default: 1/on)
✅ ALERTCHIRP - Alert chirp pattern

### System Features
✅ LittleFS persistent storage
✅ WiFiManager captive portal
✅ OTA firmware updates
✅ Factory reset (web + optional button)
✅ Custom hostname support
✅ mDNS responder
✅ UTF-8 extended ASCII (€, ñ, ü, etc.)
✅ Config backup/restore

## ESP8266 Memory Management

ESP8266 has 80KB RAM. Current static usage ~70% (~57KB BSS), leaving ~23KB heap.

**Default flags on all ESP8266 builds** (to protect WiFiManager heap):
- `-DDISABLE_WEATHER_FEATURE` — weather feature fully excluded
- `-DDISABLE_HA_CLOCK_ADVANCED` — skips 7 verbose HA clock discovery entities

**Buffer reductions on ESP8266**:
- `haLastMessage`: 256 bytes (vs 1024 on ESP32)
- `MSG_SIZE`: 1024 bytes (vs 3072 on ESP32)

**Key rule**: Do NOT remove `DISABLE_HA_CLOCK_ADVANCED` from ESP8266 builds without testing — it risks intermittent WiFiManager portal failures (missing form fields).

## Known Issues

### Development Environment
- **PlatformIO CLI**: Not in global PATH — use `~/.platformio/penv/bin/platformio`
- **Serial Monitor**: Must be closed before uploading firmware

### Hardware Limitations
- **CS Pin**: Default is D4 for D1 Mini, D8 for NodeMCU (ESP8266) or GPIO 5 (ESP32)
- **Power**: Multiple LED modules may require external 5V power supply
- **Flash Button**: Optional feature (ESP8266 only, disabled by default)
- **ESP32 EasyButton**: Excluded from ESP32 builds when `ENABLE_FLASH_BUTTON=0`

### Configuration
- **No WiFi Restart**: Changing WiFi credentials requires manual reconnection (by design)
- **MQTT Buffer**: 1024 bytes (`MQTT_MAX_PACKET_SIZE=1024` in common build flags)

## Development Workflow

### Building Firmware

**PlatformIO CLI:**
```bash
# ESP8266
~/.platformio/penv/bin/platformio run --target upload --environment d1_mini
# ESP32
~/.platformio/penv/bin/platformio run --target upload --environment esp32dev
# Build only (no upload)
~/.platformio/penv/bin/platformio run --environment d1_mini_4m
```

### Release Process
1. Update `version` in `platformio.ini` `[common]` section
2. Commit your changes
3. Run `./release.sh "notes"` — commits, tags, updates CHANGELOG.md, pushes to both remotes
4. Use `--dry-run` to preview without making changes
5. Use `--force` to recreate an existing tag

### Testing

**HTTP API Testing:**
```bash
curl --user admin:msgboard -X GET -G 'http://192.168.1.89/arg' \
  --data-urlencode "MSG=Test" --data-urlencode "REP=3"

curl --user admin:msgboard -X POST http://192.168.1.89/api \
  -H 'Content-Type: application/json' -d '{"MSG":"API Test"}'
```

**MQTT Testing:**
```bash
mosquitto_pub -h broker.local -t "rdadotmatrix/generic" -m "MQTT Test"
mosquitto_pub -h broker.local -t "rdadotmatrix/generic/json" \
  -m '{"MSG":"JSON Test","REP":"5","BRI":"10"}'
```

## Adding New Configuration

When adding new configuration parameters, follow this checklist:

1. **Define**: Add field to relevant struct in `include/config.h`
2. **Declare**: Add runtime globals in `include/globals.h` and `src/globals.cpp`
3. **Persistence**: Update `load/saveXXXConfiguration()` in `config_manager.cpp`
4. **Initialize**: Update `initXXXStoreConfig()` with defaults
5. **Web UI**: Add input field in `src/web_pages_*.cpp`
6. **Web Data**: Update XML generator in `src/web_data.cpp`
7. **Web Server**: Update form handler in `src/web_server.cpp`
8. **Backup**: Update `/exportconfig` and `/importconfig` in `src/web_server.cpp`
9. **HA Discovery**: Add entity in `src/mqtt_discovery_*.cpp` (gate with `#ifndef DISABLE_HA_CLOCK_ADVANCED` for ESP8266 if it's a verbose/optional clock entity)

## Important Code Locations

### Message Processing
- `functions.cpp:onMessageCallHttp()` - HTTP parameter parsing
- `functions.cpp:onMessageCallJson()` - JSON message parsing
- `functions.cpp:scrollTextParola()` - Display control loop
- `utf8_utils.cpp:utf8Ascii()` - UTF-8 character conversion

### Clock
- `functions.cpp:displayClock()` - Clock rendering, font selection, AM/PM, date alternation
- `functions.cpp:isMatrixLightFace()` - Font face check helper
- `globals.cpp:clockAlternateState` - 3-state int (0=time, 1=day-of-week, 2=date)

### MQTT Discovery
- `mqtt_discovery_sensors.cpp:publishAllClockStates()` - Publishes all clock state topics
- `mqtt_discovery_clock.cpp:publishClockDiscoveries()` - Clock HA discovery (7 entities gated on ESP8266)

### Configuration
- `config_manager.cpp` - All load/save/init functions
- `main.cpp` lines 73-164 - Setup sequence with deferred config loading
