# 📓 Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).


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
