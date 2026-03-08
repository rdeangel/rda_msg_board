#include "mqtt_discovery_sensors.h"
#include "mqtt_discovery_core.h"
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
#include "chirp_library.h"

// ============================================================================
// DISCOVERY PUBLISHING FUNCTIONS
// ============================================================================

void publishLightDiscovery() {
  // Now using Number entity for brightness-only control
  JsonDocument doc;

  char baseTopic[256];
  snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
  doc["~"] = baseTopic;

  doc["name"] = "Brightness";
  doc["uniq_id"] = String(haBaseTopic) + "_brightness";
  doc["cmd_t"] = "~/brightness/set";
  doc["stat_t"] = "~/brightness/state";
  doc["min"] = 0;
  doc["max"] = 15;
  doc["step"] = 1;
  doc["mode"] = "slider";
  doc["icon"] = "mdi:brightness-6";

  addDeviceInfo(doc);
  addAvailability(doc);

  char topic[256];
  buildDiscoveryTopic(topic, sizeof(topic), "number", "brightness");

  char payload[1024];
  serializeJson(doc, payload, sizeof(payload));

  mqttClient.publish(topic, payload, true);
  PRINT("\nPublished brightness discovery to: ", topic);
}

void publishTextDiscovery() {
  JsonDocument doc;

  char baseTopic[256];
  snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
  doc["~"] = baseTopic;

  doc["name"] = "Message";
  doc["uniq_id"] = String(haBaseTopic) + "_message";
  doc["cmd_t"] = "~/message/set";
  doc["stat_t"] = "~/message/state";
  doc["min"] = 0;  // Allow empty messages
  doc["max"] = 255;  // Home Assistant text entities have 255 char limit
  doc["mode"] = "text";
  doc["val_tpl"] = "{{ value_json.text }}";

  addDeviceInfo(doc);
  addAvailability(doc);

  char topic[256];
  buildDiscoveryTopic(topic, sizeof(topic), "text", "message");

  char payload[1024];
  serializeJson(doc, payload, sizeof(payload));
  mqttClient.publish(topic, payload, true);
  PRINT("\nPublished text discovery to: ", topic);
}

void publishNumberDiscoveries() {
  // Repeat count
  {
    JsonDocument doc;
    char baseTopic[256];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "Repeat Count";
    doc["uniq_id"] = String(haBaseTopic) + "_repeat";
    doc["cmd_t"] = "~/repeat/set";
    doc["stat_t"] = "~/repeat/state";
    doc["min"] = 0;
    doc["max"] = 999;
    doc["step"] = 1;
    doc["mode"] = "box";
    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "number", "repeat");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished repeat discovery to: ", topic);
  }

  // Scroll delay
  {
    JsonDocument doc;
    char baseTopic[256];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "Scroll Delay";
    doc["uniq_id"] = String(haBaseTopic) + "_scroll_delay";
    doc["cmd_t"] = "~/scroll_delay/set";
    doc["stat_t"] = "~/scroll_delay/state";
    doc["min"] = 1;
    doc["max"] = 200;
    doc["step"] = 1;
    doc["mode"] = "slider";
    doc["unit_of_meas"] = "ms";
    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "number", "scroll_delay");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished scroll_delay discovery to: ", topic);
  }

  // Buzzer chirps
  {
    JsonDocument doc;
    char baseTopic[256];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "Buzzer Chirps";
    doc["uniq_id"] = String(haBaseTopic) + "_buzzer";
    doc["cmd_t"] = "~/buzzer/set";
    doc["stat_t"] = "~/buzzer/state";
    doc["min"] = 0;
    doc["max"] = 99;
    doc["step"] = 1;
    doc["mode"] = "box";
    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "number", "buzzer");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished buzzer discovery to: ", topic);
  }
}

void publishSwitchDiscoveries() {
  JsonDocument doc;

  char baseTopic[256];
  snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
  doc["~"] = baseTopic;

  doc["name"] = "ASCII Conversion";
  doc["uniq_id"] = String(haBaseTopic) + "_ascii_conv";
  doc["cmd_t"] = "~/ascii_conv/set";
  doc["stat_t"] = "~/ascii_conv/state";
  doc["payload_on"] = "ON";
  doc["payload_off"] = "OFF";
  doc["state_on"] = "ON";
  doc["state_off"] = "OFF";

  addDeviceInfo(doc);
  addAvailability(doc);

  char topic[256];
  buildDiscoveryTopic(topic, sizeof(topic), "switch", "ascii_conv");

  char payload[1024];
  serializeJson(doc, payload, sizeof(payload));
  mqttClient.publish(topic, payload, true);
  PRINT("\nPublished switch discovery to: ", topic);

  // MQTT Messages Enable Switch
  JsonDocument doc2;
  doc2["~"] = baseTopic;

  doc2["name"] = "MQTT Messages";
  doc2["uniq_id"] = String(haBaseTopic) + "_mqtt_messages";
  doc2["cmd_t"] = "~/mqtt_messages/set";
  doc2["stat_t"] = "~/mqtt_messages/state";
  doc2["payload_on"] = "ON";
  doc2["payload_off"] = "OFF";
  doc2["state_on"] = "ON";
  doc2["state_off"] = "OFF";
  doc2["icon"] = "mdi:message-processing";

  addDeviceInfo(doc2);
  addAvailability(doc2);

  buildDiscoveryTopic(topic, sizeof(topic), "switch", "mqtt_messages");

  serializeJson(doc2, payload, sizeof(payload));
  mqttClient.publish(topic, payload, true);
  PRINT("\nPublished MQTT Messages switch discovery to: ", topic);
}

void publishSensorDiscoveries() {
    // Message Displayed sensor (read-only, shows current message)
  {
    JsonDocument doc;
    char baseTopic[256];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "Message Displayed";
    doc["uniq_id"] = String(haBaseTopic) + "_msg_displayed";
    doc["stat_t"] = "~/msg_displayed/state";
    doc["json_attr_t"] = "~/msg_displayed/attrs";
    doc["icon"] = "mdi:message-text";
    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "sensor", "msg_displayed");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished Message Displayed sensor discovery to: ", topic);
  }

  // Hostname sensor
  {
    JsonDocument doc;
    char baseTopic[256];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "Hostname";
    doc["uniq_id"] = String(haBaseTopic) + "_hostname";
    doc["stat_t"] = "~/hostname/state";
    doc["icon"] = "mdi:lan";
    doc["enabled_by_default"] = false;
    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "sensor", "hostname");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished hostname sensor discovery to: ", topic);
  }

  // IP Address sensor
  {
    JsonDocument doc;
    char baseTopic[256];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "IP Address";
    doc["uniq_id"] = String(haBaseTopic) + "_ip";
    doc["stat_t"] = "~/ip/state";
    doc["icon"] = "mdi:ip-network";
    doc["enabled_by_default"] = false;
    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "sensor", "ip");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished IP sensor discovery to: ", topic);
  }

  // RSSI sensor
    // RSSI sensor
  {
    JsonDocument doc;
    char baseTopic[256];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "WiFi Signal";
    doc["uniq_id"] = String(haBaseTopic) + "_rssi";
    doc["stat_t"] = "~/rssi/state";
    doc["unit_of_meas"] = "dBm";
    doc["dev_cla"] = "signal_strength";
    doc["stat_cla"] = "measurement";
    doc["icon"] = "mdi:wifi";
    doc["enabled_by_default"] = false;
    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "sensor", "rssi");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished RSSI sensor discovery to: ", topic);
  }

  // WiFi SSID sensor
  {
    JsonDocument doc;
    char baseTopic[256];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "WiFi SSID";
    doc["uniq_id"] = String(haBaseTopic) + "_ssid";
    doc["stat_t"] = "~/ssid/state";
    doc["icon"] = "mdi:wifi-marker";
    doc["enabled_by_default"] = false;
    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "sensor", "ssid");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished SSID sensor discovery to: ", topic);
  }

  // Version sensor
  {
    JsonDocument doc;
    char baseTopic[256];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "Firmware Version";
    doc["uniq_id"] = String(haBaseTopic) + "_version";
    doc["stat_t"] = "~/version/state";
    doc["icon"] = "mdi:information-outline";
    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "sensor", "version");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished version sensor discovery to: ", topic);
  }

  // Uptime sensor (Numeric - for graphs)
  {
    JsonDocument doc;
    char baseTopic[256];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "Uptime";
    doc["uniq_id"] = String(haBaseTopic) + "_uptime";
    doc["stat_t"] = "~/uptime/state";
    doc["unit_of_meas"] = "s";
    doc["dev_cla"] = "duration";
    doc["stat_cla"] = "total_increasing";
    doc["icon"] = "mdi:clock-outline";
    doc["enabled_by_default"] = false;
    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "sensor", "uptime");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished uptime sensor discovery to: ", topic);
  }

  // Uptime Text sensor (Formatted - for display)
  {
    JsonDocument doc;
    char baseTopic[256];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "Uptime Text";
    doc["uniq_id"] = String(haBaseTopic) + "_uptime_text";
    doc["stat_t"] = "~/uptime/formatted";
    doc["icon"] = "mdi:clock-check-outline";
    doc["enabled_by_default"] = false;
    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "sensor", "uptime_text");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished uptime text sensor discovery to: ", topic);
  }

  // Repeat Countdown sensor
  {
    JsonDocument doc;
    char baseTopic[256];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "Repeat Countdown";
    doc["uniq_id"] = String(haBaseTopic) + "_repeat_countdown";
    doc["stat_t"] = "~/repeat_countdown/state";
    doc["icon"] = "mdi:counter";
    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "sensor", "repeat_countdown");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished repeat countdown sensor discovery to: ", topic);
  }

  // Clock Time Sensor
  {
    JsonDocument doc;
    char baseTopic[256];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "Time";
    doc["uniq_id"] = String(haBaseTopic) + "_clock_time";
    doc["stat_t"] = "~/clock_time/state";
    doc["icon"] = "mdi:clock-time-four-outline";
    addDeviceInfo(doc);
    addAvailability(doc);
    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "sensor", "clock_time");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished clock_time sensor discovery to: ", topic);
  }

  // Clock NTP Synced (binary sensor)
  {
    JsonDocument doc;
    char baseTopic[256];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "NTP Synced";
    doc["uniq_id"] = String(haBaseTopic) + "_clock_ntp_synced";
    doc["stat_t"] = "~/clock_ntp_synced/state";
    doc["payload_on"] = "ON";
    doc["payload_off"] = "OFF";
    doc["dev_cla"] = "connectivity";
    doc["icon"] = "mdi:sync-circle";
    doc["enabled_by_default"] = false;
    addDeviceInfo(doc);
    addAvailability(doc);
    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "binary_sensor", "clock_ntp_synced");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished clock_ntp_synced binary sensor discovery to: ", topic);
  }

  // Clock Display Active (binary sensor)
  {
    JsonDocument doc;
    char baseTopic[256];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "Display Active";
    doc["uniq_id"] = String(haBaseTopic) + "_clock_display_active";
    doc["stat_t"] = "~/clock_display_active/state";
    doc["payload_on"] = "ON";
    doc["payload_off"] = "OFF";
    doc["icon"] = "mdi:monitor";
    doc["enabled_by_default"] = false;
    addDeviceInfo(doc);
    addAvailability(doc);
    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "binary_sensor", "clock_display_active");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished clock_display_active binary sensor discovery to: ", topic);
  }

  // Display Mode Sensor
  {
    JsonDocument doc;
    char baseTopic[256];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "Display Mode";
    doc["uniq_id"] = String(haBaseTopic) + "_display_mode";
    doc["stat_t"] = "~/display_mode/state";
    doc["icon"] = "mdi:view-dashboard";
    doc["enabled_by_default"] = false;
    addDeviceInfo(doc);
    addAvailability(doc);
    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "sensor", "display_mode");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished display_mode sensor discovery to: ", topic);
  }

#ifdef ESP32
  // TLS Connection Status (binary sensor) - ESP32 only
  {
    JsonDocument doc;
    char baseTopic[256];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "TLS Secure Connection";
    doc["uniq_id"] = String(haBaseTopic) + "_tls_status";
    doc["stat_t"] = "~/tls_status/state";
    doc["payload_on"] = "ON";
    doc["payload_off"] = "OFF";
    doc["dev_cla"] = "connectivity";
    doc["icon"] = "mdi:lock-check";
    doc["enabled_by_default"] = false;
    addDeviceInfo(doc);
    addAvailability(doc);
    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "binary_sensor", "tls_status");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished TLS status binary sensor discovery to: ", topic);
  }
#endif
}

void publishButtonDiscoveries() {
  // Reboot button
  {
    JsonDocument doc;
    char baseTopic[256];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "Reboot";
    doc["uniq_id"] = String(haBaseTopic) + "_reboot";
    doc["cmd_t"] = "~/reboot/command";
    doc["payload_press"] = "PRESS";
    doc["icon"] = "mdi:restart";
    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "button", "reboot");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished reboot button discovery to: ", topic);
  }

  // Clear message button
  {
    JsonDocument doc;
    char baseTopic[256];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "Clear Message";
    doc["uniq_id"] = String(haBaseTopic) + "_clear";
    doc["cmd_t"] = "~/clear/command";
    doc["payload_press"] = "PRESS";
    doc["icon"] = "mdi:eraser";
    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "button", "clear");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished clear button discovery to: ", topic);
  }

  // Send message button (applies all stored parameters)
  {
    JsonDocument doc;
    char baseTopic[256];
    snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
    doc["~"] = baseTopic;
    doc["name"] = "Send Message";
    doc["uniq_id"] = String(haBaseTopic) + "_send";
    doc["cmd_t"] = "~/send/command";
    doc["payload_press"] = "PRESS";
    doc["icon"] = "mdi:send";
    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "button", "send");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished send button discovery to: ", topic);
  }
}

// ============================================================================
// STATE PUBLISHING FUNCTIONS
// ============================================================================

void publishLightState() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) {
    return;
  }

  char topic[256];
  buildStateTopic(topic, sizeof(topic), "brightness");

  char value[16];
  snprintf(value, sizeof(value), "%d", ledBrightness);
  mqttClient.publish(topic, value, true);

  PRINT("\nPublished brightness state: ", value);
}

void publishMessageState() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) {
    return;
  }

  char topic[256];
  buildStateTopic(topic, sizeof(topic), "message");

  // Publish HA RAM value, wrapped in JSON to support empty strings
  // (Empty raw string clears retained message in MQTT)
  JsonDocument doc;
  doc["text"] = haLastMessage;

  char* jsonPayload = (char*)malloc(MSG_JSON_SIZE);
  if (jsonPayload != NULL) {
    serializeJson(doc, jsonPayload, MSG_JSON_SIZE);
    mqttClient.publish(topic, jsonPayload, true);
    PRINT("\nPublished message state (JSON): ", jsonPayload);
    free(jsonPayload);
  } else {
    PRINTS("\nError: Failed to allocate memory for message state");
  }
}

// Re-implemented to avoid stack overflow with large JSON buffer
void publishMessageDisplayedState() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) {
    return;
  }

  char topic[256];
  buildStateTopic(topic, sizeof(topic), "msg_displayed");

  // If clock is displaying, publish empty string
  if (currentDisplayMode == MODE_CLOCK || currentDisplayMode == MODE_TRANSITION) {
    mqttClient.publish(topic, "", true);
    PRINT("\nPublished message displayed state: ", "(empty - clock active)");
    return;  // Don't publish attributes when clock is active
  }

  // 1. Publish truncated message to state topic (HA limit 255 chars)
  char truncatedMsg[256];
  if (strlen(curMessage) > 250) {
    strncpy(truncatedMsg, curMessage, 250);
    truncatedMsg[250] = '\0';
    strcat(truncatedMsg, "...");
  } else {
    strcpy(truncatedMsg, curMessage);
  }

  mqttClient.publish(topic, truncatedMsg, true);
  PRINT("\nPublished message displayed state: ", (truncatedMsg[0] == '\0' ? "(empty)" : truncatedMsg));

  // 2. Publish full message to attributes topic using HEAP allocation
  // (Stack allocation of large buffer caused crash)
  char attrTopic[256];
  snprintf(attrTopic, sizeof(attrTopic), "%s/ha/msg_displayed/attrs", mqttTopicDevice);
  
  // Create JSON doc
  JsonDocument doc;
  doc["full_message"] = curMessage;

  // Allocate buffer on heap
  char* jsonPayload = (char*)malloc(MSG_JSON_SIZE);
  if (jsonPayload != NULL) {
    serializeJson(doc, jsonPayload, MSG_JSON_SIZE);
    
    // Check if payload fits in MQTT packet (just a safety check, though we increased MAX_PACKET_SIZE)
    if (strlen(jsonPayload) < MQTT_MAX_PACKET_SIZE) {
      mqttClient.publish(attrTopic, jsonPayload, true);
    } else {
      PRINTS("\nError: JSON payload exceeds MQTT_MAX_PACKET_SIZE");
    }
    
    free(jsonPayload); // Critical: Free memory
  } else {
    PRINTS("\nError: Failed to allocate memory for JSON payload");
  }
}

void publishRepeatCountdownState(int count) {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) {
    return;
  }

  char topic[256];
  buildStateTopic(topic, sizeof(topic), "repeat_countdown");

  char value[16];
  snprintf(value, sizeof(value), "%d", count);
  mqttClient.publish(topic, value, true);
  // Optional: Reduce log spam by only printing on significant changes or verify loop
  // PRINT("\nPublished repeat countdown: ", value);
}

void publishRepeatCountdownState() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) {
    return;
  }
  
  // Calculate current countdown state
  int count = 0;
  
  if (strlen(curMessage) == 0 && strlen(newMessage) == 0) {
      count = 0;
  } else if (atoi(newRepeat) == 0) {
    count = -1; // Infinite
  } else {
    // Finite repeat
    count = atoi(newRepeat) - repeatCount;
    if (count < 0) count = 0;
  }

  publishRepeatCountdownState(count);
}

void publishRepeatState() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) {
    return;
  }

  char topic[256];
  buildStateTopic(topic, sizeof(topic), "repeat");

  // Publish HA RAM value
  char value[16];
  snprintf(value, sizeof(value), "%d", haLastRepeat);
  mqttClient.publish(topic, value, true);
  PRINT("\nPublished repeat state: ", value);
}

void publishScrollDelayState() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) {
    return;
  }

  char topic[256];
  buildStateTopic(topic, sizeof(topic), "scroll_delay");

  // Publish HA RAM value
  char value[16];
  snprintf(value, sizeof(value), "%d", haLastScrollDelay);
  mqttClient.publish(topic, value, true);
  PRINT("\nPublished scroll_delay state: ", value);
}

void publishBuzzerState() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) {
    return;
  }

  char topic[256];
  buildStateTopic(topic, sizeof(topic), "buzzer");

  // Publish HA RAM value
  char value[16];
  snprintf(value, sizeof(value), "%d", haLastBuzzer);
  mqttClient.publish(topic, value, true);
  PRINT("\nPublished buzzer state: ", value);
}

void publishAsciiConvState() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) {
    return;
  }

  char topic[256];
  buildStateTopic(topic, sizeof(topic), "ascii_conv");

  // Publish HA RAM value
  const char* state = (haLastAsciiConv == 1) ? "ON" : "OFF";
  mqttClient.publish(topic, state, true);
  PRINT("\nPublished ascii_conv state: ", state);
}

void publishMqttMessagesState() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) {
    return;
  }

  char topic[256];
  buildStateTopic(topic, sizeof(topic), "mqtt_messages");

  const char* state = (strcmp(mqttMessagesEnable, "on") == 0) ? "ON" : "OFF";
  mqttClient.publish(topic, state, true);
  PRINTS("\nPublished MQTT Messages state");
}

void publishSensorStates() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) {
    return;
  }

  char topic[256];
  char payload[128];

  // Message Displayed
  publishMessageDisplayedState();

  // Hostname
  buildStateTopic(topic, sizeof(topic), "hostname");
  mqttClient.publish(topic, clientId.c_str(), true);

  // IP Address
  buildStateTopic(topic, sizeof(topic), "ip");
  mqttClient.publish(topic, assignedIP, true);

  // RSSI
  buildStateTopic(topic, sizeof(topic), "rssi");
  int rssi = WiFi.RSSI();
  snprintf(payload, sizeof(payload), "%d", rssi);
  mqttClient.publish(topic, payload, true);

  // SSID
  buildStateTopic(topic, sizeof(topic), "ssid");
  mqttClient.publish(topic, WiFi.SSID().c_str(), true);

  // Version
  buildStateTopic(topic, sizeof(topic), "version");
  mqttClient.publish(topic, version.c_str(), true);

  // Uptime (seconds) - for graphs
  buildStateTopic(topic, sizeof(topic), "uptime");
  unsigned long uptime = millis() / 1000;
  snprintf(payload, sizeof(payload), "%lu", uptime);
  mqttClient.publish(topic, payload, true);

  // Uptime (formatted) - for display
  char topicText[256];
  snprintf(topicText, sizeof(topicText), "%s/ha/uptime/formatted", mqttTopicDevice);
  formatUptime(uptime, payload, sizeof(payload)); // Reuse payload buffer
  mqttClient.publish(topicText, payload, true);

  PRINTS("\nPublished sensor states");
}

void publishAllStates() {
  PRINTS("\n=== Publishing All States ===");
  publishLightState();
  delay(20);
  publishMessageState();
  delay(20);
  publishRepeatState();
  delay(20);
  publishScrollDelayState();
  delay(20);
  publishBuzzerState();
  delay(20);
  publishBuzzerEnableState();
  delay(20);
  publishBrightnessOverrideEnableState();
  delay(20);
  publishBrightnessOverrideValueState();
  delay(20);
  publishAlertChirpState();
  delay(20);
  // Switches
  publishAsciiConvState();
  publishMqttMessagesState();
  delay(20);
  publishRepeatCountdownState();
  delay(20);
  // Sensors
  publishSensorStates();
  delay(20);
#ifdef ESP32
  publishTLSStatusState();
  delay(20);
#endif
  publishAllClockStates();
  delay(20);
  publishAllRecurrentAlarmStates();
#ifndef DISABLE_WEATHER_FEATURE
  delay(20);
  publishWeatherState();
#endif
  PRINTS("\n=== All States Published ===");
}

// Periodic telemetry update
void updateTelemetryIfNeeded() {
  // Only update if HA Discovery is enabled and published
  if (strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) {
    return;
  }

  unsigned long now = millis();
  if (now - lastTelemetryUpdate >= telemetryInterval) {
    publishSensorStates();
    lastTelemetryUpdate = now;
  }
}

// ============================================================================
// COMMAND HANDLERS
// ============================================================================

void handleLightCommand(const char* payload) {
  PRINT("\nHandling brightness command: ", payload);

  // Brightness is now just a number value
  int brightness = atoi(payload);
  if (brightness >= 0 && brightness <= 15) {
    // Store in HA RAM only (don't apply)
    haLastBrightness = brightness;
    PRINT("Stored brightness in HA RAM: ", haLastBrightness);
    publishLightState();
  }
}

void handleMessageCommand(const char* payload) {
  PRINT("\nHandling message command: ", payload);

  // Check if MQTT message display is disabled
  if (strcmp(mqttMessagesEnable, "on") != 0) {
    PRINTS("\nMQTT Messages display is disabled - ignoring message update");
    return;
  }

  // Store message in HA RAM only (don't apply)
  strlcpy(haLastMessage, payload, sizeof(haLastMessage));
  PRINT("Stored message in HA RAM: ", haLastMessage);

  // Publish state to reflect stored value
  publishMessageState();
}

void handleRepeatCommand(const char* payload) {
  PRINT("\nHandling repeat command: ", payload);

  int value = atoi(payload);
  if (value >= 0 && value <= 999) {
    // Store in HA RAM only (don't apply)
    haLastRepeat = value;
    PRINT("Stored repeat in HA RAM: ", haLastRepeat);
    publishRepeatState();
  }
}

void handleScrollDelayCommand(const char* payload) {
  PRINT("\nHandling scroll_delay command: ", payload);

  int value = atoi(payload);
  if (value >= 1 && value <= 200) {
    // Store in HA RAM only (don't apply)
    haLastScrollDelay = value;
    PRINT("Stored scroll delay in HA RAM: ", haLastScrollDelay);
    publishScrollDelayState();
  }
}

void handleBuzzerCommand(const char* payload) {
  PRINT("\nHandling buzzer command: ", payload);

  int value = atoi(payload);
  if (value >= 0 && value <= 99) {
    // Store in HA RAM only (don't apply)
    haLastBuzzer = value;
    PRINT("Stored buzzer in HA RAM: ", haLastBuzzer);
    publishBuzzerState();
  }
}

void handleAsciiConvCommand(const char* payload) {
  PRINT("\nHandling ascii_conv command: ", payload);

  // Store in HA RAM only (don't apply)
  if (strcmp(payload, "ON") == 0) {
    haLastAsciiConv = 1;
  } else {
    haLastAsciiConv = 0;
  }
  PRINT("Stored ASCII conv in HA RAM: ", haLastAsciiConv);
  publishAsciiConvState();
}

void handleMqttMessagesCommand(const char* payload) {
  if (strcmp(payload, "ON") == 0) {
    strcpy(newMqttMessagesEnable, "on");
  } else if (strcmp(payload, "OFF") == 0) {
    strcpy(newMqttMessagesEnable, "off");
  } else {
    return;
  }

  newMqttMessagesEnableAvailable = true;

  // Update runtime variable immediately
  strcpy(mqttMessagesEnable, newMqttMessagesEnable);

  // Save to config
  strcpy(mqttConfig.mqttMessagesEnableHolder, newMqttMessagesEnable);
  saveMqttConfiguration(mqttConfigFile, mqttConfig);

  // Publish updated state
  publishMqttMessagesState();

  PRINT("\nMQTT Messages updated via HA: ", mqttMessagesEnable);
}

void handleRebootCommand(const char* payload) {
  PRINT("\nHandling reboot command: ", payload);

  if (strcmp(payload, "PRESS") == 0) {
    PRINTS("\nRebooting device via HA command...");
    delay(100);  // Allow MQTT message to send
    rebootDevice();
  }
}

void handleClearCommand(const char* payload) {
  PRINT("\nHandling clear command: ", payload);

  if (strcmp(payload, "PRESS") == 0) {
    // Check if MQTT message display is disabled
    if (strcmp(mqttMessagesEnable, "on") != 0) {
        PRINTS("\nMQTT Messages display is disabled - ignoring clear command");
        return;
    }

    strcpy(newMessage, "");
    newMessageAvailable = true;
    publishMessageState();
    publishLightState();
    publishRepeatCountdownState(0);
  }
}

void handleSendCommand(const char* payload) {
  PRINT("\nHandling send command: ", payload);

  if (strcmp(payload, "PRESS") == 0) {
    // Check if MQTT message display is disabled
    if (strcmp(mqttMessagesEnable, "on") != 0) {
        PRINTS("\nMQTT Messages display is disabled - ignoring send command");
        return;
    }

    PRINTS("\n=== Sending Message with HA Parameters ===");

    // Apply message from HA RAM
    if (strlen(haLastMessage) > 0) {
      strlcpy(newMessage, haLastMessage, sizeof(newMessage));
      newMessageAvailable = true;
      PRINT("Message: ", haLastMessage);
    } else {
      PRINTS("No message stored in HA RAM");
      return;  // Don't send if no message
    }

    // Apply all stored HA parameters
    if (haLastRepeat >= 0) {
      snprintf(newRepeat, sizeof(newRepeat), "%d", haLastRepeat);
      newRepeatAvailable = true;
      PRINT("Repeat: ", haLastRepeat);
    }

    if (haLastBuzzer >= 0) {
      snprintf(newBuz, sizeof(newBuz), "%d", haLastBuzzer);
      newBuzAvailable = true;
      PRINT("Buzzer: ", haLastBuzzer);
    }

    if (haLastScrollDelay >= 0) {
      snprintf(newDelay, sizeof(newDelay), "%d", haLastScrollDelay);
      newDelayAvailable = true;
      PRINT("Scroll Delay: ", haLastScrollDelay);
    }

    if (haLastBrightness >= 0) {
      snprintf(newBrightness, sizeof(newBrightness), "%d", haLastBrightness);
      newBrightnessAvailable = true;
      PRINT("Brightness: ", haLastBrightness);
    }

    if (haLastAsciiConv >= 0) {
      snprintf(newAsciiConv, sizeof(newAsciiConv), "%d", haLastAsciiConv);
      newAsciiConvAvailable = true;
      PRINT("ASCII Conv: ", haLastAsciiConv);
    }

    repeatCount = 0;
    PRINTS("=== Message Queued for Display ===");

    // States will be published after message displays via displayText()
  }
}

// ============================================================================
// COMMAND SUBSCRIPTION & ROUTING
// ============================================================================

void subscribeToHACommands() {
  if (strcmp(mqttHaDiscovery, "on") != 0) {
    return;  // Don't subscribe if discovery disabled
  }

  char topic[256];

  // Subscribe to all command topics
  snprintf(topic, sizeof(topic), "%s/ha/+/set", mqttTopicDevice);
  mqttClient.subscribe(topic);
  PRINT("\nSubscribed to: ", topic);

  snprintf(topic, sizeof(topic), "%s/ha/+/command", mqttTopicDevice);
  mqttClient.subscribe(topic);
  PRINT("\nSubscribed to: ", topic);
}

bool isHACommand(const char* topic) {
  String topicStr = String(topic);
  String devicePrefix = String(mqttTopicDevice) + "/ha/";

  if (!topicStr.startsWith(devicePrefix)) {
    return false;
  }

  return (topicStr.endsWith("/set") || topicStr.endsWith("/command"));
}

void routeHACommand(const char* topic, const char* payload) {
  // Only process if discovery is enabled
  if (strcmp(mqttHaDiscovery, "on") != 0) {
    return;
  }

  String topicStr = String(topic);

  // Extract entity from topic pattern: <device>/ha/<entity>/set or /command
  int haIndex = topicStr.indexOf("/ha/");
  if (haIndex == -1) return;

  int endIndex = topicStr.indexOf("/", haIndex + 4);
  if (endIndex == -1) return;

  String entity = topicStr.substring(haIndex + 4, endIndex);

  PRINT("\nRouting command for entity: ", entity.c_str());

  if (entity == "brightness") {
    handleLightCommand(payload);  // Still using same handler, just renamed entity
  } else if (entity == "message") {
    handleMessageCommand(payload);
  } else if (entity == "repeat") {
    handleRepeatCommand(payload);
  } else if (entity == "scroll_delay") {
    handleScrollDelayCommand(payload);
  } else if (entity == "buzzer") {
    handleBuzzerCommand(payload);
  } else if (strstr(topic, "/ascii_conv/set")) {
    handleAsciiConvCommand(payload);
  } else if (strstr(topic, "/mqtt_messages/set")) {
    handleMqttMessagesCommand(payload);
  } else if (strstr(topic, "/reboot/command")) {
    handleRebootCommand(payload);
  } else if (entity == "clear") {
    handleClearCommand(payload);
  } else if (entity == "send") {
    handleSendCommand(payload);
#ifndef DISABLE_TIMER_FEATURE
  } else if (entity == "timer_enable") {
    handleTimerEnableCommand(payload);
  } else if (entity == "timer_mode") {
    handleTimerModeCommand(payload);
  } else if (entity == "timer_duration") {
    handleTimerDurationCommand(payload);
  } else if (entity == "timer_start") {
    handleTimerStartCommand();
  } else if (entity == "timer_pause") {
    handleTimerPauseCommand();
  } else if (entity == "timer_stop") {
    handleTimerStopCommand();
  } else if (entity == "timer_reset") {
    handleTimerResetCommand();
  } else if (entity == "timer_brightness") {
    handleTimerBrightnessCommand(payload);
  } else if (entity == "timer_alert_buzzer") {
    handleTimerAlertBuzzerCommand(payload);
  } else if (entity == "timer_auto_repeat") {
    handleTimerAutoRepeatCommand(payload);
  } else if (entity == "timer_alert_chirp") {
    handleTimerAlertChirpCommand(payload);
#endif
#ifndef DISABLE_SLEEP_MODE_FEATURE
  } else if (entity == "sleep_mode_enable") {
    handleSleepModeEnableCommand(payload);
  } else if (entity == "sleep_mode_on_time") {
    handleSleepModeOnTimeCommand(payload);
  } else if (entity == "sleep_mode_off_time") {
    handleSleepModeOffTimeCommand(payload);
  } else if (entity == "sleep_mode_mute_only") {
    handleSleepModeMuteOnlyCommand(payload);
#endif
  } else if (entity == "clock_enable") {
    handleClockEnableCommand(payload);
  } else if (entity == "clock_brightness") {
    handleClockBrightnessCommand(payload);
  } else if (entity == "clock_ntp_server") {
    handleClockNtpServerCommand(payload);
  } else if (entity == "clock_custom_tz") {
    handleClockCustomTzCommand(payload);
  } else if (entity == "clock_date_format") {
    handleClockDateFormatCommand(payload);
  #if MAX_DEVICES == 4
  } else if (entity == "clock_date_alternate") {
    handleClockDateAlternateSecondsCommand(payload);
  #endif
  #if MAX_DEVICES == 8
  } else if (entity == "clock_custom_format") {
    handleClockCustomFormatCommand(payload);
  #endif
  } else if (entity == "clock_transition_delay") {
    handleClockTransitionDelayCommand(payload);
  } else if (entity == "clock_transition_speed") {
    handleClockTransitionSpeedCommand(payload);
  } else if (entity == "clock_transition_effect") {
    handleClockTransitionEffectCommand(payload);
  } else if (entity == "clock_randomize") {
    handleClockRandomizeCommand(payload);
  } else if (entity == "clock_resync_interval") {
    handleClockResyncIntervalCommand(payload);
  } else if (entity == "clock_face") {
    handleClockFaceCommand(payload);
  } else if (entity == "clock_date_alternate_enable") {
    handleClockDateAlternateCommand(payload);
  } else if (entity == "clock_ampm") {
    handleClockAmPmCommand(payload);
  } else if (entity == "buzzer_enable") {
    handleBuzzerEnableCommand(payload);
  } else if (entity == "brightness_override_enable") {
    handleBrightnessOverrideEnableCommand(payload);
  } else if (entity == "brightness_override_value") {
    handleBrightnessOverrideValueCommand(payload);
  } else if (entity == "alert_chirp") {
    handleAlertChirpCommand(payload);
  } else if (entity == "recurrent_alarm_enable") {
    handleRecurrentAlarmEnableCommand(payload);
  } else if (entity == "recurrent_alarm_interval") {
    handleRecurrentAlarmIntervalCommand(payload);
  } else if (entity == "recurrent_alarm_chirp") {
    handleRecurrentAlarmChirpCommand(payload);
  } else if (entity == "recurrent_alarm_disable_weekends") {
    handleRecurrentAlarmDisableWeekendsCommand(payload);
#ifndef DISABLE_TIMER_FEATURE
  } else if (strstr(topic, "/timer_enable/set")) {
    handleTimerEnableCommand(payload);
  } else if (strstr(topic, "/timer_mode/set")) {
    handleTimerModeCommand(payload);
  } else if (strstr(topic, "/timer_duration/set")) {
    handleTimerDurationCommand(payload);
  } else if (strstr(topic, "/timer_start/command")) {
    handleTimerStartCommand();
  } else if (strstr(topic, "/timer_pause/command")) {
    handleTimerPauseCommand();
  } else if (strstr(topic, "/timer_stop/command")) {
    handleTimerStopCommand();
  } else if (strstr(topic, "/timer_reset/command")) {
    handleTimerResetCommand();
  } else if (strstr(topic, "/timer_brightness/set")) {
    handleTimerBrightnessCommand(payload);
  } else if (strstr(topic, "/timer_alert_buzzer/set")) {
    handleTimerAlertBuzzerCommand(payload);
  } else if (strstr(topic, "/timer_auto_repeat/set")) {
    handleTimerAutoRepeatCommand(payload);
  } else if (strstr(topic, "/timer_alert_chirp/set")) {
    handleTimerAlertChirpCommand(payload);
  } else if (strstr(topic, "/timer_alert_beep_count/set")) {
    handleTimerAlertBeepCountCommand(payload);
#endif
  }
}

// ==============================================
// Clock Command Handlers
// ==============================================

void handleClockEnableCommand(const char* payload) {
  PRINT("\nHandling clock_enable command: ", payload);

  bool wasEnabled = clockEnabled;
  bool enabled = (strcmp(payload, "ON") == 0);

  snprintf(clockConfig.enabled, sizeof(clockConfig.enabled), "%s", enabled ? "on" : "off");
  clockEnabled = enabled;
  saveClockConfiguration(clockConfigFile, clockConfig);

  if (clockEnabled && WiFi.status() == WL_CONNECTED) {
    initNtpTime();
  }

  // Handle display transitions
  bool enabledChanged = (wasEnabled != clockEnabled);
  if (enabledChanged && clockEnabled && currentDisplayMode == MODE_MESSAGE) {
    currentDisplayMode = MODE_TRANSITION;
    transitionStartTime = millis();
    P.displayClear();
  }
  if (enabledChanged && !clockEnabled && (currentDisplayMode == MODE_CLOCK || currentDisplayMode == MODE_TRANSITION)) {
    currentDisplayMode = MODE_CLOCK_EXIT;
    // Exit animation
    static char exitTimeString[6];
    int hours, minutes, seconds;
    getNtpTime(hours, minutes, seconds);
    if (!clockNtpSynced) {
      strcpy(exitTimeString, "--:--");
    } else {
      sprintf(exitTimeString, "%02d:%02d", hours, minutes);
    }
    textEffect_t effect;
    if (strcmp(clockConfig.randomizeTransition, "on") == 0) {
      int effectIndex = random(0, 25);
      textEffect_t effects[] = {PA_FADE, PA_DISSOLVE, PA_BLINDS, PA_WIPE, PA_GROW_UP, PA_SCROLL_UP, PA_SCAN_HORIZ, PA_OPENING, PA_CLOSING, PA_SCROLL_DOWN, PA_SCROLL_LEFT, PA_SCROLL_RIGHT, PA_SLICE, PA_MESH, PA_GROW_DOWN, PA_SCAN_VERTX, PA_SCROLL_UP_LEFT, PA_SCROLL_UP_RIGHT, PA_OPENING_CURSOR, PA_SCAN_VERT, PA_SCROLL_DOWN_LEFT, PA_WIPE_CURSOR, PA_SCAN_HORIZX, PA_CLOSING_CURSOR, PA_SCROLL_DOWN_RIGHT};
      effect = effects[effectIndex];
    } else {
      effect = getTransitionEffect(clockConfig.transitionEffect);
    }
    uint16_t animSpeed = (scrollDelay < 40) ? 40 : scrollDelay;
    P.displayText(exitTimeString, PA_CENTER, animSpeed, 0, PA_PRINT, effect);
    P.displayReset();
  }

  publishClockEnableState();
  publishClockDisplayActiveState();
  publishDisplayModeState();
}

void handleClockBrightnessCommand(const char* payload) {
  PRINT("\nHandling clock_brightness command: ", payload);

  int brightness = atoi(payload);
  if (brightness >= 0 && brightness <= 15) {
    snprintf(clockConfig.brightness, sizeof(clockConfig.brightness), "%d", brightness);
    clockBrightness = brightness;
    saveClockConfiguration(clockConfigFile, clockConfig);
    publishClockBrightnessState();
  }
}

void handleClockNtpServerCommand(const char* payload) {
  PRINT("\nHandling clock_ntp_server command: ", payload);

  if (strlen(payload) > 0 && strlen(payload) < STDSIZE) {
    strlcpy(clockConfig.ntpServer, payload, sizeof(clockConfig.ntpServer));
    saveClockConfiguration(clockConfigFile, clockConfig);

    if (clockEnabled && WiFi.status() == WL_CONNECTED) {
      initNtpTime();
    }

    publishClockNtpServerState();
  }
}

void handleClockCustomTzCommand(const char* payload) {
  PRINT("\nHandling clock_custom_tz command: ", payload);

  if (strlen(payload) > 0 && strlen(payload) < STDSIZE) {
    strlcpy(clockConfig.tzString, payload, sizeof(clockConfig.tzString));
    saveClockConfiguration(clockConfigFile, clockConfig);

    // Apply timezone (always works, even if clock display is disabled)
    applyTimezone();

    // Force immediate display update if clock is showing
    if (currentDisplayMode == MODE_CLOCK) {
      displayClock(false);  // Update without animation
    }

    // Publish state
    publishClockCustomTzState();
  }
}

void handleClockDateFormatCommand(const char* payload) {
  PRINT("\nHandling clock_date_format command: ", payload);

  bool valid = false;

  #if MAX_DEVICES == 4
    if (strcmp(payload, "TIME_ONLY") == 0 || strcmp(payload, "TIME_SECONDS") == 0) {
      valid = true;
    }
  #elif MAX_DEVICES == 8
    if (strcmp(payload, "TIME_DATE") == 0 ||
        strcmp(payload, "FULL_DATE") == 0 ||
        strcmp(payload, "TIME_FULL_DATE") == 0 ||
        strcmp(payload, "CUSTOM") == 0) {
      valid = true;
    }
  #endif

  if (valid) {
    strlcpy(clockConfig.dateFormat, payload, sizeof(clockConfig.dateFormat));
    saveClockConfiguration(clockConfigFile, clockConfig);

    clockAlternateState = 0;
    lastDateAlternate = millis();

    publishClockDateFormatState();
  }
}

#if MAX_DEVICES == 4
void handleClockDateAlternateSecondsCommand(const char* payload) {
  PRINT("\nHandling clock_date_alternate command: ", payload);

  int seconds = atoi(payload);
  if (seconds >= 1 && seconds <= 60) {
    snprintf(clockConfig.dateAlternateSeconds, sizeof(clockConfig.dateAlternateSeconds), "%d", seconds);
    saveClockConfiguration(clockConfigFile, clockConfig);
    publishClockDateAlternateSecondsState();
  }
}
#endif

#if MAX_DEVICES == 8
void handleClockCustomFormatCommand(const char* payload) {
  PRINT("\nHandling clock_custom_format command: ", payload);

  if (strlen(payload) > 0 && strlen(payload) < STDSIZE) {
    strlcpy(clockConfig.customDateFormat, payload, sizeof(clockConfig.customDateFormat));
    saveClockConfiguration(clockConfigFile, clockConfig);
    publishClockCustomFormatState();
  }
}
#endif

void handleClockTransitionDelayCommand(const char* payload) {
  PRINT("\nHandling clock_transition_delay command: ", payload);

  int delay = atoi(payload);
  if (delay >= 0 && delay <= 10000) {
    snprintf(clockConfig.transitionDelayMs, sizeof(clockConfig.transitionDelayMs), "%d", delay);
    saveClockConfiguration(clockConfigFile, clockConfig);
    publishClockTransitionDelayState();
  }
}

void handleClockTransitionSpeedCommand(const char* payload) {
  PRINT("\nHandling clock_transition_speed command: ", payload);

  int speed = atoi(payload);
  if (speed >= 10 && speed <= 200) {
    snprintf(clockConfig.transitionSpeed, sizeof(clockConfig.transitionSpeed), "%d", speed);
    saveClockConfiguration(clockConfigFile, clockConfig);
    publishClockTransitionSpeedState();
  }
}

void handleClockTransitionEffectCommand(const char* payload) {
  PRINT("\nHandling clock_transition_effect command: ", payload);

  // Validate effect name
  const char* validEffects[] = {
    "FADE", "DISSOLVE", "BLINDS", "WIPE", "GROW",
    "SCROLL", "SCAN", "OPENING", "CLOSING", "PRINT",
    "SCROLL_DOWN", "SCROLL_LEFT", "SCROLL_RIGHT",
    "SLICE", "MESH", "GROW_DOWN",
    "SCAN_VERTX", "SCROLL_UP_LEFT", "SCROLL_UP_RIGHT", "OPENING_CURSOR",
    "SCAN_VERT", "SCROLL_DOWN_LEFT", "WIPE_CURSOR", "SCAN_HORIZX",
    "CLOSING_CURSOR", "SCROLL_DOWN_RIGHT"
  };
  bool valid = false;
  for (int i = 0; i < 26; i++) {
    if (strcmp(payload, validEffects[i]) == 0) {
      valid = true;
      break;
    }
  }

  if (valid) {
    char previousEffect[STDSIZE];
    strlcpy(previousEffect, clockConfig.transitionEffect, sizeof(previousEffect));
    strlcpy(clockConfig.transitionEffect, payload, sizeof(clockConfig.transitionEffect));
    saveClockConfiguration(clockConfigFile, clockConfig);

    // Retrigger animation if effect changed while clock displaying
    bool effectChanged = (strcmp(previousEffect, clockConfig.transitionEffect) != 0);
    if (effectChanged && clockEnabled && currentDisplayMode == MODE_CLOCK) {
      currentDisplayMode = MODE_TRANSITION;
      transitionStartTime = millis();
      P.displayClear();
    }

    publishClockTransitionEffectState();
  }
}

void handleClockRandomizeCommand(const char* payload) {
  PRINT("\nHandling clock_randomize command: ", payload);

  bool randomize = (strcmp(payload, "ON") == 0);
  snprintf(clockConfig.randomizeTransition, sizeof(clockConfig.randomizeTransition), "%s", randomize ? "on" : "off");
  saveClockConfiguration(clockConfigFile, clockConfig);
  publishClockRandomizeState();
}

void handleClockResyncIntervalCommand(const char* payload) {
  PRINT("\nHandling clock_resync_interval command: ", payload);

  int hours = atoi(payload);
  if (hours >= 1 && hours <= 24) {
    snprintf(clockConfig.resyncIntervalHours, sizeof(clockConfig.resyncIntervalHours), "%d", hours);
    saveClockConfiguration(clockConfigFile, clockConfig);
    publishClockResyncIntervalState();
  }
}

void handleClockFaceCommand(const char* payload) {
  PRINT("\nHandling clock_face command: ", payload);

  if (strcmp(payload, "DEFAULT") == 0 || strcmp(payload, "MATRIX_LIGHT") == 0 || strcmp(payload, "MATRIX_LIGHT_6") == 0) {
    strlcpy(clockConfig.clockFace, payload, sizeof(clockConfig.clockFace));
    saveClockConfiguration(clockConfigFile, clockConfig);
    // Apply immediately if clock is currently displaying
    if (currentDisplayMode == MODE_CLOCK) {
      displayClock(false);
    }
    publishClockFaceState();
  }
}

void handleClockDateAlternateCommand(const char* payload) {
  if (strcmp(payload, "on") == 0 || strcmp(payload, "off") == 0) {
    strlcpy(clockConfig.dateAlternate, payload, sizeof(clockConfig.dateAlternate));
    clockAlternateState = 0;
    saveClockConfiguration(clockConfigFile, clockConfig);
    if (currentDisplayMode == MODE_CLOCK) displayClock(false);
    publishClockDateAlternateState();
  }
}

void handleClockAmPmCommand(const char* payload) {
  if (strcmp(payload, "on") == 0 || strcmp(payload, "off") == 0) {
    strlcpy(clockConfig.clockAmPm, payload, sizeof(clockConfig.clockAmPm));
    saveClockConfiguration(clockConfigFile, clockConfig);
    if (currentDisplayMode == MODE_CLOCK) displayClock(false);
    publishClockAmPmState();
  }
}

// ==============================================
// Clock State Publishers
// ==============================================

void publishClockEnableState() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) return;
  char topic[256];
  buildStateTopic(topic, sizeof(topic), "clock_enable");
  const char* value = clockEnabled ? "ON" : "OFF";
  mqttClient.publish(topic, value, true);
  PRINT("\nPublished clock_enable state: ", value);
}

void publishClockBrightnessState() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) return;
  char topic[256];
  buildStateTopic(topic, sizeof(topic), "clock_brightness");
  char value[8];
  snprintf(value, sizeof(value), "%d", clockBrightness);
  mqttClient.publish(topic, value, true);
  PRINT("\nPublished clock_brightness state: ", value);
}

void publishClockNtpServerState() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) return;
  char topic[256];
  buildStateTopic(topic, sizeof(topic), "clock_ntp_server");
  mqttClient.publish(topic, clockConfig.ntpServer, true);
  PRINT("\nPublished clock_ntp_server state: ", clockConfig.ntpServer);
}

void publishClockCustomTzState() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) return;
  char topic[256];
  buildStateTopic(topic, sizeof(topic), "clock_custom_tz");
  
  // Publish raw POSIX TZ string
  mqttClient.publish(topic, clockConfig.tzString, true);
  PRINT("\nPublished clock_custom_tz state: ", clockConfig.tzString);
}

void publishClockDateFormatState() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) return;
  char topic[256];
  buildStateTopic(topic, sizeof(topic), "clock_date_format");
  mqttClient.publish(topic, clockConfig.dateFormat, true);
  PRINT("\nPublished clock_date_format state: ", clockConfig.dateFormat);
}

void publishClockDateAlternateState() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) return;
  char topic[256];
  buildStateTopic(topic, sizeof(topic), "clock_date_alternate_enable");
  mqttClient.publish(topic, clockConfig.dateAlternate, true);
}

void publishClockAmPmState() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) return;
  char topic[256];
  buildStateTopic(topic, sizeof(topic), "clock_ampm");
  mqttClient.publish(topic, clockConfig.clockAmPm, true);
}

#if MAX_DEVICES == 4
void publishClockDateAlternateSecondsState() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) return;
  char topic[256];
  buildStateTopic(topic, sizeof(topic), "clock_date_alternate");
  mqttClient.publish(topic, clockConfig.dateAlternateSeconds, true);
  PRINT("\nPublished clock_date_alternate state: ", clockConfig.dateAlternateSeconds);
}
#endif

#if MAX_DEVICES == 8
void publishClockCustomFormatState() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) return;
  char topic[256];
  buildStateTopic(topic, sizeof(topic), "clock_custom_format");
  mqttClient.publish(topic, clockConfig.customDateFormat, true);
  PRINT("\nPublished clock_custom_format state: ", clockConfig.customDateFormat);
}
#endif

void publishClockTransitionDelayState() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) return;
  char topic[256];
  buildStateTopic(topic, sizeof(topic), "clock_transition_delay");
  mqttClient.publish(topic, clockConfig.transitionDelayMs, true);
  PRINT("\nPublished clock_transition_delay state: ", clockConfig.transitionDelayMs);
}

void publishClockTransitionSpeedState() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) return;
  char topic[256];
  buildStateTopic(topic, sizeof(topic), "clock_transition_speed");
  mqttClient.publish(topic, clockConfig.transitionSpeed, true);
  PRINT("\nPublished clock_transition_speed state: ", clockConfig.transitionSpeed);
}

void publishClockTransitionEffectState() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) return;
  char topic[256];
  buildStateTopic(topic, sizeof(topic), "clock_transition_effect");
  mqttClient.publish(topic, clockConfig.transitionEffect, true);
  PRINT("\nPublished clock_transition_effect state: ", clockConfig.transitionEffect);
}

void publishClockRandomizeState() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) return;
  char topic[256];
  buildStateTopic(topic, sizeof(topic), "clock_randomize");
  const char* value = (strcmp(clockConfig.randomizeTransition, "on") == 0) ? "ON" : "OFF";
  mqttClient.publish(topic, value, true);
  PRINT("\nPublished clock_randomize state: ", value);
}

void publishClockResyncIntervalState() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) return;
  char topic[256];
  buildStateTopic(topic, sizeof(topic), "clock_resync_interval");
  mqttClient.publish(topic, clockConfig.resyncIntervalHours, true);
  PRINT("\nPublished clock_resync_interval state: ", clockConfig.resyncIntervalHours);
}

void publishClockFaceState() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) return;
  char topic[256];
  buildStateTopic(topic, sizeof(topic), "clock_face");
  mqttClient.publish(topic, clockConfig.clockFace, true);
  PRINT("\nPublished clock_face state: ", clockConfig.clockFace);
}

void publishClockTimeState() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) return;
  char topic[256];
  buildStateTopic(topic, sizeof(topic), "clock_time");
  char timeString[6];
  int hours, minutes, seconds;
  getNtpTime(hours, minutes, seconds);
  if (!clockNtpSynced) {
    strcpy(timeString, "--:--");
  } else {
    sprintf(timeString, "%02d:%02d", hours, minutes);
  }
  mqttClient.publish(topic, timeString, true);
  PRINT("\nPublished clock_time state: ", timeString);
}

void publishClockNtpSyncedState() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) return;
  char topic[256];
  buildStateTopic(topic, sizeof(topic), "clock_ntp_synced");
  const char* value = clockNtpSynced ? "ON" : "OFF";
  mqttClient.publish(topic, value, true);
  PRINT("\nPublished clock_ntp_synced state: ", value);
}

void publishClockDisplayActiveState() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) return;
  char topic[256];
  buildStateTopic(topic, sizeof(topic), "clock_display_active");
  const char* value = (currentDisplayMode == MODE_CLOCK || currentDisplayMode == MODE_TRANSITION) ? "ON" : "OFF";
  mqttClient.publish(topic, value, true);
  PRINT("\nPublished clock_display_active state: ", value);
}

void publishDisplayModeState() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) return;
  char topic[256];
  buildStateTopic(topic, sizeof(topic), "display_mode");
  const char* modeStr = "UNKNOWN";
  switch (currentDisplayMode) {
    case MODE_MESSAGE: modeStr = "MESSAGE"; break;
    case MODE_TRANSITION: modeStr = "TRANSITION"; break;
    case MODE_CLOCK: modeStr = "CLOCK"; break;
    case MODE_CLOCK_EXIT: modeStr = "CLOCK_EXIT"; break;
#ifndef DISABLE_TIMER_FEATURE
    case MODE_TIMER: modeStr = "TIMER"; break;
    case MODE_TIMER_EXIT: modeStr = "TIMER_EXIT"; break;
#endif
#ifndef DISABLE_ALARM_FEATURE
    case MODE_ALARM: modeStr = "ALARM"; break;
    case MODE_ALARM_EXIT: modeStr = "ALARM_EXIT"; break;
#endif
#ifndef DISABLE_WEATHER_FEATURE
    case MODE_WEATHER: modeStr = "WEATHER"; break;
    case MODE_WEATHER_EXIT: modeStr = "WEATHER_EXIT"; break;
#endif
  }
  mqttClient.publish(topic, modeStr, true);
  PRINT("\nPublished display_mode state: ", modeStr);
}

#ifdef ESP32
void publishTLSStatusState() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) return;
  char topic[256];
  buildStateTopic(topic, sizeof(topic), "tls_status");
  const char* value = tlsEnabled ? "ON" : "OFF";
  mqttClient.publish(topic, value, true);
  PRINT("\nPublished TLS status state: ", value);
}
#endif

void publishAllClockStates() {
  publishClockEnableState();
  publishClockBrightnessState();
#ifndef DISABLE_HA_CLOCK_ADVANCED
  publishClockNtpServerState();
  publishClockCustomTzState();
#endif
  publishClockDateFormatState();
  #if MAX_DEVICES == 4
  publishClockDateAlternateSecondsState();
  #endif
  #if MAX_DEVICES == 8
  publishClockCustomFormatState();
  #endif
#ifndef DISABLE_HA_CLOCK_ADVANCED
  publishClockTransitionDelayState();
  publishClockTransitionSpeedState();
  publishClockTransitionEffectState();
  publishClockRandomizeState();
  publishClockResyncIntervalState();
#endif
  publishClockFaceState();
  publishClockDateAlternateState();
  publishClockAmPmState();
  publishClockTimeState();
  publishClockNtpSyncedState();
  publishClockDisplayActiveState();
  publishDisplayModeState();
}

// ============================================================================
// BUZZER GLOBAL ENABLE FUNCTIONS
// ============================================================================

void publishBuzzerEnableDiscovery() {
  JsonDocument doc;
  char baseTopic[256];
  snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
  doc["~"] = baseTopic;

  doc["name"] = "Buzzer Enable";
  doc["uniq_id"] = String(haBaseTopic) + "_buzzer_enable";
  doc["cmd_t"] = "~/buzzer_enable/set";
  doc["stat_t"] = "~/buzzer_enable/state";
  doc["payload_on"] = "ON";
  doc["payload_off"] = "OFF";
  doc["icon"] = "mdi:volume-high";
  doc["ent_cat"] = "config";
  
  addDeviceInfo(doc);
  addAvailability(doc);

  char topic[256];
  buildDiscoveryTopic(topic, sizeof(topic), "switch", "buzzer_enable");

  char payload[1024];
  serializeJson(doc, payload, sizeof(payload));
  mqttClient.publish(topic, payload, true);
  PRINT("\nPublished buzzer enable discovery to: ", topic);
}

void publishBuzzerEnableState() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) {
    return;
  }

  char topic[256];
  // buildStateTopic uses "ha/<entity>/state" pattern usually via buildStateTopic(..., "buzzer_enable")
  // but let's check buildStateTopic impl. It appends "/state" if not provided? 
  // buildStateTopic implementation: snprintf(buffer, bufferSize, "%s/ha/%s/state", mqttTopicDevice, entity);
  buildStateTopic(topic, sizeof(topic), "buzzer_enable");

  const char* state = (strcmp(generalConfig.buzzerEnable, "on") == 0) ? "ON" : "OFF";
  mqttClient.publish(topic, state, true);
  PRINT("\nPublished buzzer enable state: ", state);
}

void handleBuzzerEnableCommand(const char* payload) {
  PRINT("\nHandle Buzzer Enable Command: ", payload);

  bool stateChanged = false;
  if (strcasecmp(payload, "ON") == 0) {
    if (strcmp(generalConfig.buzzerEnable, "on") != 0) {
      strlcpy(generalConfig.buzzerEnable, "on", sizeof(generalConfig.buzzerEnable));
      stateChanged = true;
    }
  } else if (strcasecmp(payload, "OFF") == 0) {
    if (strcmp(generalConfig.buzzerEnable, "off") != 0) {
      strlcpy(generalConfig.buzzerEnable, "off", sizeof(generalConfig.buzzerEnable));
      stateChanged = true;
    }
  }

  if (stateChanged) {
    strlcpy(newBuzzerEnable, generalConfig.buzzerEnable, sizeof(newBuzzerEnable));
    newBuzzerEnableAvailable = true;
    
    // Save to persistent storage
    saveGeneralConfiguration(generalConfigFile, generalConfig);
    
    // Publish new state
    publishBuzzerEnableState();
  }
}

// ============================================================================
// BRIGHTNESS OVERRIDE ENABLE (SWITCH)
// ============================================================================

void publishBrightnessOverrideEnableDiscovery() {
  JsonDocument doc;
  char baseTopic[256];
  snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
  doc["~"] = baseTopic;

  doc["name"] = "Brightness Override Enable";
  doc["uniq_id"] = String(haBaseTopic) + "_brightness_override_enable";
  doc["cmd_t"] = "~/brightness_override_enable/set";
  doc["stat_t"] = "~/brightness_override_enable/state";
  doc["payload_on"] = "ON";
  doc["payload_off"] = "OFF";
  doc["icon"] = "mdi:brightness-auto";
  doc["ent_cat"] = "config";
  
  addDeviceInfo(doc);
  addAvailability(doc);

  char topic[256];
  buildDiscoveryTopic(topic, sizeof(topic), "switch", "brightness_override_enable");

  char payload[1024];
  serializeJson(doc, payload, sizeof(payload));
  mqttClient.publish(topic, payload, true);
  PRINT("\nPublished brightness override enable discovery to: ", topic);
}

void publishBrightnessOverrideEnableState() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) {
    return;
  }

  char topic[256];
  buildStateTopic(topic, sizeof(topic), "brightness_override_enable");

  const char* state = (strcmp(generalConfig.brightnessOverrideEnable, "on") == 0) ? "ON" : "OFF";
  mqttClient.publish(topic, state, true);
  PRINT("\nPublished brightness override enable state: ", state);
}

void handleBrightnessOverrideEnableCommand(const char* payload) {
  PRINT("\nHandle Brightness Override Enable Command: ", payload);

  bool stateChanged = false;
  if (strcasecmp(payload, "ON") == 0) {
    if (strcmp(generalConfig.brightnessOverrideEnable, "on") != 0) {
      strlcpy(generalConfig.brightnessOverrideEnable, "on", sizeof(generalConfig.brightnessOverrideEnable));
      stateChanged = true;
    }
  } else if (strcasecmp(payload, "OFF") == 0) {
    if (strcmp(generalConfig.brightnessOverrideEnable, "off") != 0) {
      strlcpy(generalConfig.brightnessOverrideEnable, "off", sizeof(generalConfig.brightnessOverrideEnable));
      stateChanged = true;
    }
  }

  if (stateChanged) {
    // Update global runtime variable
    brightnessOverrideEnabled = (strcmp(generalConfig.brightnessOverrideEnable, "on") == 0);
    
    // Save to persistent storage
    saveGeneralConfiguration(generalConfigFile, generalConfig);
    
    // Publish new state
    publishBrightnessOverrideEnableState();
  }
}

// ============================================================================
// BRIGHTNESS OVERRIDE VALUE (NUMBER)
// ============================================================================

void publishBrightnessOverrideValueDiscovery() {
  JsonDocument doc;
  char baseTopic[256];
  snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
  doc["~"] = baseTopic;

  doc["name"] = "Brightness Override Value";
  doc["uniq_id"] = String(haBaseTopic) + "_brightness_override_value";
  doc["cmd_t"] = "~/brightness_override_value/set";
  doc["stat_t"] = "~/brightness_override_value/state";
  doc["min"] = 0;
  doc["max"] = 15;
  doc["step"] = 1;
  doc["mode"] = "slider";
  doc["icon"] = "mdi:brightness-6";
  doc["ent_cat"] = "config";
  
  addDeviceInfo(doc);
  addAvailability(doc);

  char topic[256];
  buildDiscoveryTopic(topic, sizeof(topic), "number", "brightness_override_value");

  char payload[1024];
  serializeJson(doc, payload, sizeof(payload));
  mqttClient.publish(topic, payload, true);
  PRINT("\nPublished brightness override value discovery to: ", topic);
}

void publishBrightnessOverrideValueState() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) {
    return;
  }

  char topic[256];
  buildStateTopic(topic, sizeof(topic), "brightness_override_value");

  char value[16];
  snprintf(value, sizeof(value), "%s", generalConfig.brightnessOverrideValue);
  mqttClient.publish(topic, value, true);
  PRINT("\nPublished brightness override value state: ", value);
}

void handleBrightnessOverrideValueCommand(const char* payload) {
  PRINT("\nHandle Brightness Override Value Command: ", payload);

  int value = atoi(payload);

  // Validate range (0-15)
  if (value < 0 || value > 15) {
    PRINTS("\nError: Brightness override value out of range (0-15)");
    return;
  }

  // Convert to string and check if changed
  char valueStr[BRI_SIZE];
  snprintf(valueStr, sizeof(valueStr), "%d", value);

  if (strcmp(generalConfig.brightnessOverrideValue, valueStr) != 0) {
    strlcpy(generalConfig.brightnessOverrideValue, valueStr, sizeof(generalConfig.brightnessOverrideValue));

    // Update global runtime variable
    brightnessOverrideValue = value;

    // Save to persistent storage
    saveGeneralConfiguration(generalConfigFile, generalConfig);

    // Publish new state
    publishBrightnessOverrideValueState();
  }
}

// ============================================================================
// ALERT CHIRP SELECT ENTITY
// ============================================================================

void publishAlertChirpDiscovery() {
  JsonDocument doc;
  char baseTopic[256];
  snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);
  doc["~"] = baseTopic;

  doc["name"] = "Message Sound";
  doc["uniq_id"] = String(haBaseTopic) + "_alert_chirp";
  doc["cmd_t"] = "~/alert_chirp/set";
  doc["stat_t"] = "~/alert_chirp/state";
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

  addDeviceInfo(doc);
  addAvailability(doc);

  char topic[256];
  buildDiscoveryTopic(topic, sizeof(topic), "select", "alert_chirp");

  char payload[2048];  // Larger buffer for options list
  serializeJson(doc, payload, sizeof(payload));
  mqttClient.publish(topic, payload, true);
  PRINT("\nPublished alert chirp discovery to: ", topic);
}

void publishAlertChirpState() {
  if (!mqttClient.connected() || strcmp(mqttHaDiscovery, "on") != 0 || !mqttDiscoveryPublished) {
    return;
  }

  char topic[256];
  buildStateTopic(topic, sizeof(topic), "alert_chirp");
  mqttClient.publish(topic, alertChirpDefault, true);
  PRINT("\nPublished alert chirp state: ", alertChirpDefault);
}

void handleAlertChirpCommand(const char* payload) {
  PRINT("\nHandle Alert Chirp Command: ", payload);

  // Validate that the chirp name exists
  const ChirpPattern* pattern = getChirpByName(payload);
  if (pattern == nullptr) {
    PRINT("\nError: Unknown chirp pattern: ", payload);
    return;
  }

  // Update the default alert chirp
  strlcpy(alertChirpDefault, payload, sizeof(alertChirpDefault));
  strlcpy(defaultsConfig.alertChirpDefaultHolder, payload, sizeof(defaultsConfig.alertChirpDefaultHolder));

  // Save to persistent storage
  saveDefaultsConfiguration(defaultsConfigFile, defaultsConfig);

  // Publish new state
  publishAlertChirpState();

  PRINT("\nAlert chirp updated to: ", alertChirpDefault);
}
