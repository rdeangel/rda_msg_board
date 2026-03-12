#include "mqtt_discovery_core.h"
#include "mqtt_discovery_sensors.h"
#include "mqtt_discovery_clock.h"
#ifndef DISABLE_TIMER_FEATURE
#include "mqtt_discovery_timer.h"
#endif
#ifndef DISABLE_SLEEP_MODE_FEATURE
#include "mqtt_discovery_sleep.h"
#endif
#ifndef DISABLE_WEATHER_FEATURE
#include "mqtt_discovery_weather.h"
#endif
#include "globals.h"
#include "mqtt.h"
#include "functions.h"
#include "web_server.h"

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

// Build base topic for HA discovery (device identifier)
// Uses chipId (MAC-based) instead of clientId (hostname) for stability
void buildHABaseTopic() {
  String deviceId = "rda_msg_" + String(chipId);
  deviceId.toLowerCase();
  deviceId.replace("-", "_");
  snprintf(haBaseTopic, sizeof(haBaseTopic), "%s", deviceId.c_str());
}

// Build discovery topic: homeassistant/<component>/<device_id>/<entity>/config
void buildDiscoveryTopic(char* buffer, size_t bufferSize, const char* component, const char* entity) {
  snprintf(buffer, bufferSize, "%s/%s/%s/%s/config",
           MQTT_DISCOVERY_PREFIX, component, haBaseTopic, entity);
}

// Build command topic: <device>/ha/<entity>/set
void buildCommandTopic(char* buffer, size_t bufferSize, const char* entity) {
  snprintf(buffer, bufferSize, "%s/ha/%s/set", mqttTopicDevice, entity);
}

// Build state topic: <device>/ha/<entity>/state
void buildStateTopic(char* buffer, size_t bufferSize, const char* entity) {
  snprintf(buffer, bufferSize, "%s/ha/%s/state", mqttTopicDevice, entity);
}

// Add device information block to discovery payload
void addDeviceInfo(JsonDocument& doc) {
  JsonObject device = doc["dev"].to<JsonObject>();

  // Use chipId (MAC-based) for stable device identifier
  String deviceId = "rda_msg_" + String(chipId);
  deviceId.toLowerCase();
  deviceId.replace("-", "_");

  device["ids"][0] = deviceId;
  device["name"] = "RDA MSG Board " + String(chipId);
  device["mf"] = "RDA";
  device["mdl"] = "LED Message Board";
  device["sw"] = String(version);

  // Configuration URL - link to web interface
  char configUrl[280];  // Increased to handle max IP length (255 + "http://" + margin)
  snprintf(configUrl, sizeof(configUrl), "http://%s", assignedIP);
  device["cu"] = configUrl;
}

// Add availability information to discovery payload
void addAvailability(JsonDocument& doc) {
  char availTopic[256];
  snprintf(availTopic, sizeof(availTopic), "%s/status", mqttTopicDevice);
  doc["avty_t"] = availTopic;
  doc["pl_avail"] = "online";
  doc["pl_not_avail"] = "offline";
}

// Format seconds into human readable string: "1d 2h 30m 45s"
void formatUptime(unsigned long seconds, char* buffer, size_t size) {
  unsigned long days = seconds / 86400;
  seconds %= 86400;
  unsigned long hours = seconds / 3600;
  seconds %= 3600;
  unsigned long minutes = seconds / 60;
  seconds %= 60;

  if (days > 0) {
    snprintf(buffer, size, "%lud %02luh %02lum %02lus", days, hours, minutes, seconds);
  } else if (hours > 0) {
    snprintf(buffer, size, "%luh %02lum %02lus", hours, minutes, seconds);
  } else if (minutes > 0) {
    snprintf(buffer, size, "%lum %02lus", minutes, seconds);
  } else {
    snprintf(buffer, size, "%lus", seconds);
  }
}

// ============================================================================
// INITIALIZATION
// ============================================================================

// Initialize HA parameters from flash defaults on boot
void initializeHAParameters() {
  if (haParametersInitialized) {
    return;  // Already initialized
  }

  PRINTS("\n=== Initializing HA Parameters from Flash Defaults ===");

  // Initialize message as empty (no default message)
  haLastMessage[0] = '\0';

  // Initialize repeat from flash default
  haLastRepeat = atoi(repeatDefault);
  PRINT("HA Repeat: ", haLastRepeat);

  // Initialize buzzer from flash default
  haLastBuzzer = atoi(buzzerDefault);
  PRINT("HA Buzzer: ", haLastBuzzer);

  // Initialize scroll delay from flash default
  haLastScrollDelay = atoi(scrollDelayDefault);
  PRINT("HA Scroll Delay: ", haLastScrollDelay);

  // Initialize brightness from flash default
  haLastBrightness = atoi(ledBrightnessDefault);
  PRINT("HA Brightness: ", haLastBrightness);

  // Initialize ASCII conversion from flash default
  haLastAsciiConv = atoi(asciiConvDefault);
  PRINT("HA ASCII Conv: ", haLastAsciiConv);

  // Initialize force repetitions (default off)
  haLastForceRep = false;
  PRINTS("HA Force Rep: false");

  haParametersInitialized = true;
  PRINTS("=== HA Parameters Initialized ===\n");
}

// ============================================================================
// DISCOVERY PUBLISHING FUNCTIONS
// ============================================================================

// Main discovery function - publishes all entities
void publishDiscoveryMessages() {
  // Check if HA Discovery is enabled
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0) {
    PRINTS("\n=== Home Assistant Discovery Disabled ===");
    return;
  }

  PRINTS("\n=== Publishing MQTT Discovery Messages ===");
  PRINT("[DEBUG] Free heap before discovery: ", ESP.getFreeHeap());
  PRINTS(" bytes\n");

  // Build base topic once
  buildHABaseTopic();

  // Publish all discovery messages with small delays and yield for ESP8266 memory management
  PRINTS("[DEBUG] Publishing Light Discovery...\n");
  publishLightDiscovery();
  delay(100);
  yield();  // Allow ESP8266 to handle background tasks
  PRINT("[DEBUG] Free heap: ", ESP.getFreeHeap());
  PRINTS(" bytes\n");

  PRINTS("[DEBUG] Publishing Text Discovery...\n");
  publishTextDiscovery();
  delay(100);
  yield();
  PRINT("[DEBUG] Free heap: ", ESP.getFreeHeap());
  PRINTS(" bytes\n");

  PRINTS("[DEBUG] Publishing Number Discoveries...\n");
  publishNumberDiscoveries();
  delay(100);
  yield();
  PRINT("[DEBUG] Free heap: ", ESP.getFreeHeap());
  PRINTS(" bytes\n");

  PRINTS("[DEBUG] Publishing Switch Discoveries...\n");
  publishSwitchDiscoveries();
  delay(100);
  yield();
  PRINT("[DEBUG] Free heap: ", ESP.getFreeHeap());
  PRINTS(" bytes\n");

  PRINTS("[DEBUG] Publishing Buzzer Enable Discovery...\n");
  publishBuzzerEnableDiscovery();
  delay(100);
  yield();
  PRINT("[DEBUG] Free heap: ", ESP.getFreeHeap());
  PRINTS(" bytes\n");

  PRINTS("[DEBUG] Publishing Brightness Override Enable Discovery...\n");
  publishBrightnessOverrideEnableDiscovery();
  delay(100);
  yield();
  PRINT("[DEBUG] Free heap: ", ESP.getFreeHeap());
  PRINTS(" bytes\n");

  PRINTS("[DEBUG] Publishing Brightness Override Value Discovery...\n");
  publishBrightnessOverrideValueDiscovery();
  delay(100);
  yield();
  PRINT("[DEBUG] Free heap: ", ESP.getFreeHeap());
  PRINTS(" bytes\n");

  PRINTS("[DEBUG] Publishing Alert Chirp Discovery...\n");
  publishAlertChirpDiscovery();
  delay(100);
  yield();
  PRINT("[DEBUG] Free heap: ", ESP.getFreeHeap());
  PRINTS(" bytes\n");

  PRINTS("[DEBUG] Publishing Sensor Discoveries...\n");
  publishSensorDiscoveries();
  delay(100);
  yield();
  PRINT("[DEBUG] Free heap: ", ESP.getFreeHeap());
  PRINTS(" bytes\n");

  PRINTS("[DEBUG] Publishing Button Discoveries...\n");
  publishButtonDiscoveries();
  delay(100);
  yield();
  PRINT("[DEBUG] Free heap: ", ESP.getFreeHeap());
  PRINTS(" bytes\n");

  PRINTS("[DEBUG] Publishing Clock Discoveries...\n");
  publishClockDiscoveries();
  delay(100);
  yield();
  PRINT("[DEBUG] Free heap: ", ESP.getFreeHeap());
  PRINTS(" bytes\n");

  // Recurrent Alarm MQTT discovery - ESP32 only (web-only on ESP8266 to prevent memory exhaustion)
  #ifdef ESP32
  PRINTS("[DEBUG] Publishing Recurrent Alarm Discoveries...\n");
  publishRecurrentAlarmDiscoveries();
  delay(100);
  yield();
  PRINT("[DEBUG] Free heap: ", ESP.getFreeHeap());
  PRINTS(" bytes\n");
  #else
  PRINTS("[DEBUG] Skipping Recurrent Alarm Discoveries on ESP8266 (web-only feature)\n");
  #endif

#ifndef DISABLE_TIMER_FEATURE
  PRINTS("[DEBUG] Publishing Timer Discoveries...\n");
  publishTimerDiscoveries();
  delay(100);
  yield();
  PRINT("[DEBUG] Free heap: ", ESP.getFreeHeap());
  PRINTS(" bytes\n");
#endif

#ifndef DISABLE_SLEEP_MODE_FEATURE
  PRINTS("[DEBUG] Publishing Sleep Mode Discoveries...\n");
  publishSleepModeDiscoveries();
  delay(100);
  yield();
  PRINT("[DEBUG] Free heap: ", ESP.getFreeHeap());
  PRINTS(" bytes\n");
#endif

#ifndef DISABLE_WEATHER_FEATURE
  PRINTS("[DEBUG] Publishing Weather Discovery...\n");
  publishWeatherDiscovery();
  delay(100);
  yield();
  PRINT("[DEBUG] Free heap: ", ESP.getFreeHeap());
  PRINTS(" bytes\n");
#endif

  // Subscribe to command topics
  subscribeToHACommands();

  // Mark as published
  mqttDiscoveryPublished = true;

  PRINTS("\n=== Discovery Complete ===");

  // Publish initial states
  publishAllStates();
}
