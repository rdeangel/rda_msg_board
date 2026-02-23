#ifndef DISABLE_SLEEP_MODE_FEATURE
#include "mqtt_discovery_sleep.h"
#include "mqtt_discovery_core.h"
#include "globals.h"
#include "mqtt.h"
#include "functions.h"
#include "config_manager.h"

// Publish all sleep mode MQTT discovery messages
void publishSleepModeDiscoveries() {
  if (strcmp(mqttOnOff, "on") != 0 || strcmp(mqttHaDiscovery, "on") != 0) {
    return;
  }

  char baseTopic[256];
  snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);

  // Sleep Mode Enable Switch
  {
    JsonDocument doc;
    doc["~"] = baseTopic;
    doc["name"] = "Sleep Mode Enable";
    doc["uniq_id"] = String(haBaseTopic) + "_sleep_mode_enable";
    doc["cmd_t"] = "~/sleep_mode_enable/set";
    doc["stat_t"] = "~/sleep_mode_enable/state";
    doc["payload_on"] = "ON";
    doc["payload_off"] = "OFF";
    doc["icon"] = "mdi:sleep";
    doc["ent_cat"] = "config";

    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "switch", "sleep_mode_enable");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished sleep_mode_enable discovery to: ", topic);
  }

  // Sleep Mode On Time
  {
    JsonDocument doc;
    doc["~"] = baseTopic;
    doc["name"] = "Sleep Mode On Time";
    doc["uniq_id"] = String(haBaseTopic) + "_sleep_mode_on_time";
    doc["cmd_t"] = "~/sleep_mode_on_time/set";
    doc["stat_t"] = "~/sleep_mode_on_time/state";
    doc["icon"] = "mdi:clock-start";
    doc["ent_cat"] = "config";

    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "time", "sleep_mode_on_time");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished sleep_mode_on_time discovery to: ", topic);
  }

  // Sleep Mode Off Time
  {
    JsonDocument doc;
    doc["~"] = baseTopic;
    doc["name"] = "Sleep Mode Off Time";
    doc["uniq_id"] = String(haBaseTopic) + "_sleep_mode_off_time";
    doc["cmd_t"] = "~/sleep_mode_off_time/set";
    doc["stat_t"] = "~/sleep_mode_off_time/state";
    doc["icon"] = "mdi:clock-end";
    doc["ent_cat"] = "config";

    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "time", "sleep_mode_off_time");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished sleep_mode_off_time discovery to: ", topic);
  }
  
  // Sleep Mode Mute Only Switch
  {
    JsonDocument doc;
    doc["~"] = baseTopic;
    doc["name"] = "Sleep Mode Mute Only";
    doc["uniq_id"] = String(haBaseTopic) + "_sleep_mode_mute_only";
    doc["cmd_t"] = "~/sleep_mode_mute_only/set";
    doc["stat_t"] = "~/sleep_mode_mute_only/state";
    doc["payload_on"] = "ON";
    doc["payload_off"] = "OFF";
    doc["icon"] = "mdi:volume-off";
    doc["ent_cat"] = "config";

    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "switch", "sleep_mode_mute_only");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished sleep_mode_mute_only discovery to: ", topic);
  }

  // Sleep Mode Status Binary Sensor
  {
    JsonDocument doc;
    doc["~"] = baseTopic;
    doc["name"] = "Sleep Mode Status";
    doc["uniq_id"] = String(haBaseTopic) + "_sleep_mode_status";
    doc["stat_t"] = "~/sleep_mode_status/state";
    doc["payload_on"] = "Active";
    doc["payload_off"] = "Inactive";
    doc["dev_cla"] = "running";
    doc["icon"] = "mdi:sleep";
    doc["ent_cat"] = "diagnostic";

    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "binary_sensor", "sleep_mode_status");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished sleep_mode_status discovery to: ", topic);
  }

  Serial.println(F("Sleep Mode MQTT discoveries published"));

  // Publish initial states (all at once for consistency)
  publishSleepModeState();
}

// State Publishers
void publishSleepModeEnableState() {
  if (strcmp(mqttOnOff, "on") != 0) return;
  char topic[256];
  buildStateTopic(topic, sizeof(topic), "sleep_mode_enable");
  mqttClient.publish(topic, sleepModeEnabled ? "ON" : "OFF", true);
}

void publishSleepModeOnTimeState() {
  if (strcmp(mqttOnOff, "on") != 0) return;
  char topic[256];
  buildStateTopic(topic, sizeof(topic), "sleep_mode_on_time");
  mqttClient.publish(topic, sleepModeConfig.onTime, true);
}

void publishSleepModeOffTimeState() {
  if (strcmp(mqttOnOff, "on") != 0) return;
  char topic[256];
  buildStateTopic(topic, sizeof(topic), "sleep_mode_off_time");
  mqttClient.publish(topic, sleepModeConfig.offTime, true);
}

void publishSleepModeMuteOnlyState() {
  if (strcmp(mqttOnOff, "on") != 0) return;
  char topic[256];
  buildStateTopic(topic, sizeof(topic), "sleep_mode_mute_only");
  mqttClient.publish(topic, sleepModeMuteOnly ? "ON" : "OFF", true);
}

void publishSleepModeStatusState() {
  if (strcmp(mqttOnOff, "on") != 0) return;
  char topic[256];
  buildStateTopic(topic, sizeof(topic), "sleep_mode_status");
  mqttClient.publish(topic, sleepModeActive ? "Active" : "Inactive", true);
}

// Publish all sleep mode states
void publishSleepModeState() {
  publishSleepModeEnableState();
  publishSleepModeOnTimeState();
  publishSleepModeOffTimeState();
  publishSleepModeMuteOnlyState();
  publishSleepModeStatusState();
}

// Command Handlers
void handleSleepModeEnableCommand(const char* payload) {
  bool newEnabled = (strcasecmp(payload, "ON") == 0);
  strlcpy(sleepModeConfig.enabled, newEnabled ? "on" : "off", sizeof(sleepModeConfig.enabled));
  sleepModeEnabled = newEnabled;

  saveSleepModeConfiguration(sleepModeConfigFile, sleepModeConfig);

  // Recalculate active status based on current time window before publishing
  updateSleepModeState();

  // Publish all sleep mode states for full sync
  publishSleepModeState();

  Serial.print(F("Sleep Mode enabled: "));
  Serial.println(sleepModeEnabled ? "ON" : "OFF");
}

void handleSleepModeOnTimeCommand(const char* payload) {
  // Validate payload format (HH:MM)
  if (strlen(payload) < 4 || strlen(payload) > 5 || strchr(payload, ':') == NULL) {
    Serial.println(F("Invalid sleep mode on time format"));
    return;
  }

  // Additional validation for hour and minute values
  int hour = 0, minute = 0;
  if (sscanf(payload, "%d:%d", &hour, &minute) != 2 ||
      hour < 0 || hour > 23 || minute < 0 || minute > 59) {
    Serial.println(F("Invalid sleep mode on time values"));
    return;
  }

  strlcpy(sleepModeConfig.onTime, payload, sizeof(sleepModeConfig.onTime));
  saveSleepModeConfiguration(sleepModeConfigFile, sleepModeConfig);

  // Recalculate active status
  updateSleepModeState();

  // Publish all sleep mode states for full sync
  publishSleepModeState();

  Serial.print(F("Sleep Mode on time: "));
  Serial.println(sleepModeConfig.onTime);
}

void handleSleepModeOffTimeCommand(const char* payload) {
  // Validate payload format (HH:MM)
  if (strlen(payload) < 4 || strlen(payload) > 5 || strchr(payload, ':') == NULL) {
    Serial.println(F("Invalid sleep mode off time format"));
    return;
  }

  // Additional validation for hour and minute values
  int hour = 0, minute = 0;
  if (sscanf(payload, "%d:%d", &hour, &minute) != 2 ||
      hour < 0 || hour > 23 || minute < 0 || minute > 59) {
    Serial.println(F("Invalid sleep mode off time values"));
    return;
  }

  strlcpy(sleepModeConfig.offTime, payload, sizeof(sleepModeConfig.offTime));
  saveSleepModeConfiguration(sleepModeConfigFile, sleepModeConfig);

  // Recalculate active status
  updateSleepModeState();

  // Publish all sleep mode states for full sync
  publishSleepModeState();

  Serial.print(F("Sleep Mode off time: "));
  Serial.println(sleepModeConfig.offTime);
}

void handleSleepModeMuteOnlyCommand(const char* payload) {
  bool newMuteOnly = (strcasecmp(payload, "ON") == 0);
  strlcpy(sleepModeConfig.muteOnly, newMuteOnly ? "on" : "off", sizeof(sleepModeConfig.muteOnly));
  sleepModeMuteOnly = newMuteOnly;

  saveSleepModeConfiguration(sleepModeConfigFile, sleepModeConfig);

  // Recalculate active status
  updateSleepModeState();

  // Publish all sleep mode states for full sync
  publishSleepModeState();

  Serial.print(F("Sleep Mode mute only: "));
  Serial.println(sleepModeMuteOnly ? "ON" : "OFF");
}

#endif // DISABLE_SLEEP_MODE_FEATURE
