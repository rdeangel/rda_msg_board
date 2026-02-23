# AGENT.md - Agent Notes & Memory

## Current Context
- **Status**: Active development (migrated from Arduino IDE to PlatformIO)
- **Version**: v1.0.0 (semantic versioning, in version string)
- **Target Hardware**:
  - **ESP8266**: NodeMCU 1.0, WeMos D1 Mini
  - **ESP32**: ESP32 DevKit v1
- **Recent Migration**: Arduino IDE → PlatformIO for better dependency management
- **Multi-Platform**: Supports both ESP8266 and ESP32 with unified codebase

## Project Summary

ESP8266/ESP32-based LED matrix message board with web interface, HTTP API, and MQTT support. Displays scrolling messages with configurable parameters. Features persistent configuration, OTA updates, and Home Assistant integration. Uses platform-specific abstractions for seamless cross-platform compatibility.

## Recent Changes

### Major Refactoring (January 2026)
- Refactored "System" page to move "Wipe Config" and "Reboot" buttons to common area
- Added JavaScript confirmation modals for "Wipe Config" and "Reboot"
- Added `/test_build` workflow for easy firmware uploading
- Created comprehensive documentation (README.md, GEMINI.md updates)

### New Features
- **Config Export/Import** (`/exportconfig`, `/importconfig`) - Backup/restore complete device configuration
- **General Settings** - Global buzzer toggle and device-wide parameters
- **Advanced Clock Settings** - POSIX Timezone support, date formats, and transition effects
- **Custom Defaults Management** (`/setdefault`, `/resetdefaults`) - Save custom default values for message parameters
- **Improved JSON Handling** - Updated to ArduinoJson v7 for better performance
- **Better Debug Output** - Organized serial output for easier troubleshooting
- **Multi-Environment Builds** - Support for multiple board types in single project

## Architecture Overview

### Four Configuration Tiers
1. **Web Credentials** - HTTP auth & hostname (`/web_config.json`)
2. **MQTT Settings** - Server, topics, authentication (`/mqtt_config.json`)
3. **Message Defaults** - Repeat, buzzer count, speed, brightness (`/defaults_config.json`)
4. **General Settings** - Global buzzer enable, etc. (`/general.config`)

### Key Components
- **Web Server**: Platform-abstracted server on port 80, 14 HTTP endpoints
  - ESP8266: ESP8266WebServer
  - ESP32: WebServer
- **MQTT Client**: PubSubClient with 2048-byte buffer, flexible topic subscriptions
- **Display Driver**: MD_Parola for scrolling effects, UTF-8 conversion support (hardware SPI)
- **Storage**: LittleFS for persistent config (compatible on both platforms)
- **WiFi**: WiFiManager for captive portal setup (multi-platform v2.0.17)

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
✅ GET `/exportconfig` - Export configuration  
✅ POST `/importconfig` - Import configuration  
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

### Message Parameters
✅ MSG - Message text  
✅ REP - Repeat count (default: 10, 0 = infinite)  
✅ BUZ - Buzzer chirps (default: 10)  
✅ DEL - Scroll delay ms (default: 35)  
✅ BRI - Brightness 0-15 (default: 7)  
✅ ASC - UTF-8 conversion (default: 1/on)  
✅ ALERTCHIRP - Alert chirp pattern (default: Fast Beep)  

### System Features
✅ LittleFS persistent storage  
✅ WiFiManager captive portal  
✅ OTA firmware updates  
✅ Factory reset (web + optional button)  
✅ Custom hostname support  
✅ mDNS responder  
✅ UTF-8 extended ASCII (€, ñ, ü, etc.)  
✅ Config backup/restore  

## Known Issues

### Development Environment
- **PlatformIO CLI**: Not in global PATH, must use full path or VS Code extension
- **Large HTML Strings**: `src/web_pages.cpp` contains large C-style raw string literals, slightly hard to edit
- **Serial Monitor**: Must be closed before uploading firmware (common ESP8266 issue)

### Hardware Limitations
- **CS Pin**: Default is D4 for D1 Mini, D8 for NodeMCU (ESP8266) or GPIO 5 (ESP32)
- **Power**: Multiple LED modules may require external 5V power supply
- **Flash Button**: Optional feature (currently ESP8266 only, disabled by default)
- **ESP32 EasyButton**: Library excluded from ESP32 builds when ENABLE_FLASH_BUTTON=0 due to dependency issues

### Configuration
- **No WiFi Restart**: Changing WiFi credentials requires manual reconnection (by design)
- **MQTT Buffer**: Limited to 2048 bytes for messages (configurable but memory-constrained)

## Development Workflow

### Building Firmware

**Using Workflow (Recommended):**
```
/test_build
```

**Using PlatformIO CLI:**
```bash
# Full path required (not in global PATH)
# ESP8266
platformio run --target upload --environment d1_mini
platformio run --target upload --environment nodemcuv2
# ESP32
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

**Using VS Code:**
1. Select environment in PlatformIO toolbar
   - ESP8266: nodemcuv2, d1_mini
   - ESP32: esp32dev
2. Click checkmark to build
3. Click arrow to upload

### Testing

**Manual Web Interface Testing:**
1. Connect to device WiFi or join same network
2. Browse to IP or mDNS hostname
3. Test all pages: main, device config, MQTT config, system
4. Verify message display with different parameters

**HTTP API Testing:**
```bash
# Test GET endpoint
curl --user admin:msgboard -X GET -G 'http://192.168.1.89/arg' \
  --data-urlencode "MSG=Test" --data-urlencode "REP=3"

# Test JSON POST
curl --user admin:msgboard -X POST http://192.168.1.89/api \
  -H 'Content-Type: application/json' -d '{"MSG":"API Test"}'

# Test config export
curl --user admin:msgboard http://192.168.1.89/exportconfig > backup.json
```

**MQTT Testing:**
```bash
# Plain message
mosquitto_pub -h broker.local -t "rdadotmatrix/generic" -m "MQTT Test"

# JSON message
mosquitto_pub -h broker.local -t "rdadotmatrix/generic/json" \
  -m '{"MSG":"JSON Test","REP":"5","BRI":"10"}'
```

### Configuration Files Location

LittleFS filesystem (ESP8266 or ESP32 flash):
```
/web_config.json          # Username, password, hostname
/mqtt_config.json         # MQTT server, topics, authentication
/defaults_config.json     # Custom message parameter defaults
/general.config           # General settings (e.g. global buzzer)
/clock.config             # Clock configuration data
```

**Platform-Specific Storage:**
- ESP8266: Uses LittleFS partition in flash
- ESP32: Uses LittleFS partition (compatible implementation)

### Debug Output

Enable in `include/config.h`:
```cpp
#define DEBUG 1           // Serial output at 115200 baud
#define PRINT_CALLBACK 0  // Display callback debugging
```

## Migration Notes (Arduino IDE → PlatformIO)

### File Structure Changes
- `ESP8266_MAX7219_RDA_MSG_BOARD.ino` → `src/main.cpp`
- `01_Shared.h` → `include/config.h`
- Separate `.cpp`/`.h` files for better organization
- `platformio.ini` for build configuration

### Library Management
- Arduino IDE: Manual library installation
- PlatformIO: Auto-managed in `platformio.ini` `lib_deps`

### Build Configuration
- Arduino IDE: Board selected in Tools menu
- PlatformIO: Multiple environments in single `platformio.ini`
- Version strings: Build flags per environment

### What Stayed the Same
✅ All HTTP endpoints  
✅ MQTT topic patterns  
✅ Web interface HTML/CSS/JS  
✅ Configuration file format  
✅ Pin assignments (configurable)  
✅ WiFiManager setup flow  

## Future Enhancement Possibilities

### Potential Improvements
- [ ] Refactor web pages to use templates instead of raw strings
- [ ] Add more robust MQTT error handling
- [ ] WebSocket support for real-time updates
- [ ] More LED animation effects
- [ ] Multi-language web interface
- [ ] NTP time sync for scheduled messages
- [ ] Message queue/playlist support

### Integration Ideas
- [ ] Telegram bot integration
- [ ] Discord webhook support
- [ ] Google Calendar integration
- [ ] Weather display mode
- [ ] RSS feed ticker (currently via NodeRed)

## Integration Examples Available

✅ **Home Assistant** - REST commands, MQTT, RSS feeds (see README.md)  
✅ **NodeRed** - Complete flow with RSS processing (see `node_red_flow.json`)  
✅ **Command Line** - curl examples for Linux/Windows  
✅ **Browser** - Direct URL access with parameters  

## Important Code Locations

### Message Processing
- `functions.cpp:onMessageCallHttp()` - HTTP parameter parsing
- `functions.cpp:onMessageCallJson()` - JSON message parsing
- `functions.cpp:scrollTextParola()` - Display control loop
- `functions.cpp:utf8Ascii()` - UTF-8 character conversion

### Web Server
- `web_server.cpp:httpWebDirDef()` - All route definitions (lines 437-780)
- `web_pages.cpp` - HTML page content as raw strings

### MQTT
- `mqtt.cpp:mqttCallBack()` - Message received handler
- `mqtt.cpp:mqttConnectProc()` - Topic subscription logic
- `mqtt.cpp:reconnectMqtt()` - Connection management

### Configuration
- `web_server.cpp:initWebStoreConfig()` - Web credentials init
- `web_server.cpp:initDefaultsStoreConfig()` - Defaults init
- `mqtt.cpp:initMqttStoreConfig()` - MQTT config init

## Recommended Practices

### Code Changes
1. Test on single environment first (d1_mini recommended)
2. Use `/test_build` workflow for quick iterations
3. Check serial output for debugging (115200 baud)
4. Export config before testing destructive changes

### Configuration
1. Always change default credentials after first setup
2. Use static DHCP for consistent IP addressing
3. Export config regularly for backup
4. Test MQTT connection before deploying

### Hardware
1. Verify pin assignments match your wiring
2. Use external power for 8+ LED modules
3. Test buzzer volume (can be adjusted in code)
4. Consider disabling buzzer for silent installations

## Common Development Tasks

### Change Pin Assignments

Pin assignments are platform-specific (defined in `include/config.h`):

**ESP8266:**
```cpp
#define CLK_PIN D5     // GPIO 14
#define DATA_PIN D7    // GPIO 13
#define CS_PIN D8      // GPIO 15
#define BUZZER_PIN D1  // GPIO 5
```

**ESP32:**
```cpp
#define CLK_PIN 18     // VSPI CLK
#define DATA_PIN 23    // VSPI MOSI
#define CS_PIN 5       // VSPI CS
#define BUZZER_PIN 4   // General purpose
```

Override via build flags in `platformio.ini`:
```ini
build_flags =
    -DCLK_PIN=18
    -DCS_PIN=5
```

### LED Module Count for your device
Edit `include/config.h`:
```cpp
#define MAX_DEVICES 4
```

### Enable Flash Button Reset
Edit `include/config.h`:
```cpp
#define ENABLE_FLASH_BUTTON 1  // Enable GPIO0 button
```

### Default Credentials
Edit `include/config.h`:
```cpp
#define DEFAULT_WEB_USER "admin"
#define DEFAULT_WEB_PASS "msgboard"
```

### Update Version String
Edit `platformio.ini` build flags:
```ini
build_flags =
    -DVERSION=\"v1.x.x_custom\"
```
