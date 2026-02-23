#ifndef DISABLE_TIMER_FEATURE
#include "mqtt_discovery_timer.h"
#include "mqtt_discovery_core.h"
#include "globals.h"
#include "mqtt.h"
#include "timer.h"
#include "config_manager.h"
#include "chirp_library.h"

// Publish all timer MQTT discovery messages
void publishTimerDiscoveries() {
  if (strcmp(mqttOnOff, "on") != 0 || strcmp(mqttHaDiscovery, "on") != 0) {
    return;
  }

  char baseTopic[256];
  snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);

  // Timer Enable Switch
  {
    JsonDocument doc;
    doc["~"] = baseTopic;
    doc["name"] = "Timer Enable";
    doc["uniq_id"] = String(haBaseTopic) + "_timer_enable";
    doc["cmd_t"] = "~/timer_enable/set";
    doc["stat_t"] = "~/timer_enable/state";
    doc["payload_on"] = "ON";
    doc["payload_off"] = "OFF";
    doc["icon"] = "mdi:timer-outline";
    
    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "switch", "timer_enable");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished timer_enable discovery to: ", topic);
  }
  
  // Timer Mode Select
  {
    JsonDocument doc;
    doc["~"] = baseTopic;
    doc["name"] = "Timer Mode";
    doc["uniq_id"] = String(haBaseTopic) + "_timer_mode";
    doc["cmd_t"] = "~/timer_mode/set";
    doc["stat_t"] = "~/timer_mode/state";
    doc["icon"] = "mdi:timer-cog-outline";
    
    JsonArray options = doc["options"].to<JsonArray>();
    options.add("countdown");
    options.add("stopwatch");

    addDeviceInfo(doc);
    addAvailability(doc);
    
    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "select", "timer_mode");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished timer_mode discovery to: ", topic);
  }
  
  // Timer Duration Number
  {
    JsonDocument doc;
    doc["~"] = baseTopic;
    doc["name"] = "Timer Duration";
    doc["uniq_id"] = String(haBaseTopic) + "_timer_duration";
    doc["cmd_t"] = "~/timer_duration/set";
    doc["stat_t"] = "~/timer_duration/state";
    doc["min"] = 1;
    doc["max"] = 86400;
    doc["mode"] = "box";
    doc["unit_of_meas"] = "s";
    doc["icon"] = "mdi:timer-sand";

    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "number", "timer_duration");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished timer_duration discovery to: ", topic);
  }
  
  // Timer Start Button
  {
    JsonDocument doc;
    doc["~"] = baseTopic;
    doc["name"] = "Timer Start";
    doc["uniq_id"] = String(haBaseTopic) + "_timer_start";
    doc["cmd_t"] = "~/timer_start/command";
    doc["payload_press"] = "PRESS";
    doc["icon"] = "mdi:play";

    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "button", "timer_start");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished timer_start discovery to: ", topic);
  }
  
  // Timer Pause Button
  {
    JsonDocument doc;
    doc["~"] = baseTopic;
    doc["name"] = "Timer Pause";
    doc["uniq_id"] = String(haBaseTopic) + "_timer_pause";
    doc["cmd_t"] = "~/timer_pause/command";
    doc["payload_press"] = "PRESS";
    doc["icon"] = "mdi:pause";

    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "button", "timer_pause");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished timer_pause discovery to: ", topic);
  }
  
  // Timer Stop Button
  {
    JsonDocument doc;
    doc["~"] = baseTopic;
    doc["name"] = "Timer Stop";
    doc["uniq_id"] = String(haBaseTopic) + "_timer_stop";
    doc["cmd_t"] = "~/timer_stop/command";
    doc["payload_press"] = "PRESS";
    doc["icon"] = "mdi:stop";

    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "button", "timer_stop");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished timer_stop discovery to: ", topic);
  }
  
  // Timer Reset Button
  {
    JsonDocument doc;
    doc["~"] = baseTopic;
    doc["name"] = "Timer Reset";
    doc["uniq_id"] = String(haBaseTopic) + "_timer_reset";
    doc["cmd_t"] = "~/timer_reset/command";
    doc["payload_press"] = "PRESS";
    doc["icon"] = "mdi:restart";

    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "button", "timer_reset");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished timer_reset discovery to: ", topic);
  }
  
  // Timer Brightness Number
  {
    JsonDocument doc;
    doc["~"] = baseTopic;
    doc["name"] = "Timer Brightness";
    doc["uniq_id"] = String(haBaseTopic) + "_timer_brightness";
    doc["cmd_t"] = "~/timer_brightness/set";
    doc["stat_t"] = "~/timer_brightness/state";
    doc["min"] = 0;
    doc["max"] = 15;
    doc["mode"] = "slider";
    doc["icon"] = "mdi:brightness-5";

    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "number", "timer_brightness");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished timer_brightness discovery to: ", topic);
  }
  
  // Timer Alert Beep Count Number (for Simple Beep mode)
  {
    JsonDocument doc;
    doc["~"] = baseTopic;
    doc["name"] = "Timer Alert Beep Count";
    doc["uniq_id"] = String(haBaseTopic) + "_timer_alert_beep_count";
    doc["cmd_t"] = "~/timer_alert_beep_count/set";
    doc["stat_t"] = "~/timer_alert_beep_count/state";
    doc["min"] = 1;
    doc["max"] = 20;
    doc["mode"] = "box";
    doc["icon"] = "mdi:counter";

    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "number", "timer_alert_beep_count");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished timer_alert_beep_count discovery to: ", topic);
  }
  
  // Timer Alert Buzzer Switch
  {
    JsonDocument doc;
    doc["~"] = baseTopic;
    doc["name"] = "Timer Alert Buzzer";
    doc["uniq_id"] = String(haBaseTopic) + "_timer_alert_buzzer";
    doc["cmd_t"] = "~/timer_alert_buzzer/set";
    doc["stat_t"] = "~/timer_alert_buzzer/state";
    doc["payload_on"] = "ON";
    doc["payload_off"] = "OFF";
    doc["icon"] = "mdi:bell-ring-outline";

    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "switch", "timer_alert_buzzer");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished timer_alert_buzzer discovery to: ", topic);
  }
  
  // Timer Auto Repeat Switch
  {
    JsonDocument doc;
    doc["~"] = baseTopic;
    doc["name"] = "Timer Auto Repeat";
    doc["uniq_id"] = String(haBaseTopic) + "_timer_auto_repeat";
    doc["cmd_t"] = "~/timer_auto_repeat/set";
    doc["stat_t"] = "~/timer_auto_repeat/state";
    doc["payload_on"] = "ON";
    doc["payload_off"] = "OFF";
    doc["icon"] = "mdi:repeat";

    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "switch", "timer_auto_repeat");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished timer_auto_repeat discovery to: ", topic);
  }

  // Timer Alert Chirp Select
  {
    JsonDocument doc;
    doc["~"] = baseTopic;
    doc["name"] = "Timer Alert Sound";
    doc["uniq_id"] = String(haBaseTopic) + "_timer_alert_chirp";
    doc["cmd_t"] = "~/timer_alert_chirp/set";
    doc["stat_t"] = "~/timer_alert_chirp/state";
    doc["icon"] = "mdi:music-note";

    // Add all chirp options from the library
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
    buildDiscoveryTopic(topic, sizeof(topic), "select", "timer_alert_chirp");
    char payload[1024]; // Buffer for chirp list
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished timer_alert_chirp discovery to: ", topic);
  }

  // Timer State Sensor
  {
    JsonDocument doc;
    doc["~"] = baseTopic;
    doc["name"] = "Timer State";
    doc["uniq_id"] = String(haBaseTopic) + "_timer_state";
    doc["stat_t"] = "~/timer_state/state";
    doc["icon"] = "mdi:timer-outline";

    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "sensor", "timer_state");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished timer_state discovery to: ", topic);
  }
  
  // Timer Display Sensor
  {
    JsonDocument doc;
    doc["~"] = baseTopic;
    doc["name"] = "Timer Display";
    doc["uniq_id"] = String(haBaseTopic) + "_timer_display";
    doc["stat_t"] = "~/timer_display/state";
    doc["icon"] = "mdi:clock-digital";

    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "sensor", "timer_display");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished timer_display discovery to: ", topic);
  }
  
  // Timer Remaining Sensor
  {
    JsonDocument doc;
    doc["~"] = baseTopic;
    doc["name"] = "Timer Remaining";
    doc["uniq_id"] = String(haBaseTopic) + "_timer_remaining";
    doc["stat_t"] = "~/timer_remaining/state";
    doc["unit_of_meas"] = "s";
    doc["icon"] = "mdi:timer-sand";

    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "sensor", "timer_remaining");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished timer_remaining discovery to: ", topic);
  }
  
  // Timer Elapsed Sensor
  {
    JsonDocument doc;
    doc["~"] = baseTopic;
    doc["name"] = "Timer Elapsed";
    doc["uniq_id"] = String(haBaseTopic) + "_timer_elapsed";
    doc["stat_t"] = "~/timer_elapsed/state";
    doc["unit_of_meas"] = "s";
    doc["icon"] = "mdi:timer-sand-complete";

    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "sensor", "timer_elapsed");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished timer_elapsed discovery to: ", topic);
  }
  
  Serial.println(F("Timer MQTT discoveries published"));

  // Publish initial states
  publishTimerEnableState();
  publishTimerModeState();
  publishTimerDurationState();
  publishTimerBrightnessState();
  publishTimerAlertBuzzerState();
  publishTimerAutoRepeatState();
  publishTimerAlertChirpState();
  publishTimerAlertBeepCountState();
  publishTimerStateState();
  publishTimerDisplayState();
  publishTimerRemainingState();
  publishTimerElapsedState();
}

// State Publishers
void publishTimerEnableState() {
  if (strcmp(mqttOnOff, "on") != 0) return;
  char topic[256];
  buildStateTopic(topic, sizeof(topic), "timer_enable");
  mqttClient.publish(topic, timerEnabled ? "ON" : "OFF", true);
}

void publishTimerModeState() {
  if (strcmp(mqttOnOff, "on") != 0) return;
  char topic[256];
  buildStateTopic(topic, sizeof(topic), "timer_mode");
  mqttClient.publish(topic, timerConfig.mode, true);
}

void publishTimerDurationState() {
  if (strcmp(mqttOnOff, "on") != 0) return;
  char topic[256];
  buildStateTopic(topic, sizeof(topic), "timer_duration");
  mqttClient.publish(topic, timerConfig.durationSeconds, true);
}

void publishTimerBrightnessState() {
  if (strcmp(mqttOnOff, "on") != 0) return;
  char topic[256];
  buildStateTopic(topic, sizeof(topic), "timer_brightness");
  mqttClient.publish(topic, timerConfig.brightness, true);
}

void publishTimerAlertBuzzerState() {
  if (strcmp(mqttOnOff, "on") != 0) return;
  char topic[256];
  buildStateTopic(topic, sizeof(topic), "timer_alert_buzzer");
  mqttClient.publish(topic, (strcmp(timerConfig.alertBuzzer, "on") == 0) ? "ON" : "OFF", true);
}

void publishTimerAutoRepeatState() {
  if (strcmp(mqttOnOff, "on") != 0) return;
  char topic[256];
  buildStateTopic(topic, sizeof(topic), "timer_auto_repeat");
  mqttClient.publish(topic, (strcmp(timerConfig.autoRepeat, "on") == 0) ? "ON" : "OFF", true);
}

void publishTimerAlertChirpState() {
  if (strcmp(mqttOnOff, "on") != 0) return;
  char topic[256];
  buildStateTopic(topic, sizeof(topic), "timer_alert_chirp");
  mqttClient.publish(topic, timerConfig.alertChirp, true);
}

void publishTimerAlertBeepCountState() {
  if (strcmp(mqttOnOff, "on") != 0) return;
  char topic[256];
  buildStateTopic(topic, sizeof(topic), "timer_alert_beep_count");
  mqttClient.publish(topic, timerConfig.alertBuzzerCount, true);
}

void publishTimerStateState() {
  if (strcmp(mqttOnOff, "on") != 0) return;
  char topic[256];
  const char* state;
  if (!timerRunning) {
    state = timerAlertTriggered ? "completed" : "stopped";
  } else if (timerPaused) {
    state = "paused";
  } else {
    state = "running";
  }
  buildStateTopic(topic, sizeof(topic), "timer_state");
  mqttClient.publish(topic, state, true);
}

void publishTimerDisplayState() {
  if (strcmp(mqttOnOff, "on") != 0) return;
  char topic[256];
  char displayBuffer[32];
  getTimerDisplay(displayBuffer, sizeof(displayBuffer));
  buildStateTopic(topic, sizeof(topic), "timer_display");
  mqttClient.publish(topic, displayBuffer, true);
}

void publishTimerRemainingState() {
  if (strcmp(mqttOnOff, "on") != 0) return;
  if (strcmp(timerConfig.mode, "countdown") != 0) return;
  
  char topic[256];
  char remaining[16];
  unsigned long remainingSeconds = (timerElapsedSeconds >= timerTargetSeconds) ? 
    0 : (timerTargetSeconds - timerElapsedSeconds);
  snprintf(remaining, sizeof(remaining), "%lu", remainingSeconds);
  buildStateTopic(topic, sizeof(topic), "timer_remaining");
  mqttClient.publish(topic, remaining, true);
}

void publishTimerElapsedState() {
  if (strcmp(mqttOnOff, "on") != 0) return;
  char topic[256];
  char elapsed[16];
  snprintf(elapsed, sizeof(elapsed), "%lu", timerElapsedSeconds);
  buildStateTopic(topic, sizeof(topic), "timer_elapsed");
  mqttClient.publish(topic, elapsed, true);
}

// Command Handlers
void handleTimerEnableCommand(const char* payload) {
  bool newEnabled = (strcasecmp(payload, "ON") == 0);
  strlcpy(timerConfig.enabled, newEnabled ? "on" : "off", sizeof(timerConfig.enabled));
  timerEnabled = newEnabled;
  
  if (!timerEnabled) {
    stopTimer(); // Force exit from timer mode
  }
  
  saveTimerConfiguration(timerConfigFile, timerConfig);
  publishTimerEnableState();
  Serial.print(F("Timer enabled: "));
  Serial.println(timerEnabled ? "ON" : "OFF");
}

void handleTimerModeCommand(const char* payload) {
  strlcpy(timerConfig.mode, payload, sizeof(timerConfig.mode));
  saveTimerConfiguration(timerConfigFile, timerConfig);
  publishTimerModeState();
  Serial.print(F("Timer mode: "));
  Serial.println(timerConfig.mode);
}

void handleTimerDurationCommand(const char* payload) {
  strlcpy(timerConfig.durationSeconds, payload, sizeof(timerConfig.durationSeconds));
  timerTargetSeconds = atoi(payload);
  saveTimerConfiguration(timerConfigFile, timerConfig);
  publishTimerDurationState();
  Serial.print(F("Timer duration: "));
  Serial.println(timerConfig.durationSeconds);
}

void handleTimerStartCommand() {
  startTimer();
  publishTimerStateState();
  Serial.println(F("Timer start command received"));
}

void handleTimerPauseCommand() {
  pauseTimer();
  publishTimerStateState();
  Serial.println(F("Timer pause command received"));
}

void handleTimerStopCommand() {
  stopTimer();
  publishTimerStateState();
  publishTimerDisplayState();
  publishTimerRemainingState();
  publishTimerElapsedState();
  Serial.println(F("Timer stop command received"));
}

void handleTimerResetCommand() {
  resetTimer();
  publishTimerStateState();
  publishTimerDisplayState();
  publishTimerRemainingState();
  publishTimerElapsedState();
  Serial.println(F("Timer reset command received"));
}

void handleTimerBrightnessCommand(const char* payload) {
  strlcpy(timerConfig.brightness, payload, sizeof(timerConfig.brightness));
  timerBrightness = atoi(payload);
  saveTimerConfiguration(timerConfigFile, timerConfig);
  publishTimerBrightnessState();
  Serial.print(F("Timer brightness: "));
  Serial.println(timerConfig.brightness);
}

void handleTimerAlertBuzzerCommand(const char* payload) {
  bool enabled = (strcasecmp(payload, "ON") == 0);
  strlcpy(timerConfig.alertBuzzer, enabled ? "on" : "off", sizeof(timerConfig.alertBuzzer));
  saveTimerConfiguration(timerConfigFile, timerConfig);
  publishTimerAlertBuzzerState();
  Serial.print(F("Timer alert buzzer: "));
  Serial.println(timerConfig.alertBuzzer);
}

void handleTimerAutoRepeatCommand(const char* payload) {
  bool enabled = (strcasecmp(payload, "ON") == 0);
  strlcpy(timerConfig.autoRepeat, enabled ? "on" : "off", sizeof(timerConfig.autoRepeat));
  saveTimerConfiguration(timerConfigFile, timerConfig);
  publishTimerAutoRepeatState();
  Serial.print(F("Timer auto repeat: "));
  Serial.println(timerConfig.autoRepeat);
}

void handleTimerAlertChirpCommand(const char* payload) {
  // Validate chirp name exists in library
  if (getChirpByName(payload) != nullptr) {
    strlcpy(timerConfig.alertChirp, payload, sizeof(timerConfig.alertChirp));
    saveTimerConfiguration(timerConfigFile, timerConfig);
    publishTimerAlertChirpState();
    Serial.print(F("Timer alert chirp: "));
    Serial.println(timerConfig.alertChirp);
  } else {
    Serial.print(F("Invalid chirp name: "));
    Serial.println(payload);
  }
}

void handleTimerAlertBeepCountCommand(const char* payload) {
  int count = atoi(payload);
  if (count >= 1 && count <= 20) {
    strlcpy(timerConfig.alertBuzzerCount, payload, sizeof(timerConfig.alertBuzzerCount));
    saveTimerConfiguration(timerConfigFile, timerConfig);
    publishTimerAlertBeepCountState();
    Serial.print(F("Timer alert beep count: "));
    Serial.println(timerConfig.alertBuzzerCount);
  } else {
    Serial.print(F("Invalid beep count (must be 1-20): "));
    Serial.println(payload);
  }
}
#endif // DISABLE_TIMER_FEATURE
