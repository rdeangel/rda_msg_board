# GEMINI.md - Project Context & Rules

## Project Description
ESP8266/ESP32-based Message Board firmware (`rda_msg_board`). Controls MAX7219 LED matrices to display scrolling messages. Features a web interface, HTTP REST API, MQTT support, clock with multiple font faces, recurrent alarms, sleep mode, and Home Assistant MQTT Discovery integration. Originally developed for Arduino IDE, migrated to PlatformIO.

**Current version**: v1.2.0

## Architecture Overview

### Configuration System (LittleFS flash)
1. **Web Credentials** (`/web_config.json`) - HTTP Basic Auth username/password, device hostname
2. **MQTT Settings** (`/mqtt_config.json`) - Enable/disable, server, port, auth, TLS, topic prefix, HA discovery toggle, MQTT messages toggle
3. **Message Defaults** (`/defaults_config.json`) - Repeat, buzzer count, scroll delay, brightness, ASCII conversion
4. **General Settings** (`/general.config`) - Global buzzer enable, brightness override (enable + value)
5. **Clock Settings** (`/clock.config`) - NTP server, POSIX timezone, brightness, transition effect/speed/delay, randomize, resync interval, date format, date alternation (enable + seconds), custom date format, clock face, AM/PM mode
6. **Timer Settings** (`/timer.config`) - Timer vs stopwatch mode, duration, brightness, buzzer, auto-repeat, alert chirp
7. **Sleep Mode** (`/sleep_mode.config`) - Scheduled display on/off, mute-only mode, weekend override times
8. **Alarm** (`/alarm.config`) - Recurrent alarm: enable, interval, chirp name, disable-weekends

**Deferred loading (ESP8266 only)**: Only defaults + general configs load before `wm.autoConnect()` to preserve heap for the WiFiManager captive portal page. All other configs load after WiFiManager completes.

### Config Objects (in `include/config.h`)
- `webConfig`: usernameWebHolder, passwordWebHolder, hostnameWebHolder
- `mqttConfig`: server, port, prefix, auth, TLS, HA discovery, MQTT messages enable
- `defaultsConfig`: REP, BUZ, DEL, BRI, ASC defaults
- `generalConfig`: buzzerEnable, brightnessOverrideEnable, brightnessOverrideValue
- `clockConfig`: enabled, ntpServer, tzString, brightness, transitionDelayMs, transitionEffect, randomizeTransition, transitionSpeed, resyncIntervalHours, dateFormat, dateAlternate, dateAlternateSeconds, customDateFormat, **clockFace**, **clockAmPm**
- `timerConfig`: enabled, mode, durationSeconds, brightness, alertBuzzer, alertChirp, autoRepeat
- `sleepModeConfig`: enabled, onTime, offTime, muteOnly, weekendEnabled, weekendOnTime, weekendOffTime
- `recurrentAlarmConfig`: enabled, interval, chirpName, disableWeekends

### Core Components
- **WiFiManager**: Captive portal for initial WiFi setup (AP: `RDA-MSG-XXXXXX`, Password: `wifi-setup`)
- **Web Server**: Platform-abstracted HTTP server on port 80, Basic Auth
  - ESP8266: `ESP8266WebServer` / ESP32: `WebServer`
- **PubSubClient**: MQTT client, `MQTT_MAX_PACKET_SIZE=1024`
- **MD_Parola + MD_MAX72xx**: LED matrix scrolling text (hardware SPI)
- **Fonts**: Built-in Parola font (`nullptr`), `MatrixLight8Font` (8px, PROGMEM), `MatrixLight6Font` (6px, PROGMEM)
- **LittleFS**: Flash filesystem — compatible on both platforms
- **EasyButton**: Optional FLASH button (ESP8266 only, disabled by default via `ENABLE_FLASH_BUTTON=0`)

## Feature Flags (define to disable)

| Flag | Default ESP8266 | Default ESP32 | Effect |
|------|----------------|---------------|--------|
| `DISABLE_TIMER_FEATURE` | OFF (timer enabled) | OFF | Exclude timer/stopwatch |
| `DISABLE_WEATHER_FEATURE` | **ON** | OFF | Exclude weather |
| `DISABLE_SLEEP_MODE_FEATURE` | OFF | OFF | Exclude sleep mode |
| `DISABLE_ALARM_FEATURE` | OFF | OFF | Exclude recurrent alarm |
| `DISABLE_HA_CLOCK_ADVANCED` | **ON** | OFF | Skip 7 verbose HA clock entities |

`DISABLE_HA_CLOCK_ADVANCED` gates these discovery entities on ESP8266: `clock_transition_speed`, `clock_transition_effect`, `clock_randomize`, `clock_ntp_server`, `clock_custom_tz`, `clock_transition_delay`, `clock_resync_interval`. Essential entities (enable, brightness, face, date format, AM/PM, date alternate) are always published.

## Clock Features

### Clock Faces
- `DEFAULT` — built-in Parola font (`P.setFont(nullptr)`)
- `MATRIX_LIGHT` — MatrixLight8Font (compact 8px, PROGMEM)
- `MATRIX_LIGHT_6` — MatrixLight6Font (compact 6px, PROGMEM)

Font files: `include/MatrixLight8_font.h`, `include/MatrixLight6_font.h` (generated from BDF source via `tools/bdf_to_parola.py`)

### AM/PM Mode
- `clockConfig.clockAmPm = "on"` enables 12-hour display with AM/PM suffix
- Enforces display-width constraints on 4-module builds (TIME_SECONDS disabled with AM/PM)

### Date Alternation
- `clockConfig.dateAlternate = "on"` enables 3-state rotation
- `clockAlternateState` int (0=time, 1=day-of-week, 2=date) — replaces old `showingDate` bool
- Interval configured via `clockConfig.dateAlternateSeconds`
- Matrix Light fonts support the full 3-step cycle; DEFAULT font may skip day-of-week step depending on module count

## HTTP Endpoints

All require HTTP Basic Auth (`admin`/`msgboard` by default).

### Message
- `GET /` — Main web interface
- `GET /arg` — Send message via URL params (MSG, REP, BUZ, DEL, BRI, ASC)
- `POST /api` — Send message via JSON body

### Configuration
- `GET /deviceconfig` — Device credentials page
- `POST /changecredentials` — Update username/password/hostname
- `GET /mqttconfig` — MQTT config page
- `POST /applymqttconfig` — Update MQTT settings
- `GET /generalvars` — XML for general settings modal
- `POST /savegeneral` — Save general settings
- `GET /clockvars` — XML for clock config modal
- `POST /saveclock` — Save clock settings
- `GET /exportconfig` — Export complete config as JSON
- `POST /importconfig` — Import config from JSON backup

### Defaults
- `GET /setdefault?type=REP&value=10` — Set individual default (REP/BUZ/DEL/BRI)
- `GET /resetdefaults` — Reset all defaults to hardcoded values

### System
- `GET /system` — System/OTA page
- `GET /reboot` — Reboot device
- `GET /factoryreset` — Factory reset
- `POST /submitupdate` — OTA firmware upload

### AJAX Data
- `GET /mainpagevars` — XML for main page
- `GET /changecredvars` — XML for credentials page
- `GET /mqttpagevars` — XML for MQTT page
- `GET /updatevars` — XML for system page

## MQTT Topic Pattern

For topic prefix: `rdadotmatrix/generic`, device: `RDA-MSG-ABCDEF`:

**Subscribed:**
```
rdadotmatrix                    # Root - plain
rdadotmatrix/json               # Root - JSON
rdadotmatrix/generic            # Prefix - plain
rdadotmatrix/generic/json       # Prefix - JSON
RDA-MSG-ABCDEF                  # Device - plain
RDA-MSG-ABCDEF/json             # Device - JSON
```

Wildcards `#` and `+` supported in topic prefix.

**Published:**
```
RDA-MSG-ABCDEF/status           # "Connected" on connect
RDA-MSG-ABCDEF/ha/*/state       # HA entity states
```

**Plain vs JSON**: Topics NOT ending in `/json` display message with default parameters. Topics ending in `/json` parse MSG, REP, BUZ, DEL, BRI, ASC, ALERTCHIRP fields.

## Home Assistant Discovery

Published on MQTT connect. Key entities:

| Entity | Type | Notes |
|--------|------|-------|
| Message | text | Main message input |
| Repeat / Buzzer / Speed | number | Message parameters |
| Brightness | light | LED brightness |
| Display Power | switch | On/off |
| Buzzer Enable | switch | Global buzzer |
| Clock Enable | switch | |
| Clock Brightness | number | |
| Clock Face | select | DEFAULT / MATRIX_LIGHT / MATRIX_LIGHT_6 |
| Clock Date Format | select | Per MAX_DEVICES count |
| Alternate Date | switch | Date alternation toggle |
| 12-hour AM/PM | switch | |
| *(advanced — ESP32 only by default)* | | transition speed/effect, NTP server, timezone, delay, resync |
| Timer entities | various | Gated by `DISABLE_TIMER_FEATURE` |
| Alarm entities | various | Gated by `DISABLE_ALARM_FEATURE` |
| Sleep entities | various | Gated by `DISABLE_SLEEP_MODE_FEATURE` |

## Code Structure

### Key Files
- `src/main.cpp` — Entry point, deferred config loading, WiFiManager, main loop
- `src/web_server.cpp` — HTTP routing and request handling
- `src/config_manager.cpp` — All load/save/init functions for every config type
- `src/web_data.cpp` — XML response generation for AJAX
- `src/web_pages_*.cpp` — Embedded HTML/CSS/JS (main, config, status)
- `src/mqtt.cpp` — MQTT connection, subscriptions, callback dispatch
- `src/mqtt_discovery_core.cpp` — Base topics, device info, availability
- `src/mqtt_discovery_sensors.cpp` — Main entities + `publishAllClockStates()`
- `src/mqtt_discovery_clock.cpp` — Clock + recurrent alarm entities
- `src/mqtt_discovery_timer.cpp` — Timer/stopwatch entities
- `src/mqtt_discovery_sleep.cpp` — Sleep mode entities
- `src/functions.cpp` — Message processing, clock rendering, NTP, UTF-8
- `src/globals.cpp` + `include/globals.h` — All global variable definitions
- `include/config.h` — Hardware pins, buffer sizes, feature flags, config structs
- `include/MatrixLight8_font.h` + `include/MatrixLight6_font.h` — PROGMEM font data

### Config Management Pattern
Each config type follows:
1. `loadXXXConfiguration()` — reads JSON from LittleFS
2. `saveXXXConfiguration()` — writes JSON to LittleFS
3. `initXXXStoreConfig()` — loads or creates defaults on boot
4. Runtime variables updated from struct on load

### Adding New Configuration — Checklist
1. Add field to struct in `include/config.h`
2. Add runtime global in `include/globals.h` + `src/globals.cpp`
3. Update `load/saveXXXConfiguration()` in `config_manager.cpp`
4. Update `initXXXStoreConfig()` with default
5. Add form field in `src/web_pages_*.cpp`
6. Update XML in `src/web_data.cpp`
7. Update handler in `src/web_server.cpp`
8. **CRITICAL**: Update `/exportconfig` and `/importconfig` in `src/web_server.cpp`
9. Add HA discovery entity in `src/mqtt_discovery_*.cpp` (use `#ifndef DISABLE_HA_CLOCK_ADVANCED` guard for optional clock entities)

## Hardware Configuration

**ESP8266 defaults** (`include/config.h`):
```cpp
#define MAX_DEVICES 4
#define CLK_PIN  D5   // GPIO 14 - SPI Clock
#define DATA_PIN D7   // GPIO 13 - SPI MOSI
#define CS_PIN   D8   // GPIO 15 - CS (D4 for D1 Mini)
#define BUZZER_PIN D1 // GPIO 5
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
```

**ESP32 defaults:**
```cpp
#define MAX_DEVICES 4
#define CLK_PIN  18   // VSPI CLK
#define DATA_PIN 23   // VSPI MOSI
#define CS_PIN    5   // VSPI CS
#define BUZZER_PIN 4
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
```

Override via `platformio.ini` `build_flags`.

## ESP8266 Memory

- Static RAM: ~70% (~57KB / 81KB), ~23KB heap available
- `MSG_SIZE` = 1024 bytes (ESP8266) vs 3072 (ESP32)
- `haLastMessage` = 256 bytes (ESP8266) vs MSG_SIZE (ESP32)
- `DISABLE_HA_CLOCK_ADVANCED` and `DISABLE_WEATHER_FEATURE` are mandatory on ESP8266 builds

## Build Environments

| Environment | Board | MAX_DEVICES |
|-------------|-------|-------------|
| `nodemcu_4m` | NodeMCU 1.0 | 4 |
| `nodemcu_8m` | NodeMCU 1.0 | 8 |
| `d1_mini_4m` | D1 Mini | 4 |
| `d1_mini_8m` | D1 Mini | 8 |
| `esp32_4m` | ESP32 DevKit | 4 |
| `esp32_8m` | ESP32 DevKit | 8 |

PlatformIO CLI path on Linux: `~/.platformio/penv/bin/platformio`

## Release Process

1. Update `version` in `platformio.ini` `[common]` section
2. Run `./release.sh "notes"` — commits, tags, updates CHANGELOG.md, pushes to `origin` (Forgejo) + `github`
3. GitHub Actions builds all 6 environments and creates the release with firmware artifacts

Flags: `--dry-run` (preview only, no commits), `--force` (recreate existing tag), `--no-changelog`

## Default Credentials

```
WiFi AP SSID:     RDA-MSG-XXXXXX
WiFi AP Password: wifi-setup
Web Username:     admin
Web Password:     msgboard
AP IP:            192.168.4.1
```
