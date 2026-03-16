# 📓 Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

<!-- releases -->

## [v1.4.1] - 2026-03-16

### ✨ New Features
- feat(ota): freeze display during firmware updates - [4ce30b8](https://github.com/rdeangel/rda_msg_board/commit/4ce30b8)

## [v1.4.0] - 2026-03-16

### ✨ New Features
- feat(esp32): implement non-blocking operations via FreeRTOS tasks - [c63cd7d](https://github.com/rdeangel/rda_msg_board/commit/c63cd7d)
- feat(crypto): add proper currency symbols for GBP, EUR, and JPY - [9c9d9d0](https://github.com/rdeangel/rda_msg_board/commit/9c9d9d0)
- feat(clock): add separate clock display duration for date alternation - [60fd546](https://github.com/rdeangel/rda_msg_board/commit/60fd546)
- feat(crypto): add CoinPaprika crypto price ticker for ESP32 - [12de8bc](https://github.com/rdeangel/rda_msg_board/commit/12de8bc)
- feat: add force repetitions feature to force continue repetition of messages untill the last repeat. - [16cbf02](https://github.com/rdeangel/rda_msg_board/commit/16cbf02)

### 🐛 Bug Fixes
- fix(font): add fixed-width clock font to prevent '1' shift with default font. - [2cd23f9](https://github.com/rdeangel/rda_msg_board/commit/2cd23f9)
- fix(font): pad digit '1' to 3 columns in 6X and 8X fonts - [a5264e2](https://github.com/rdeangel/rda_msg_board/commit/a5264e2)

### 📚 Documentation
- docs: update documentation for v1.4.0 features - [28f1880](https://github.com/rdeangel/rda_msg_board/commit/28f1880)

## [v1.3.1] - 2026-03-09

### 🐛 Bug Fixes
- fix(utf8): add mappings for smart quotes, dashes, and ellipsis - [ef148a9](https://github.com/rdeangel/rda_msg_board/commit/ef148a9)

## [v1.3.0] - 2026-03-08

### ✨ New Features
- feat(clock): add four new bitmap clock faces with extended spacing - [556a983](https://github.com/rdeangel/rda_msg_board/commit/556a983)

## [v1.2.0] - 2026-03-08

### ✨ New Features
- feat: Add MQTT control for clock face selection and include new MatrixLight fonts with a BDF conversion tool - [0f8df7d](https://github.com/rdeangel/rda_msg_board/commit/0f8df7d)
- feat(clock): add AM/PM mode, date alternation, and Matrix Light 6px font - [87f0968](https://github.com/rdeangel/rda_msg_board/commit/87f0968)

### 🐛 Bug Fixes
- fix(ESP8266): reduce heap pressure to stabilize WiFiManager captive portal - [ee0f1c2](https://github.com/rdeangel/rda_msg_board/commit/ee0f1c2)
- fix(release.sh): prevent commits during dry-run and validate remote existence before push - [df76a86](https://github.com/rdeangel/rda_msg_board/commit/df76a86)

### 📚 Documentation
- docs: update documentation for v1.2.0 release and architecture - [88d79d3](https://github.com/rdeangel/rda_msg_board/commit/88d79d3)

## [v1.1.0] - 2026-03-07

### ✨ New Features
- feat(alarm): add disable weekends option for recurrent alarms - [e4987cc](https://github.com/rdeangel/rda_msg_board/commit/e4987cc)

### 🐛 Bug Fixes
- fix(release): support optional scopes in conventional commit regex and remove default message - [c01988b](https://github.com/rdeangel/rda_msg_board/commit/c01988b)

### 📚 Documentation
- docs: enhance README with images and documentation sections - [5258daa](https://github.com/rdeangel/rda_msg_board/commit/5258daa)
- docs: modified skill instruction and added fixed some README.md text - [d99936c](https://github.com/rdeangel/rda_msg_board/commit/d99936c)

## [1.0.0] - Initial Release

This is the initial V1 release of the RDA MSG Board firmware, which has been entirely refactored from the original Arduino IDE implementation to PlatformIO for a robust dependency-managed project structure.

### ✨ Added

**🖥️ Core Capabilities**
- 🌍 **Multi-Platform Support**: Pre-configured build environments targeting both ESP8266 (NodeMCU, Wemos D1 Mini) and ESP32 DevKit boards.
- 🚥 **MAX7219 Dot Matrix Support**: Pre-built environments for 4 and 8 module matrices, scalable to larger daisy chains.
- 📱 **Modern Web Interface**: Responsive, AJAX-driven web GUI protected by HTTP Basic Auth for configuring device parameters, WiFi, MQTT, and casting messages.
- 🔌 **RESTful API**: Manage the board programmatically via an HTTP REST API (`GET /arg` utilizing URL-encoded parameters, or `POST /api` with a JSON payload).
- 📡 **MQTT Integration**: Reliable PubSub messaging loop with TLS/SSL encryption support (ESP32 only), customizable topic prefixes, wildcard topic support, and connection state publishing.
- 🏠 **Home Assistant Auto-Discovery**: Zero-configuration integration via Home Assistant MQTT discovery allowing for transparent entity mapping of all device sensors, text inputs, and toggles.
- 🔍 **mDNS Network Resolution**: Local network discovery out of the box (e.g., `http://RDA-MSG-ABCDEF.local`), bypassing the need to know the assigned IP address.

**💬 Messaging & Visual Display**
- 🔣 **Extended UTF-8 Support**: Robust extended ASCII logic mapping for seamless rendering of international language characters and unique symbols (e.g., €, ñ, ü, à, ©, °).
- ⚙️ **Dynamic Configurable Parameters**: Customize scroll repeats, scroll speeds, display brightness, and buzzer alerts on-the-fly per message.
- 💾 **Customizable Default Fallbacks**: Manage default messaging fallback settings directly from the web GUI utilizing persistent non-volatile storage.

**🔊 Audio Alerts & Timers**
- 🔔 **Hardware Buzzer Integration**: Piezo buzzer support initialized over dynamically configurable hardware pins.
- 🎵 **Extensive Chirp Audio Library**: Over 20 pre-defined musical alerts and chirps (Gentle Dawn, Urgent, Mario Bros, For Elise, Theme Songs, etc.) available for timers and incoming messages.
- ⏱️ **Timers & Stopwatches**: Configurable hardware-driven countdown timers and up-counting stopwatches with native audible alarms.

**⏰ Clock & Automations**
- 🕛 **Animated LED Clock**: A highly configurable transitioning LED clock featuring date overlap, precise POSIX timezone offset mapping, and dedicated brightness controls.
- 😴 **Intelligent Sleep Scheduling**: Schedule display power-saving dimming and complete screen blackouts specifically tailored individually for weekday and weekend time windows.

**🛠️ Configuration & Maintenance**
- 📶 **WiFi Captive Portal**: Secure out-of-the-box bootstrapping utilizing an automatic WiFi AP configuration network (`RDA-MSG-XXXXXX`).
- 🗄️ **LittleFS Storage Engine**: Secure and scalable configuration saves capitalizing on modern LittleFS paradigms natively parsed by ArduinoJson v7.
- 🔄 **Backup & Restore Pipelines**: Feature-complete ecosystem backups leveraging OTA JSON Config Export/Import pipelines through the internal HTTP Server.
- ☁️ **Over-The-Air (OTA) Updating**: Seamless `.bin` firmware and filesystem upgrades managed directly through the onboard web system dashboard.
- ♻️ **Factory Restorations**: Reliable soft "Wipe Config" factory resets accessible via the web, or physical hardware Flash-button triggered resets (ESP8266 natively supported).

### 🔄 Changed
- 🏗️ Complete architecture overhaul migrating to isolated modules (`web_server.cpp`, `config_manager.cpp`, `mqtt_discovery.cpp`, etc.).
- 📦 Exchanged deprecated EEPROM configurations for reliable `LittleFS` `.json` and `.config` object persistence tracking.
- ⚡ Replaced outdated JSON handling utilizing `ArduinoJson v7` yielding faster performance and stabilized serialization limits.

### 🐛 Fixed
- 🛡️ Stabilized limited execution heap sizing on ESP8266 platforms by gating memory-heavy tasks (such as OpenWeatherMap JSON payloads) behind selective compile-time flags.
- 🧵 Mitigated blocking scroll behaviors rendering cross-layer `delay()` logic non-blocking across the main execution loop.
