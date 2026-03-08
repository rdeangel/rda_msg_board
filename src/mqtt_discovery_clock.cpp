#include "mqtt_discovery_clock.h"
#include "mqtt_discovery_core.h"
#include "globals.h"
#include "mqtt.h"
#include "functions.h"
#include "config_manager.h"

// Clock control discoveries
void publishClockDiscoveries() {
  // Clock Enable Switch
  {
    JsonDocument doc;
    char baseTopic[256];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "Clock Enable";
    doc["uniq_id"] = String(haBaseTopic) + "_clock_enable";
    doc["cmd_t"] = "~/clock_enable/set";
    doc["stat_t"] = "~/clock_enable/state";
    doc["payload_on"] = "ON";
    doc["payload_off"] = "OFF";
    doc["icon"] = "mdi:clock-digital";
    doc["ent_cat"] = "config";
    addDeviceInfo(doc);
    addAvailability(doc);
    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "switch", "clock_enable");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished clock_enable discovery to: ", topic);
  }

  // Clock Brightness Number (0-15, slider) - Main Controls
  {
    JsonDocument doc;
    char baseTopic[256];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "Clock Brightness";
    doc["uniq_id"] = String(haBaseTopic) + "_clock_brightness";
    doc["cmd_t"] = "~/clock_brightness/set";
    doc["stat_t"] = "~/clock_brightness/state";
    doc["min"] = 0;
    doc["max"] = 15;
    doc["step"] = 1;
    doc["mode"] = "slider";
    doc["icon"] = "mdi:brightness-6";
    doc["ent_cat"] = "config";
    addDeviceInfo(doc);
    addAvailability(doc);
    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "number", "clock_brightness");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished clock_brightness discovery to: ", topic);
  }

  // --- Advanced clock entities: skipped on ESP8266 to reduce heap pressure ---
  // Re-enable by removing DISABLE_HA_CLOCK_ADVANCED from platformio.ini build_flags.
  // These are still configurable via the web UI.
#ifndef DISABLE_HA_CLOCK_ADVANCED

  // Transition Speed Number - Main Controls
  {
    JsonDocument doc;
    char baseTopic[256];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "Clock Transition Speed";
    doc["uniq_id"] = String(haBaseTopic) + "_clock_transition_speed";
    doc["cmd_t"] = "~/clock_transition_speed/set";
    doc["stat_t"] = "~/clock_transition_speed/state";
    doc["min"] = 10;
    doc["max"] = 200;
    doc["step"] = 5;
    doc["mode"] = "slider";
    doc["unit_of_meas"] = "ms";
    doc["icon"] = "mdi:speedometer";
    doc["ent_cat"] = "config";
    addDeviceInfo(doc);
    addAvailability(doc);
    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "number", "clock_transition_speed");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished clock_transition_speed discovery to: ", topic);
  }

  // Transition Effect Select (dropdown) - Main Controls
  {
    JsonDocument doc;
    char baseTopic[256];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "Clock Transition Effect";
    doc["uniq_id"] = String(haBaseTopic) + "_clock_transition_effect";
    doc["cmd_t"] = "~/clock_transition_effect/set";
    doc["stat_t"] = "~/clock_transition_effect/state";
    JsonArray options = doc["options"].to<JsonArray>();
    options.add("FADE");
    options.add("DISSOLVE");
    options.add("BLINDS");
    options.add("WIPE");
    options.add("GROW");
    options.add("SCROLL");
    options.add("SCAN");
    options.add("OPENING");
    options.add("CLOSING");
    options.add("SCROLL_DOWN");
    options.add("SCROLL_LEFT");
    options.add("SCROLL_RIGHT");
    options.add("SLICE");
    options.add("MESH");
    options.add("GROW_DOWN");
    options.add("SCAN_VERTX");
    options.add("SCROLL_UP_LEFT");
    options.add("SCROLL_UP_RIGHT");
    options.add("OPENING_CURSOR");
    options.add("SCAN_VERT");
    options.add("SCROLL_DOWN_LEFT");
    options.add("WIPE_CURSOR");
    options.add("SCAN_HORIZX");
    options.add("CLOSING_CURSOR");
    options.add("SCROLL_DOWN_RIGHT");
    options.add("PRINT");
    doc["icon"] = "mdi:animation";
    doc["ent_cat"] = "config";
    addDeviceInfo(doc);
    addAvailability(doc);
    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "select", "clock_transition_effect");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished clock_transition_effect discovery to: ", topic);
  }

  // Randomize Transitions Switch - Main Controls
  {
    JsonDocument doc;
    char baseTopic[256];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "Clock Randomize Transitions";
    doc["uniq_id"] = String(haBaseTopic) + "_clock_randomize";
    doc["cmd_t"] = "~/clock_randomize/set";
    doc["stat_t"] = "~/clock_randomize/state";
    doc["payload_on"] = "ON";
    doc["payload_off"] = "OFF";
    doc["icon"] = "mdi:shuffle-variant";
    doc["ent_cat"] = "config";
    addDeviceInfo(doc);
    addAvailability(doc);
    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "switch", "clock_randomize");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished clock_randomize discovery to: ", topic);
  }

  // NTP Server Text - Configuration
  {
    JsonDocument doc;
    char baseTopic[256];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "Clock NTP Server";
    doc["uniq_id"] = String(haBaseTopic) + "_clock_ntp_server";
    doc["cmd_t"] = "~/clock_ntp_server/set";
    doc["stat_t"] = "~/clock_ntp_server/state";
    doc["mode"] = "text";
    doc["icon"] = "mdi:server-network";
    doc["ent_cat"] = "config";
    addDeviceInfo(doc);
    addAvailability(doc);
    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "text", "clock_ntp_server");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished clock_ntp_server discovery to: ", topic);
  }

  // Timezone Text (POSIX) - Configuration
  // Note: Removed dropdown select entity to reduce payload size and prevent WDT crashes
  // Users can enter any POSIX timezone string directly (e.g., EST5EDT,M3.2.0,M11.1.0)
  {
    JsonDocument doc;
    char baseTopic[256];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "Clock Timezone (POSIX)";
    doc["uniq_id"] = String(haBaseTopic) + "_clock_custom_tz";
    doc["cmd_t"] = "~/clock_custom_tz/set";
    doc["stat_t"] = "~/clock_custom_tz/state";
    doc["mode"] = "text";
    doc["icon"] = "mdi:map-clock";
    doc["ent_cat"] = "config";
    addDeviceInfo(doc);
    addAvailability(doc);
    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "text", "clock_custom_tz");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished clock_custom_tz discovery to: ", topic);
  }

  // Date Format Select - Configuration
  {
    JsonDocument doc;
    char baseTopic[256];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "Clock Date Format";
    doc["uniq_id"] = String(haBaseTopic) + "_clock_date_format";
    doc["cmd_t"] = "~/clock_date_format/set";
    doc["stat_t"] = "~/clock_date_format/state";
    JsonArray options = doc["options"].to<JsonArray>();
    #if MAX_DEVICES == 4
      options.add("TIME_ONLY");
      options.add("TIME_SECONDS");
    #elif MAX_DEVICES == 8
      options.add("TIME_ONLY");
      options.add("TIME_DATE");
      options.add("FULL_DATE");
      options.add("TIME_FULL_DATE");
      options.add("CUSTOM");
      options.add("TIME_SECONDS");
    #else
      options.add("TIME_ONLY");
    #endif
    doc["icon"] = "mdi:calendar-clock";
    doc["ent_cat"] = "config";
    addDeviceInfo(doc);
    addAvailability(doc);
    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "select", "clock_date_format");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished clock_date_format discovery to: ", topic);
  }

  #if MAX_DEVICES == 4
  // Date Alternate Interval Number - Configuration (4-module only)
  {
    JsonDocument doc;
    char baseTopic[256];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "Clock Date Alternate Interval";
    doc["uniq_id"] = String(haBaseTopic) + "_clock_date_alternate";
    doc["cmd_t"] = "~/clock_date_alternate/set";
    doc["stat_t"] = "~/clock_date_alternate/state";
    doc["min"] = 1;
    doc["max"] = 60;
    doc["step"] = 1;
    doc["mode"] = "slider";
    doc["unit_of_meas"] = "s";
    doc["icon"] = "mdi:timer-outline";
    doc["ent_cat"] = "config";
    addDeviceInfo(doc);
    addAvailability(doc);
    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "number", "clock_date_alternate");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished clock_date_alternate discovery to: ", topic);
  }
  #endif

  #if MAX_DEVICES == 8
  // Custom Date Format Text - Configuration (8-module only)
  {
    JsonDocument doc;
    char baseTopic[256];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "Clock Custom Date Format";
    doc["uniq_id"] = String(haBaseTopic) + "_clock_custom_format";
    doc["cmd_t"] = "~/clock_custom_format/set";
    doc["stat_t"] = "~/clock_custom_format/state";
    doc["mode"] = "text";
    doc["icon"] = "mdi:format-text";
    doc["ent_cat"] = "config";
    addDeviceInfo(doc);
    addAvailability(doc);
    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "text", "clock_custom_format");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished clock_custom_format discovery to: ", topic);
  }
  #endif

  // Transition Delay Number - Configuration
  {
    JsonDocument doc;
    char baseTopic[256];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "Clock Transition Delay";
    doc["uniq_id"] = String(haBaseTopic) + "_clock_transition_delay";
    doc["cmd_t"] = "~/clock_transition_delay/set";
    doc["stat_t"] = "~/clock_transition_delay/state";
    doc["min"] = 0;
    doc["max"] = 10000;
    doc["step"] = 100;
    doc["mode"] = "slider";
    doc["unit_of_meas"] = "ms";
    doc["icon"] = "mdi:timer-outline";
    doc["ent_cat"] = "config";
    addDeviceInfo(doc);
    addAvailability(doc);
    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "number", "clock_transition_delay");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished clock_transition_delay discovery to: ", topic);
  }

  // Resync Interval Number - Configuration
  {
    JsonDocument doc;
    char baseTopic[256];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "Clock Resync Interval";
    doc["uniq_id"] = String(haBaseTopic) + "_clock_resync_interval";
    doc["cmd_t"] = "~/clock_resync_interval/set";
    doc["stat_t"] = "~/clock_resync_interval/state";
    doc["min"] = 1;
    doc["max"] = 24;
    doc["step"] = 1;
    doc["mode"] = "box";
    doc["unit_of_meas"] = "h";
    doc["icon"] = "mdi:sync";
    doc["ent_cat"] = "config";
    addDeviceInfo(doc);
    addAvailability(doc);
    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "number", "clock_resync_interval");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished clock_resync_interval discovery to: ", topic);
  }

#endif // DISABLE_HA_CLOCK_ADVANCED

  // Clock Face Select - font/display style
  {
    JsonDocument doc;
    char baseTopic[256];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "Clock Face";
    doc["uniq_id"] = String(haBaseTopic) + "_clock_face";
    doc["cmd_t"] = "~/clock_face/set";
    doc["stat_t"] = "~/clock_face/state";
    JsonArray options = doc["options"].to<JsonArray>();
    options.add("DEFAULT");
    options.add("MATRIX_LIGHT");
    options.add("MATRIX_LIGHT_6");
    doc["icon"] = "mdi:clock-digital";
    doc["ent_cat"] = "config";
    addDeviceInfo(doc);
    addAvailability(doc);
    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "select", "clock_face");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished clock_face discovery to: ", topic);
  }
  delay(20);

  // Date Alternate Enable switch
  {
    JsonDocument doc;
    char baseTopic[128];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "Alternate Date";
    doc["uniq_id"] = String(haBaseTopic) + "_clock_date_alternate_enable";
    doc["cmd_t"] = "~/clock_date_alternate_enable/set";
    doc["stat_t"] = "~/clock_date_alternate_enable/state";
    doc["payload_on"] = "on";
    doc["payload_off"] = "off";
    doc["icon"] = "mdi:calendar-clock";
    doc["ent_cat"] = "config";
    addDeviceInfo(doc);
    addAvailability(doc);
    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "switch", "clock_date_alternate_enable");
    char payload[512];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
  }
  delay(20);

  // AM/PM mode switch
  {
    JsonDocument doc;
    char baseTopic[128];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "12-hour AM/PM";
    doc["uniq_id"] = String(haBaseTopic) + "_clock_ampm";
    doc["cmd_t"] = "~/clock_ampm/set";
    doc["stat_t"] = "~/clock_ampm/state";
    doc["payload_on"] = "on";
    doc["payload_off"] = "off";
    doc["icon"] = "mdi:hours-12";
    doc["ent_cat"] = "config";
    addDeviceInfo(doc);
    addAvailability(doc);
    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "switch", "clock_ampm");
    char payload[512];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
  }
  delay(20);
}

// ============================================================================
// RECURRENT ALARM DISCOVERY
// ============================================================================

void publishRecurrentAlarmDiscoveries() {
  // Recurrent Alarm Enable Switch
  {
    JsonDocument doc;
    char baseTopic[256];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "Recurrent Alarm Enable";
    doc["uniq_id"] = String(haBaseTopic) + "_recurrent_alarm_enable";
    doc["cmd_t"] = "~/recurrent_alarm_enable/set";
    doc["stat_t"] = "~/recurrent_alarm_enable/state";
    doc["payload_on"] = "ON";
    doc["payload_off"] = "OFF";
    doc["icon"] = "mdi:alarm-multiple";
    doc["ent_cat"] = "config";
    addDeviceInfo(doc);
    addAvailability(doc);
    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "switch", "recurrent_alarm_enable");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished recurrent_alarm_enable discovery to: ", topic);
  }

  // Recurrent Alarm Disable Weekends Switch
  {
    JsonDocument doc;
    char baseTopic[256];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "Recurrent Alarm Disable Weekends";
    doc["uniq_id"] = String(haBaseTopic) + "_recurrent_alarm_disable_weekends";
    doc["cmd_t"] = "~/recurrent_alarm_disable_weekends/set";
    doc["stat_t"] = "~/recurrent_alarm_disable_weekends/state";
    doc["payload_on"] = "ON";
    doc["payload_off"] = "OFF";
    doc["icon"] = "mdi:calendar-weekend";
    doc["ent_cat"] = "config";
    addDeviceInfo(doc);
    addAvailability(doc);
    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "switch", "recurrent_alarm_disable_weekends");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished recurrent_alarm_disable_weekends discovery to: ", topic);
  }

  // Recurrent Alarm Interval Select
  {
    JsonDocument doc;
    char baseTopic[256];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "Recurrent Alarm Interval";
    doc["uniq_id"] = String(haBaseTopic) + "_recurrent_alarm_interval";
    doc["cmd_t"] = "~/recurrent_alarm_interval/set";
    doc["stat_t"] = "~/recurrent_alarm_interval/state";
    JsonArray options = doc["options"].to<JsonArray>();
    options.add("15min");
    options.add("30min");
    options.add("1hour");
    options.add("3hours");
    options.add("6hours");
    options.add("midday");
    doc["icon"] = "mdi:timer-outline";
    doc["ent_cat"] = "config";
    addDeviceInfo(doc);
    addAvailability(doc);
    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "select", "recurrent_alarm_interval");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished recurrent_alarm_interval discovery to: ", topic);
  }

  // Recurrent Alarm Chirp Select
  {
    JsonDocument doc;
    char baseTopic[256];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "Recurrent Alarm Sound";
    doc["uniq_id"] = String(haBaseTopic) + "_recurrent_alarm_chirp";
    doc["cmd_t"] = "~/recurrent_alarm_chirp/set";
    doc["stat_t"] = "~/recurrent_alarm_chirp/state";
    doc["icon"] = "mdi:music-note";

    // Build options array from chirp library
    JsonArray options = doc["options"].to<JsonArray>();
    int chirpCount = getChirpCount();
    for (int i = 0; i < chirpCount; i++) {
      const ChirpPattern* pattern = getChirpByIndex(i);
      if (pattern) {
        options.add(pattern->name);
      }
    }

    doc["ent_cat"] = "config";
    addDeviceInfo(doc);
    addAvailability(doc);
    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "select", "recurrent_alarm_chirp");
    char payload[2048];  // Larger buffer for options list
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished recurrent_alarm_chirp discovery to: ", topic);
  }
}

// ============================================================================
// RECURRENT ALARM STATE PUBLISHERS
// ============================================================================

void publishRecurrentAlarmEnableState() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) return;
  char topic[256];
  buildStateTopic(topic, sizeof(topic), "recurrent_alarm_enable");
  const char* value = recurrentAlarmEnabled ? "ON" : "OFF";
  mqttClient.publish(topic, value, true);
  PRINT("\nPublished recurrent_alarm_enable state: ", value);
}

void publishRecurrentAlarmDisableWeekendsState() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) return;
  char topic[256];
  buildStateTopic(topic, sizeof(topic), "recurrent_alarm_disable_weekends");
  const char* value = recurrentAlarmDisableWeekends ? "ON" : "OFF";
  mqttClient.publish(topic, value, true);
  PRINT("\nPublished recurrent_alarm_disable_weekends state: ", value);
}

void publishRecurrentAlarmIntervalState() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) return;
  char topic[256];
  buildStateTopic(topic, sizeof(topic), "recurrent_alarm_interval");
  mqttClient.publish(topic, recurrentAlarmConfig.interval, true);
  PRINT("\nPublished recurrent_alarm_interval state: ", recurrentAlarmConfig.interval);
}

void publishRecurrentAlarmChirpState() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) return;
  char topic[256];
  buildStateTopic(topic, sizeof(topic), "recurrent_alarm_chirp");
  mqttClient.publish(topic, recurrentAlarmConfig.chirpName, true);
  PRINT("\nPublished recurrent_alarm_chirp state: ", recurrentAlarmConfig.chirpName);
}

void publishAllRecurrentAlarmStates() {
  publishRecurrentAlarmEnableState();
  delay(20);
  publishRecurrentAlarmDisableWeekendsState();
  delay(20);
  publishRecurrentAlarmIntervalState();
  delay(20);
  publishRecurrentAlarmChirpState();
}

// ============================================================================
// RECURRENT ALARM COMMAND HANDLERS
// ============================================================================

void handleRecurrentAlarmEnableCommand(const char* payload) {
  PRINT("\nHandling recurrent_alarm_enable command: ", payload);

  bool enabled = (strcmp(payload, "ON") == 0);
  strlcpy(recurrentAlarmConfig.enabled, enabled ? "on" : "off", sizeof(recurrentAlarmConfig.enabled));
  recurrentAlarmEnabled = enabled;
  saveRecurrentAlarmConfiguration(recurrentAlarmConfigFile, recurrentAlarmConfig);
  publishRecurrentAlarmEnableState();
}

void handleRecurrentAlarmIntervalCommand(const char* payload) {
  PRINT("\nHandling recurrent_alarm_interval command: ", payload);

  // Validate interval
  if (strcmp(payload, "15min") == 0 || strcmp(payload, "30min") == 0 ||
      strcmp(payload, "1hour") == 0 || strcmp(payload, "3hours") == 0 ||
      strcmp(payload, "6hours") == 0 || strcmp(payload, "midday") == 0) {
    strlcpy(recurrentAlarmConfig.interval, payload, sizeof(recurrentAlarmConfig.interval));
    recurrentAlarmIntervalMinutes = getIntervalMinutes(payload);
    saveRecurrentAlarmConfiguration(recurrentAlarmConfigFile, recurrentAlarmConfig);
    publishRecurrentAlarmIntervalState();
  }
}

void handleRecurrentAlarmChirpCommand(const char* payload) {
  PRINT("\nHandling recurrent_alarm_chirp command: ", payload);

  // Validate that the chirp name exists
  const ChirpPattern* pattern = getChirpByName(payload);
  if (pattern == nullptr) {
    PRINT("\nError: Unknown chirp pattern: ", payload);
    return;
  }

  strlcpy(recurrentAlarmConfig.chirpName, payload, sizeof(recurrentAlarmConfig.chirpName));
  saveRecurrentAlarmConfiguration(recurrentAlarmConfigFile, recurrentAlarmConfig);
  publishRecurrentAlarmChirpState();
}

void handleRecurrentAlarmDisableWeekendsCommand(const char* payload) {
  PRINT("\nHandling recurrent_alarm_disable_weekends command: ", payload);

  bool disableWeekends = (strcmp(payload, "ON") == 0);
  strlcpy(recurrentAlarmConfig.disableWeekends, disableWeekends ? "on" : "off", sizeof(recurrentAlarmConfig.disableWeekends));
  recurrentAlarmDisableWeekends = disableWeekends;
  saveRecurrentAlarmConfiguration(recurrentAlarmConfigFile, recurrentAlarmConfig);
  publishRecurrentAlarmDisableWeekendsState();
}
