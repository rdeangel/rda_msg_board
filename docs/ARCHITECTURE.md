# Architecture Documentation

## Overview
The `rda_msg_board` firmware controls MAX7219 LED matrices, displaying MQTT messages and providing a web configuration interface. The architecture is modular, separating core logic, network communication, and hardware control.

## Module Structure

### 1. Web Interface Layer
Handles HTTP requests, configuration management, and dynamic page generation.

- **`web_server`** (`.cpp/.h`): Main Controller
  - Sets up HTTP routes (`/`, `/api`, `/saveconfig`)
  - Handles authentication
  - Dispatches requests to logic handlers
- **`config_manager`** (`.cpp/.h`): Persistence Layer
  - Loads/Saves JSON configuration files from LittleFS
  - Manages configuration structs (`webConfig`, `mqttConfig`, etc.)
  - Handles file system initialization
- **`web_data`** (`.cpp/.h`): View/Data Layer
  - Generates XML/JSON responses for AJAX requests
  - Formats data for web consumers
- **`web_pages_*`** (`.cpp/.h`):
  - Stores HTML/JS/CSS as raw string literals
  - `web_pages_config`: Configuration modals
  - `web_pages_main`: Main dashboard

### 2. MQTT Layer
Handles communication with the MQTT broker and Home Assistant discovery.

- **`mqtt`** (`.cpp/.h`): Client Wrapper
  - Manages connection/reconnection logic
  - SSL/TLS configuration (ESP32)
  - Topic subscription handling
- **`mqtt_discovery`** (`.cpp/.h`): Discovery Facade
  - Aggregates all discovery modules
- **`mqtt_discovery_core`** (`.cpp/.h`):
  - Base topic construction (`rdadotmatrix/...`)
  - Device registry info
- **`mqtt_discovery_sensors`** (`.cpp/.h`):
  - Discovery logic for standard entities (Light, Text, Switch)
  - State publishing for telemetry
  - Command handling for standard controls
- **`mqtt_discovery_clock`** (`.cpp/.h`):
  - Discovery logic for Clock entities (Timezone, Alarm, etc.)
  - Clock-specific command handling

### 3. Core Logic & Hardware
- **`functions`** (`.cpp/.h`): "Do-er" Layer
  - Display control (MAX7219)
  - Time synchronization (NTP)
  - Message scrolling logic
- **`utf8_utils`** (`.cpp/.h`): Character Encoding
  - UTF-8 to extended ASCII conversion
  - Special character handling (€, ñ, ü, etc.)
- **`buzzer_utils`** (`.cpp/.h`): Audio Feedback
  - System event sounds (startup, WiFi mode, MQTT events)
  - Buzzer pattern control
- **`globals`** (`.cpp/.h`): State
  - Holds runtime state variables
  - Defines global configuration objects

## Configuration Flow
1. **Boot**: `main.cpp` calls `init*StoreConfig()` in `config_manager`.
2. **Load**: `config_manager` reads JSON from LittleFS.
3. **Apply**: Values are loaded into global config structs.
4. **Runtime**: Modules read from global structs.
5. **Update**: Web UI POSTs to `web_server` -> calls `config_manager::saveConfiguration()` -> Updates JSON & Global Structs.

## Directory Structure
```
src/
├── config_manager.cpp      # persistence
├── web_data.cpp            # view models
├── web_server.cpp          # controllers
├── mqtt_discovery_*.cpp    # home assistant integration
├── functions.cpp           # hardware/logic
├── utf8_utils.cpp          # character encoding
├── buzzer_utils.cpp        # audio feedback
└── main.cpp                # entry point

include/
├── config.h                # macros, pin definitions
└── *.h                     # module interfaces
```
