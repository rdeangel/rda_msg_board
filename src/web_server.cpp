#include "web_server.h"
#include "functions.h"
#include "globals.h"
#include "mqtt.h"
#include "web_pages.h"
#include "config_manager.h"
#ifndef DISABLE_TIMER_FEATURE
#include "timer.h"
#endif
#ifndef DISABLE_WEATHER_FEATURE
#include "weather.h"
#endif
#include "chirp_library.h"
#include "buzzer_utils.h"
#ifdef ESP8266
  #include <Updater.h>
#elif defined(ESP32)
  #include <Update.h>
#endif

// Global buffer for receiving config JSON via upload handler
// ESP8266WebServer's arg("plain") doesn't work for POST bodies, so we use the upload mechanism
String uploadedConfigJson;

// ################################ START OF SPECIFIC HTTP SERVER FUNCTIONS
// ################################//
void showWebpageHttp() {
  // Send directly from PROGMEM to avoid RAM allocation and blocking
  serverHttp.send_P(200, "text/html", MAIN_page);
}

void showChangeCredentialsHttp() {
  // Send directly from PROGMEM to avoid RAM allocation and blocking
  serverHttp.send_P(200, "text/html", DEVICE_CONFIG_page);
}

void usernamePasswordHttp() {
  // Removed debug String building to prevent blocking during clock transitions
  for (uint8_t i = 0; i < serverHttp.args(); i++) {
    if (serverHttp.argName(i) == "Username") {
      serverHttp.arg(i).toCharArray(newWebUsername, STDSIZE);
      newWebUsernameAvailable = true;
    }
    if (serverHttp.argName(i) == "Password") {
      serverHttp.arg(i).toCharArray(newWebPassword, STDSIZE);
      newWebPasswordAvailable = true;
    }
    if (serverHttp.argName(i) == "Hostname") {
      serverHttp.arg(i).toCharArray(newWebHostname, STDSIZE);
      newWebHostnameAvailable = true;
    }
  }
  PRINTS("\nCredentials update received");
}

void showChangeMqttConfigHttp() {
  // Send directly from PROGMEM to avoid RAM allocation and blocking
  serverHttp.send_P(200, "text/html", CHANGEMQTTCONFIG_page);
}

void onMqttConfigChangeHttp() {
  // Removed debug String building to prevent blocking during clock transitions
  for (uint8_t i = 0; i < serverHttp.args(); i++) {
    if (serverHttp.argName(i) == "MQTTONOFF") {
      serverHttp.arg(i).toCharArray(newMqttOnOff, STDSIZE);
      if ((newMqttOnOff != NULL) && (newMqttOnOff[0] == '\0')) {
        newMqttOnOffAvailable = false;
      } else {
        newMqttOnOffAvailable = true;
      }
    }
    if (serverHttp.argName(i) == "MQTTANONYMOUS") {
      serverHttp.arg(i).toCharArray(newMqttAnonymous, STDSIZE);
      if ((newMqttAnonymous != NULL) && (newMqttAnonymous[0] == '\0')) {
        newMqttAnonymousAvailable = false;
      } else {
        newMqttAnonymousAvailable = true;
      }
    }
    if (serverHttp.argName(i) == "MQTTALERT") {
      serverHttp.arg(i).toCharArray(newMqttAlert, STDSIZE);
      if ((newMqttAlert != NULL) && (newMqttAlert[0] == '\0')) {
        newMqttAlertAvailable = false;
      } else {
        newMqttAlertAvailable = true;
      }
    }
    if (serverHttp.argName(i) == "MQTTUSERNAME") {
      serverHttp.arg(i).toCharArray(newMqttUsername, STDSIZE);
      if ((newMqttUsername != NULL) && (newMqttUsername[0] == '\0')) {
        newMqttUsernameAvailable = false;
      } else {
        newMqttUsernameAvailable = true;
      }
    }
    if (serverHttp.argName(i) == "MQTTPASSWORD") {
      serverHttp.arg(i).toCharArray(newMqttPassword, STDSIZE);
      if ((newMqttPassword != NULL) && (newMqttPassword[0] == '\0')) {
        newMqttPasswordAvailable = false;
      } else {
        newMqttPasswordAvailable = true;
      }
    }
    if (serverHttp.argName(i) == "MQTTSERVERADDRESS") {
      serverHttp.arg(i).toCharArray(newMqttServerAddress, STDSIZE);
      if ((newMqttServerAddress != NULL) && (newMqttServerAddress[0] == '\0')) {
        newMqttServerAddressAvailable = false;
      } else {
        newMqttServerAddressAvailable = true;
      }
    }
    if (serverHttp.argName(i) == "MQTTSERVERPORT") {
      serverHttp.arg(i).toCharArray(newMqttServerPort, STDSIZE);
      if ((newMqttServerPort != NULL) && (newMqttServerPort[0] == '\0')) {
        newMqttServerPortAvailable = false;
      } else {
        newMqttServerPortAvailable = true;
      }
    }
    if (serverHttp.argName(i) == "MQTTTOPICPREFIX") {
      serverHttp.arg(i).toCharArray(newMqttTopicPrefix, STDSIZE);
      if ((newMqttTopicPrefix != NULL) && (newMqttTopicPrefix[0] == '\0')) {
        newMqttTopicPrefixAvailable = false;
      } else {
        newMqttTopicPrefixAvailable = true;
      }
    }
    if (serverHttp.argName(i) == "MQTTHADISCOVERY") {
      serverHttp.arg(i).toCharArray(newMqttHaDiscovery, STDSIZE);
      if ((newMqttHaDiscovery != NULL) && (newMqttHaDiscovery[0] == '\0')) {
        newMqttHaDiscoveryAvailable = false;
      } else {
        newMqttHaDiscoveryAvailable = true;
      }
    }
    if (serverHttp.argName(i) == "MQTTMESSAGESENABLE") {
      serverHttp.arg(i).toCharArray(newMqttMessagesEnable, STDSIZE);
      if ((newMqttMessagesEnable != NULL) && (newMqttMessagesEnable[0] == '\0')) {
        newMqttMessagesEnableAvailable = false;
      } else {
        newMqttMessagesEnableAvailable = true;
      }
    }
#ifdef ESP32
    if (serverHttp.argName(i) == "MQTTTLSENABLED") {
      serverHttp.arg(i).toCharArray(newTlsEnabledMqtt, STDSIZE);
      if ((newTlsEnabledMqtt != NULL) && (newTlsEnabledMqtt[0] == '\0')) {
        newTlsEnabledMqttAvailable = false;
      } else {
        newTlsEnabledMqttAvailable = true;
      }
    }
#endif
  }
  PRINTS("\nMQTT config update received");
}

void onNotFoundUriHttp() {
  serverHttp.send(
      404, "text/plain",
      "404: Not found"); // Send HTTP status 404 (Not Found) when there's no
                         // handler for the URI in the request
}

void handleSaveGeneral() {
  for (uint8_t i = 0; i < serverHttp.args(); i++) {
    if (serverHttp.argName(i) == "BuzzerEnable") {
      serverHttp.arg(i).toCharArray(generalConfig.buzzerEnable, STDSIZE);
      // Sync global var
      strlcpy(newBuzzerEnable, generalConfig.buzzerEnable, STDSIZE);
      newBuzzerEnableAvailable = true;
    }
    if (serverHttp.argName(i) == "BrightnessOverrideEnable") {
      serverHttp.arg(i).toCharArray(generalConfig.brightnessOverrideEnable, STDSIZE);
    }
    if (serverHttp.argName(i) == "BrightnessOverrideValue") {
      serverHttp.arg(i).toCharArray(generalConfig.brightnessOverrideValue, BRI_SIZE);
    }
  }
  
  // Update global variables
  brightnessOverrideEnabled = (strcmp(generalConfig.brightnessOverrideEnable, "on") == 0);
  brightnessOverrideValue = atoi(generalConfig.brightnessOverrideValue);
  
  saveGeneralConfiguration(generalConfigFile, generalConfig);
  
  PRINTS("\nGeneral settings saved - Buzzer Enable: ");
  Serial.print(generalConfig.buzzerEnable);
  PRINTS(", Brightness Override: ");
  Serial.print(generalConfig.brightnessOverrideEnable);
  PRINTS(", Override Value: ");
  Serial.println(generalConfig.brightnessOverrideValue);
  
  serverHttp.send(200, "text/plain", "OK");
}

void httpWebDirDef() {
  serverHttp.on("/", []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }
    showWebpageHttp();
  });
  serverHttp.on("/mainpagevars", []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }
    setMainPageVars();
    serverHttp.send(200, "text/plane", mainPageVars);
  });
  serverHttp.on("/changecredvars", []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }
    setChangeCredVars();
    serverHttp.send(200, "text/plane", changeCredVars);
  });
  serverHttp.on("/generalvars", HTTP_GET, setGeneralVars);
  serverHttp.on("/savegeneral", HTTP_POST, handleSaveGeneral);

  serverHttp.on("/updatevars", []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }
    setUpdateVars();
    serverHttp.send(200, "text/plane", updateVars);
  });
  serverHttp.on("/setdefault", []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }
    String type = serverHttp.arg("type");
    String value = serverHttp.arg("value");
    
    if (type == "REP") {
      value.toCharArray(defaultsConfig.repeatDefaultHolder, sizeof(defaultsConfig.repeatDefaultHolder));
      strlcpy(repeatDefault, defaultsConfig.repeatDefaultHolder, sizeof(repeatDefault));
    } else if (type == "BUZ") {
      value.toCharArray(defaultsConfig.buzzerDefaultHolder, sizeof(defaultsConfig.buzzerDefaultHolder));
      strlcpy(buzzerDefault, defaultsConfig.buzzerDefaultHolder, sizeof(buzzerDefault));
    } else if (type == "DEL") {
      value.toCharArray(defaultsConfig.scrollDelayDefaultHolder, sizeof(defaultsConfig.scrollDelayDefaultHolder));
      strlcpy(scrollDelayDefault, defaultsConfig.scrollDelayDefaultHolder, sizeof(scrollDelayDefault));
    } else if (type == "BRI") {
      value.toCharArray(defaultsConfig.brightnessDefaultHolder, sizeof(defaultsConfig.brightnessDefaultHolder));
      strlcpy(ledBrightnessDefault, defaultsConfig.brightnessDefaultHolder, sizeof(ledBrightnessDefault));
    } else if (type == "ALERTCHIRP") {
      value.toCharArray(defaultsConfig.alertChirpDefaultHolder, sizeof(defaultsConfig.alertChirpDefaultHolder));
      strlcpy(alertChirpDefault, defaultsConfig.alertChirpDefaultHolder, sizeof(alertChirpDefault));
    }

    saveDefaultsConfiguration(defaultsConfigFile, defaultsConfig);
    PRINT("Default saved - Type: ", type);
    PRINT(", Value: ", value);
    PRINTS("\n");
    serverHttp.send(200, "text/plain", "OK");
  });
  serverHttp.on("/resetdefaults", []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }
    // Delete the custom defaults file to revert to hardcoded defaults
    LittleFS.remove(defaultsConfigFile);
    
    // Reset to hardcoded defaults (these are the compile-time defaults)
    strlcpy(repeatDefault, "10", sizeof(repeatDefault));
    strlcpy(buzzerDefault, "10", sizeof(buzzerDefault));
    strlcpy(scrollDelayDefault, "35", sizeof(scrollDelayDefault));
    strlcpy(ledBrightnessDefault, "7", sizeof(ledBrightnessDefault));
    strlcpy(alertChirpDefault, "Fast Beep", sizeof(alertChirpDefault));

    // Also update the config struct
    strlcpy(defaultsConfig.repeatDefaultHolder, repeatDefault, sizeof(defaultsConfig.repeatDefaultHolder));
    strlcpy(defaultsConfig.buzzerDefaultHolder, buzzerDefault, sizeof(defaultsConfig.buzzerDefaultHolder));
    strlcpy(defaultsConfig.scrollDelayDefaultHolder, scrollDelayDefault, sizeof(defaultsConfig.scrollDelayDefaultHolder));
    strlcpy(defaultsConfig.brightnessDefaultHolder, ledBrightnessDefault, sizeof(defaultsConfig.brightnessDefaultHolder));
    strlcpy(defaultsConfig.alertChirpDefaultHolder, alertChirpDefault, sizeof(defaultsConfig.alertChirpDefaultHolder));

    PRINTS("Defaults reset to factory values\n");

    // Return the hardcoded defaults as JSON
    String response = "{\"REP\":\"" + String(repeatDefault) +
                      "\",\"BUZ\":\"" + String(buzzerDefault) +
                      "\",\"DEL\":\"" + String(scrollDelayDefault) +
                      "\",\"BRI\":\"" + String(ledBrightnessDefault) +
                      "\",\"ALERTCHIRP\":\"" + String(alertChirpDefault) + "\"}";
    serverHttp.send(200, "application/json", response);
  });
  serverHttp.on("/arg", []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }
    #ifndef DISABLE_SLEEP_MODE_FEATURE
    // Respect sleep mode - don't display messages during sleep hours (unless Mute Only enabled)
    if (isSleepModeActive() && !sleepModeMuteOnly) {
      PRINTS("Message blocked: Sleep mode is active\n");
      serverHttp.send(403, "text/plain", "Sleep mode active");
      return;
    }
    #endif
    if (forceRepetitions && newMessageAvailable && strlen(curMessage) > 0 && repeatCount < atoi(newRepeat)) {
      PRINTS("Message blocked: Forced repetition in progress\n");
      serverHttp.send(409, "text/plain", "Forced repetition in progress");
      return;
    }
    onMessageCallHttp();
  });
  serverHttp.on("/api", HTTP_POST, []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }
    #ifndef DISABLE_SLEEP_MODE_FEATURE
    // Respect sleep mode - don't display messages during sleep hours (unless Mute Only enabled)
    if (isSleepModeActive() && !sleepModeMuteOnly) {
      PRINTS("Message blocked: Sleep mode is active\n");
      serverHttp.send(403, "application/json", "{\"error\":\"Sleep mode active\"}");
      return;
    }
    #endif
    if (forceRepetitions && newMessageAvailable && strlen(curMessage) > 0 && repeatCount < atoi(newRepeat)) {
      PRINTS("Message blocked: Forced repetition in progress\n");
      serverHttp.send(409, "application/json", "{\"error\":\"Forced repetition in progress\"}");
      return;
    }
    PRINTS("\nHTTP JSON Message Arrived!\nHTTP Message: ");
    onMessageCallJson(serverHttp.arg("plain").c_str());
    serverHttp.send(204, "");
  });
  serverHttp.on("/api/clock", HTTP_POST, []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }

    // Parse JSON
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, serverHttp.arg("plain"));
    if (error) {
      serverHttp.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      return;
    }

    // Track changes for animation logic
    bool wasEnabled = clockEnabled;
    char previousEffect[STDSIZE];
    strlcpy(previousEffect, clockConfig.transitionEffect, sizeof(previousEffect));
    bool configChanged = false;

    // Process each parameter (if present in JSON)
    if (!doc["enabled"].isNull()) {
      bool enabled = parseBoolean(doc["enabled"]);
      snprintf(clockConfig.enabled, sizeof(clockConfig.enabled), "%s", enabled ? "on" : "off");
      clockEnabled = enabled;
      configChanged = true;
    }

    if (!doc["brightness"].isNull()) {
      int brightness = doc["brightness"];
      if (brightness < 0 || brightness > 15) {
        serverHttp.send(400, "application/json", "{\"error\":\"Brightness must be 0-15\"}");
        return;
      }
      snprintf(clockConfig.brightness, sizeof(clockConfig.brightness), "%d", brightness);
      clockBrightness = brightness;
      configChanged = true;
    }

    if (!doc["ntpServer"].isNull()) {
      String server = doc["ntpServer"].as<String>();
      if (server.length() == 0 || server.length() >= STDSIZE) {
        serverHttp.send(400, "application/json", "{\"error\":\"Invalid NTP server\"}");
        return;
      }
      strlcpy(clockConfig.ntpServer, server.c_str(), sizeof(clockConfig.ntpServer));
      configChanged = true;
    }

    if (!doc["tzString"].isNull()) {
      const char* tz = doc["tzString"];
      strlcpy(clockConfig.tzString, tz, sizeof(clockConfig.tzString));
      configChanged = true;
    }

    if (!doc["dateFormat"].isNull()) {
      const char* format = doc["dateFormat"];
      strlcpy(clockConfig.dateFormat, format, sizeof(clockConfig.dateFormat));
      configChanged = true;
    }

    if (!doc["transitionDelayMs"].isNull()) {
      int delay = doc["transitionDelayMs"];
      if (delay < 0 || delay > 10000) {
        serverHttp.send(400, "application/json", "{\"error\":\"Transition delay must be 0-10000ms\"}");
        return;
      }
      snprintf(clockConfig.transitionDelayMs, sizeof(clockConfig.transitionDelayMs), "%d", delay);
      configChanged = true;
    }

    if (!doc["transitionEffect"].isNull()) {
      String effect = doc["transitionEffect"].as<String>();
      if (!isValidEffect(effect.c_str())) {
        serverHttp.send(400, "application/json", "{\"error\":\"Invalid transition effect\"}");
        return;
      }
      strlcpy(clockConfig.transitionEffect, effect.c_str(), sizeof(clockConfig.transitionEffect));
      configChanged = true;
    }

    if (!doc["randomizeTransition"].isNull()) {
      bool randomize = parseBoolean(doc["randomizeTransition"]);
      snprintf(clockConfig.randomizeTransition, sizeof(clockConfig.randomizeTransition), "%s", randomize ? "on" : "off");
      configChanged = true;
    }

    if (!doc["resyncIntervalHours"].isNull()) {
      int hours = doc["resyncIntervalHours"];
      if (hours < 1 || hours > 24) {
        serverHttp.send(400, "application/json", "{\"error\":\"Resync interval must be 1-24 hours\"}");
        return;
      }
      snprintf(clockConfig.resyncIntervalHours, sizeof(clockConfig.resyncIntervalHours), "%d", hours);
      configChanged = true;
    }

    // Save and apply changes
    if (configChanged) {
      saveClockConfiguration(clockConfigFile, clockConfig);

      if (clockEnabled && WiFi.status() == WL_CONNECTED) {
        initNtpTime();
      }

      // Handle display mode transitions (same logic as /saveclocksettings)
      bool enabledChanged = (wasEnabled != clockEnabled);
      bool effectChanged = (strcmp(previousEffect, clockConfig.transitionEffect) != 0);

      if (enabledChanged && clockEnabled && currentDisplayMode == MODE_MESSAGE) {
        currentDisplayMode = MODE_TRANSITION;
        transitionStartTime = millis();
        P.displayClear();
      }

      if (!enabledChanged && effectChanged && clockEnabled && currentDisplayMode == MODE_CLOCK) {
        currentDisplayMode = MODE_TRANSITION;
        transitionStartTime = millis();
        P.displayClear();
      }

      if (enabledChanged && !clockEnabled && (currentDisplayMode == MODE_CLOCK || currentDisplayMode == MODE_TRANSITION)) {
        currentDisplayMode = MODE_CLOCK_EXIT;
        // Exit animation logic (same as web_server.cpp:950-977)
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

      // Publish MQTT state updates if HA discovery enabled
      publishAllClockStates();
    }

    serverHttp.send(200, "application/json", "{\"status\":\"ok\"}");
  });

  #ifndef DISABLE_TIMER_FEATURE
  // Timer API endpoints
  serverHttp.on("/api/timer/status", HTTP_GET, []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }
    String json = getTimerStatusJson();
    serverHttp.send(200, "application/json", json);
  });

  serverHttp.on("/api/timer", HTTP_GET, []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }
    String json = getTimerStatusJson();
    serverHttp.send(200, "application/json", json);
  });

  serverHttp.on("/api/timer", HTTP_POST, []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }

    // Parse JSON
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, serverHttp.arg("plain"));
    if (error) {
      serverHttp.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      return;
    }

    bool configChanged = false;

    // Update timer configuration
    if (!doc["enabled"].isNull()) {
      bool enabled = parseBoolean(doc["enabled"]);
      snprintf(timerConfig.enabled, sizeof(timerConfig.enabled), "%s", enabled ? "on" : "off");
      timerEnabled = enabled;
      configChanged = true;
      
      if (!timerEnabled) {
        stopTimer(); // Force exit from timer mode
      }
    }

    if (!doc["mode"].isNull()) {
      String mode = doc["mode"].as<String>();
      if (mode != "countdown" && mode != "stopwatch") {
        serverHttp.send(400, "application/json", "{\"error\":\"Mode must be 'countdown' or 'stopwatch'\"}");
        return;
      }
      strlcpy(timerConfig.mode, mode.c_str(), sizeof(timerConfig.mode));
      configChanged = true;
    }

    if (!doc["durationSeconds"].isNull()) {
      int duration = doc["durationSeconds"];
      if (duration < 0) {
        serverHttp.send(400, "application/json", "{\"error\":\"Duration must be >= 0\"}");
        return;
      }
      snprintf(timerConfig.durationSeconds, sizeof(timerConfig.durationSeconds), "%d", duration);
      timerTargetSeconds = duration;
      configChanged = true;
    }

    if (!doc["brightness"].isNull()) {
      int brightness = doc["brightness"];
      if (brightness < 0 || brightness > 15) {
        serverHttp.send(400, "application/json", "{\"error\":\"Brightness must be 0-15\"}");
        return;
      }
      snprintf(timerConfig.brightness, sizeof(timerConfig.brightness), "%d", brightness);
      timerBrightness = brightness;
      configChanged = true;
    }

    if (!doc["alertBuzzer"].isNull()) {
      bool alertBuzzer = parseBoolean(doc["alertBuzzer"]);
      snprintf(timerConfig.alertBuzzer, sizeof(timerConfig.alertBuzzer), "%s", alertBuzzer ? "on" : "off");
      configChanged = true;
    }

    if (!doc["alertBuzzerCount"].isNull()) {
      int count = doc["alertBuzzerCount"];
      if (count < 0) {
        serverHttp.send(400, "application/json", "{\"error\":\"Alert buzzer count must be >= 0\"}");
        return;
      }
      snprintf(timerConfig.alertBuzzerCount, sizeof(timerConfig.alertBuzzerCount), "%d", count);
      configChanged = true;
    }

    if (!doc["autoRepeat"].isNull()) {
      bool autoRepeat = parseBoolean(doc["autoRepeat"]);
      snprintf(timerConfig.autoRepeat, sizeof(timerConfig.autoRepeat), "%s", autoRepeat ? "on" : "off");
      configChanged = true;
    }

    if (configChanged) {
      saveTimerConfiguration(timerConfigFile, timerConfig);
    }

    String json = getTimerStatusJson();
    serverHttp.send(200, "application/json", json);
  });

  serverHttp.on("/api/timer/start", HTTP_POST, []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }
    startTimer();
    String json = getTimerStatusJson();
    serverHttp.send(200, "application/json", json);
  });

  serverHttp.on("/api/timer/pause", HTTP_POST, []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }
    pauseTimer();
    String json = getTimerStatusJson();
    serverHttp.send(200, "application/json", json);
  });

  serverHttp.on("/api/timer/stop", HTTP_POST, []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }
    stopTimer();
    String json = getTimerStatusJson();
    serverHttp.send(200, "application/json", json);
  });

  serverHttp.on("/api/timer/reset", HTTP_POST, []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }
    resetTimer();
    String json = getTimerStatusJson();
    serverHttp.send(200, "application/json", json);
  });

  serverHttp.on("/api/timer/set", HTTP_POST, []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, serverHttp.arg("plain"));
    if (error) {
      serverHttp.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      return;
    }

    int hours = doc["hours"] | 0;
    int minutes = doc["minutes"] | 0;
    int seconds = doc["seconds"] | 0;

    if (hours < 0 || minutes < 0 || minutes >= 60 || seconds < 0 || seconds >= 60) {
      serverHttp.send(400, "application/json", "{\"error\":\"Invalid time values\"}");
      return;
    }

    setTimerDuration(hours, minutes, seconds);
    String json = getTimerStatusJson();
    serverHttp.send(200, "application/json", json);
  });
  #endif

  serverHttp.on("/exportconfig", []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }
    
    PRINTS("Starting config export (manual JSON builder)\n");
    
    // Build JSON manually to avoid JsonDocument memory overhead
    // We'll use String but build it incrementally to monitor memory
    String json;
    json.reserve(2048); // Pre-allocate reasonable size
    
    // Helper to add escaped string
    auto addString = [&json](const char* str) {
      json += "\"";
      for (const char* p = str; *p; p++) {
        if (*p == '"' || *p == '\\') json += '\\';
        json += *p;
      }
      json += "\"";
    };
    
    // Start JSON
    json += "{\n";
    
    // Device config
    json += "  \"hostname\": "; addString(clientId.c_str()); json += ",\n";
    json += "  \"username\": "; addString(web_username); json += ",\n";
    json += "  \"password\": "; addString(web_password); json += ",\n";
    
    // MQTT config
    json += "  \"mqtt\": {\n";
    json += "    \"enabled\": "; addString(mqttOnOff); json += ",\n";
    json += "    \"anonymous\": "; addString(mqttAnonymous); json += ",\n";
    json += "    \"alert\": "; addString(mqttAlert); json += ",\n";
    json += "    \"username\": "; addString(mqttUsername); json += ",\n";
    json += "    \"password\": "; addString(mqttPassword); json += ",\n";
    json += "    \"server\": "; addString(mqttServerAddress); json += ",\n";
    json += "    \"port\": "; addString(mqttServerPort); json += ",\n";
    json += "    \"topicPrefix\": "; addString(mqttTopicPrefix); json += ",\n";
    json += "    \"haDiscovery\": "; addString(mqttHaDiscovery); json += ",\n";
#ifdef ESP32
    json += "    \"tlsEnabled\": "; addString(tlsEnabledMqtt); json += ",\n";
#endif
    json += "    \"messagesEnable\": "; addString(mqttMessagesEnable); json += "\n";
    json += "  },\n";
    
    // Defaults config
    json += "  \"defaults\": {\n";
    json += "    \"repeat\": "; addString(repeatDefault); json += ",\n";
    json += "    \"buzzer\": "; addString(buzzerDefault); json += ",\n";
    json += "    \"scrollDelay\": "; addString(scrollDelayDefault); json += ",\n";
    json += "    \"brightness\": "; addString(ledBrightnessDefault); json += "\n";
    json += "  },\n";
    
    // General config
    json += "  \"general\": {\n";
    json += "    \"buzzerEnable\": "; addString(generalConfig.buzzerEnable); json += ",\n";
    json += "    \"brightnessOverrideEnable\": "; addString(generalConfig.brightnessOverrideEnable); json += ",\n";
    json += "    \"brightnessOverrideValue\": "; addString(generalConfig.brightnessOverrideValue); json += "\n";
    json += "  },\n";
    
    // Clock config
    json += "  \"clock\": {\n";
    json += "    \"enabled\": "; addString(clockConfig.enabled); json += ",\n";
    json += "    \"ntpServer\": "; addString(clockConfig.ntpServer); json += ",\n";
    json += "    \"tzString\": "; addString(clockConfig.tzString); json += ",\n";
    json += "    \"dateFormat\": "; addString(clockConfig.dateFormat); json += ",\n";
    json += "    \"brightness\": "; addString(clockConfig.brightness); json += ",\n";
    json += "    \"transitionDelayMs\": "; addString(clockConfig.transitionDelayMs); json += ",\n";
    json += "    \"transitionSpeed\": "; addString(clockConfig.transitionSpeed); json += ",\n";
    json += "    \"transitionEffect\": "; addString(clockConfig.transitionEffect); json += ",\n";
    json += "    \"randomizeTransition\": "; addString(clockConfig.randomizeTransition); json += ",\n";
    json += "    \"resyncIntervalHours\": "; addString(clockConfig.resyncIntervalHours); json += ",\n";
    json += "    \"clockFace\": "; addString(clockConfig.clockFace); json += ",\n";
    json += "    \"dateAlternate\": "; addString(clockConfig.dateAlternate); json += ",\n";
    json += "    \"dateAlternateSeconds\": "; addString(clockConfig.dateAlternateSeconds); json += ",\n";
    json += "    \"customDateFormat\": "; addString(clockConfig.customDateFormat); json += ",\n";
    json += "    \"clockAmPm\": "; addString(clockConfig.clockAmPm); json += "\n";
    json += "  },\n";

#ifndef DISABLE_TIMER_FEATURE
    // Timer config
    json += "  \"timer\": {\n";
    json += "    \"enabled\": "; addString(timerConfig.enabled); json += ",\n";
    json += "    \"mode\": "; addString(timerConfig.mode); json += ",\n";
    json += "    \"durationSeconds\": "; addString(timerConfig.durationSeconds); json += ",\n";
    json += "    \"brightness\": "; addString(timerConfig.brightness); json += ",\n";
    json += "    \"alertBuzzer\": "; addString(timerConfig.alertBuzzer); json += ",\n";
    json += "    \"alertBuzzerCount\": "; addString(timerConfig.alertBuzzerCount); json += ",\n";
    json += "    \"alertChirp\": "; addString(timerConfig.alertChirp); json += ",\n";
    json += "    \"autoRepeat\": "; addString(timerConfig.autoRepeat); json += "\n";
    json += "  },\n";
#endif

#ifndef DISABLE_SLEEP_MODE_FEATURE
    // Sleep Mode config
    json += "  \"sleepMode\": {\n";
    json += "    \"enabled\": "; addString(sleepModeConfig.enabled); json += ",\n";
    json += "    \"onTime\": "; addString(sleepModeConfig.onTime); json += ",\n";
    json += "    \"offTime\": "; addString(sleepModeConfig.offTime); json += ",\n";
    json += "    \"muteOnly\": "; addString(sleepModeConfig.muteOnly); json += ",\n";
    json += "    \"weekendEnabled\": "; addString(sleepModeConfig.weekendEnabled); json += ",\n";
    json += "    \"weekendOnTime\": "; addString(sleepModeConfig.weekendOnTime); json += ",\n";
    json += "    \"weekendOffTime\": "; addString(sleepModeConfig.weekendOffTime); json += "\n";
    json += "  },\n";
#endif

#ifndef DISABLE_ALARM_FEATURE
    // Alarm config
    json += "  \"alarm\": {\n";
    json += "    \"masterEnabled\": "; addString(alarmConfig.masterEnabled); json += ",\n";
    json += "    \"alarms\": [\n";
    for (int i = 0; i < MAX_ALARMS; i++) {
      json += "      {\n";
      json += "        \"enabled\": "; addString(alarmConfig.alarms[i].enabled); json += ",\n";
      json += "        \"time\": "; addString(alarmConfig.alarms[i].time); json += ",\n";
      json += "        \"daysOfWeek\": "; addString(alarmConfig.alarms[i].daysOfWeek); json += ",\n";
      json += "        \"message\": "; addString(alarmConfig.alarms[i].message); json += ",\n";
      json += "        \"chirpName\": "; addString(alarmConfig.alarms[i].chirpName); json += ",\n";
      json += "        \"repeatCount\": "; addString(alarmConfig.alarms[i].repeatCount); json += "\n";
      json += "      }";
      if (i < MAX_ALARMS - 1) json += ",";
      json += "\n";
      yield(); // Prevent WDT
    }
    json += "    ]\n";
    json += "  },\n";
#endif

#ifndef DISABLE_WEATHER_FEATURE
    // Weather config
    json += "  \"weather\": {\n";
    json += "    \"enabled\": "; addString(weatherConfig.enabled); json += ",\n";
    json += "    \"location\": "; addString(weatherConfig.location); json += ",\n";
    json += "    \"latitude\": "; addString(weatherConfig.latitude); json += ",\n";
    json += "    \"longitude\": "; addString(weatherConfig.longitude); json += ",\n";
    json += "    \"updateIntervalMinutes\": "; addString(weatherConfig.updateIntervalMinutes); json += ",\n";
    json += "    \"temperatureUnit\": "; addString(weatherConfig.temperatureUnit); json += ",\n";
    json += "    \"brightness\": "; addString(weatherConfig.brightness); json += ",\n";
    json += "    \"displayDurationSeconds\": "; addString(weatherConfig.displayDurationSeconds); json += "\n";
    json += "  },\n";
#endif

    // Recurrent Alarm config
    json += "  \"recurrentAlarm\": {\n";
    json += "    \"enabled\": "; addString(recurrentAlarmConfig.enabled); json += ",\n";
    json += "    \"chirpName\": "; addString(recurrentAlarmConfig.chirpName); json += ",\n";
    json += "    \"interval\": "; addString(recurrentAlarmConfig.interval); json += ",\n";
    json += "    \"disableWeekends\": "; addString(recurrentAlarmConfig.disableWeekends); json += "\n";
    json += "  },\n";

    // WiFi config (Security Warning: Password stored in plain text)
    json += "  \"wifi\": {\n";
    json += "    \"ssid\": "; addString(WiFi.SSID().c_str()); json += ",\n";
    json += "    \"password\": "; addString(WiFi.psk().c_str()); json += "\n";
    json += "  }\n";
    json += "}";
    
    PRINTS("Config export complete, size: ");
    Serial.println(json.length());
    
    serverHttp.send(200, "application/json", json);
  });
  serverHttp.on("/importconfig", HTTP_POST,
    []() {
      // This handler runs AFTER the upload completes
      if (!serverHttp.authenticate(web_username, web_password)) {
        return serverHttp.requestAuthentication();
      }
      
      if (uploadedConfigJson.length() == 0) {
        PRINTS("Import failed: No config data received\n");
        serverHttp.send(400, "text/plain", "No config data received");
        return;
      }
      
      PRINTS("Processing config import, size: ");
      Serial.println(uploadedConfigJson.length());
      
      // Debug: print first 100 chars
      PRINTS("First 100 chars: ");
      Serial.println(uploadedConfigJson.substring(0, 100));
      
      
      // Use JsonDocument (ArduinoJson v7+) for heap-allocated JSON parsing
      // Replaces deprecated DynamicJsonDocument - memory is managed automatically
      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, uploadedConfigJson);
      
      // Clear buffer immediately after parsing to free memory
      uploadedConfigJson = "";
      
      if (error) {
      PRINTS("Import failed: ");
      Serial.println(error.c_str());
      String errorMsg = "Invalid JSON: ";
      errorMsg += error.c_str();
      serverHttp.send(400, "text/plain", errorMsg);
      return;
    }
    
    // Apply device config
    if (!doc["hostname"].isNull()) {
      String hostname = doc["hostname"].as<String>();
      hostname.toCharArray(webConfig.hostnameWebHolder, sizeof(webConfig.hostnameWebHolder));
      clientId = hostname;
    }
    if (!doc["username"].isNull()) {
      String username = doc["username"].as<String>();
      username.toCharArray(webConfig.usernameWebHolder, sizeof(webConfig.usernameWebHolder));
      strlcpy(web_username, webConfig.usernameWebHolder, sizeof(web_username));
    }
    if (!doc["password"].isNull()) {
      String password = doc["password"].as<String>();
      password.toCharArray(webConfig.passwordWebHolder, sizeof(webConfig.passwordWebHolder));
      strlcpy(web_password, webConfig.passwordWebHolder, sizeof(web_password));
    }
    saveConfiguration(webConfigFile, webConfig);
    
    // Apply MQTT config
    if (!doc["mqtt"].isNull()) {
      JsonObject mqtt = doc["mqtt"];
      if (!mqtt["enabled"].isNull()) strlcpy(mqttConfig.onOffMqttHolder, mqtt["enabled"], sizeof(mqttConfig.onOffMqttHolder));
      if (!mqtt["anonymous"].isNull()) strlcpy(mqttConfig.anonymousMqttHolder, mqtt["anonymous"], sizeof(mqttConfig.anonymousMqttHolder));
      if (!mqtt["alert"].isNull()) strlcpy(mqttConfig.alertMqttHolder, mqtt["alert"], sizeof(mqttConfig.alertMqttHolder));
      if (!mqtt["username"].isNull()) strlcpy(mqttConfig.usernameMqttHolder, mqtt["username"], sizeof(mqttConfig.usernameMqttHolder));
      if (!mqtt["password"].isNull()) strlcpy(mqttConfig.passwordMqttHolder, mqtt["password"], sizeof(mqttConfig.passwordMqttHolder));
      if (!mqtt["server"].isNull()) strlcpy(mqttConfig.serverAddressMqttHolder, mqtt["server"], sizeof(mqttConfig.serverAddressMqttHolder));
      if (!mqtt["port"].isNull()) strlcpy(mqttConfig.serverPortMqttHolder, mqtt["port"], sizeof(mqttConfig.serverPortMqttHolder));
      if (!mqtt["topicPrefix"].isNull()) strlcpy(mqttConfig.topicPrefixMqttHolder, mqtt["topicPrefix"], sizeof(mqttConfig.topicPrefixMqttHolder));
      if (!mqtt["haDiscovery"].isNull()) strlcpy(mqttConfig.haDiscoveryMqttHolder, mqtt["haDiscovery"], sizeof(mqttConfig.haDiscoveryMqttHolder));
#ifdef ESP32
      if (!mqtt["tlsEnabled"].isNull()) strlcpy(mqttConfig.tlsEnabledMqttHolder, mqtt["tlsEnabled"], sizeof(mqttConfig.tlsEnabledMqttHolder));
#endif
      if (!mqtt["messagesEnable"].isNull()) strlcpy(mqttConfig.mqttMessagesEnableHolder, mqtt["messagesEnable"], sizeof(mqttConfig.mqttMessagesEnableHolder));
      
      saveMqttConfiguration(mqttConfigFile, mqttConfig);
    }
    
    // Apply defaults config
    if (!doc["defaults"].isNull()) {
      JsonObject defaults = doc["defaults"];
      if (!defaults["repeat"].isNull()) {
        strlcpy(defaultsConfig.repeatDefaultHolder, defaults["repeat"], sizeof(defaultsConfig.repeatDefaultHolder));
        strlcpy(repeatDefault, defaultsConfig.repeatDefaultHolder, sizeof(repeatDefault));
      }
      if (!defaults["buzzer"].isNull()) {
        strlcpy(defaultsConfig.buzzerDefaultHolder, defaults["buzzer"], sizeof(defaultsConfig.buzzerDefaultHolder));
        strlcpy(buzzerDefault, defaultsConfig.buzzerDefaultHolder, sizeof(buzzerDefault));
      }
      if (!defaults["scrollDelay"].isNull()) {
        strlcpy(defaultsConfig.scrollDelayDefaultHolder, defaults["scrollDelay"], sizeof(defaultsConfig.scrollDelayDefaultHolder));
        strlcpy(scrollDelayDefault, defaultsConfig.scrollDelayDefaultHolder, sizeof(scrollDelayDefault));
      }
      if (!defaults["brightness"].isNull()) {
        strlcpy(defaultsConfig.brightnessDefaultHolder, defaults["brightness"], sizeof(defaultsConfig.brightnessDefaultHolder));
        strlcpy(ledBrightnessDefault, defaultsConfig.brightnessDefaultHolder, sizeof(ledBrightnessDefault));
      }
      saveDefaultsConfiguration(defaultsConfigFile, defaultsConfig);
    }

    // Apply general config
    if (!doc["general"].isNull()) {
      JsonObject general = doc["general"];
      if (!general["buzzerEnable"].isNull()) {
        strlcpy(generalConfig.buzzerEnable, general["buzzerEnable"], sizeof(generalConfig.buzzerEnable));
      }
      if (!general["brightnessOverrideEnable"].isNull()) {
        strlcpy(generalConfig.brightnessOverrideEnable, general["brightnessOverrideEnable"], sizeof(generalConfig.brightnessOverrideEnable));
      }
      if (!general["brightnessOverrideValue"].isNull()) {
        strlcpy(generalConfig.brightnessOverrideValue, general["brightnessOverrideValue"], sizeof(generalConfig.brightnessOverrideValue));
      }
      // Update global variables
      brightnessOverrideEnabled = (strcmp(generalConfig.brightnessOverrideEnable, "on") == 0);
      brightnessOverrideValue = atoi(generalConfig.brightnessOverrideValue);
      saveGeneralConfiguration(generalConfigFile, generalConfig);
    }

    // Apply clock config
    if (!doc["clock"].isNull()) {
      JsonObject clock = doc["clock"];
      if (!clock["enabled"].isNull()) {
        strlcpy(clockConfig.enabled, clock["enabled"], sizeof(clockConfig.enabled));
        clockEnabled = (strcmp(clockConfig.enabled, "on") == 0);
      }
      if (!clock["ntpServer"].isNull()) strlcpy(clockConfig.ntpServer, clock["ntpServer"], sizeof(clockConfig.ntpServer));
      if (!clock["tzString"].isNull()) strlcpy(clockConfig.tzString, clock["tzString"], sizeof(clockConfig.tzString));
      if (!clock["dateFormat"].isNull()) strlcpy(clockConfig.dateFormat, clock["dateFormat"], sizeof(clockConfig.dateFormat));
      if (!clock["brightness"].isNull()) {
        strlcpy(clockConfig.brightness, clock["brightness"], sizeof(clockConfig.brightness));
        clockBrightness = atoi(clockConfig.brightness);
      }
      if (!clock["transitionDelayMs"].isNull()) strlcpy(clockConfig.transitionDelayMs, clock["transitionDelayMs"], sizeof(clockConfig.transitionDelayMs));
      if (!clock["transitionSpeed"].isNull()) strlcpy(clockConfig.transitionSpeed, clock["transitionSpeed"], sizeof(clockConfig.transitionSpeed));
      if (!clock["transitionEffect"].isNull()) strlcpy(clockConfig.transitionEffect, clock["transitionEffect"], sizeof(clockConfig.transitionEffect));
      if (!clock["randomizeTransition"].isNull()) strlcpy(clockConfig.randomizeTransition, clock["randomizeTransition"], sizeof(clockConfig.randomizeTransition));
      if (!clock["resyncIntervalHours"].isNull()) strlcpy(clockConfig.resyncIntervalHours, clock["resyncIntervalHours"], sizeof(clockConfig.resyncIntervalHours));
      if (!clock["clockFace"].isNull()) strlcpy(clockConfig.clockFace, clock["clockFace"], sizeof(clockConfig.clockFace));
      if (!clock["dateAlternate"].isNull()) strlcpy(clockConfig.dateAlternate, clock["dateAlternate"], sizeof(clockConfig.dateAlternate));
      if (!clock["dateAlternateSeconds"].isNull()) strlcpy(clockConfig.dateAlternateSeconds, clock["dateAlternateSeconds"], sizeof(clockConfig.dateAlternateSeconds));
      if (!clock["customDateFormat"].isNull()) strlcpy(clockConfig.customDateFormat, clock["customDateFormat"], sizeof(clockConfig.customDateFormat));
      if (!clock["clockAmPm"].isNull()) strlcpy(clockConfig.clockAmPm, clock["clockAmPm"], sizeof(clockConfig.clockAmPm));
      saveClockConfiguration(clockConfigFile, clockConfig);

      // Reinitialize NTP if clock enabled
      if (clockEnabled && WiFi.status() == WL_CONNECTED) {
        initNtpTime();
      }
    }

#ifndef DISABLE_TIMER_FEATURE
    // Apply timer config
    if (!doc["timer"].isNull()) {
      JsonObject timer = doc["timer"];
      if (!timer["enabled"].isNull()) {
        strlcpy(timerConfig.enabled, timer["enabled"], sizeof(timerConfig.enabled));
        timerEnabled = (strcmp(timerConfig.enabled, "on") == 0);
      }
      if (!timer["mode"].isNull()) strlcpy(timerConfig.mode, timer["mode"], sizeof(timerConfig.mode));
      if (!timer["durationSeconds"].isNull()) {
        strlcpy(timerConfig.durationSeconds, timer["durationSeconds"], sizeof(timerConfig.durationSeconds));
        timerTargetSeconds = atoi(timerConfig.durationSeconds);
      }
      if (!timer["brightness"].isNull()) {
        strlcpy(timerConfig.brightness, timer["brightness"], sizeof(timerConfig.brightness));
        timerBrightness = atoi(timerConfig.brightness);
      }
      if (!timer["alertBuzzer"].isNull()) strlcpy(timerConfig.alertBuzzer, timer["alertBuzzer"], sizeof(timerConfig.alertBuzzer));
      if (!timer["alertBuzzerCount"].isNull()) strlcpy(timerConfig.alertBuzzerCount, timer["alertBuzzerCount"], sizeof(timerConfig.alertBuzzerCount));
      if (!timer["alertChirp"].isNull()) strlcpy(timerConfig.alertChirp, timer["alertChirp"], sizeof(timerConfig.alertChirp));
      if (!timer["autoRepeat"].isNull()) strlcpy(timerConfig.autoRepeat, timer["autoRepeat"], sizeof(timerConfig.autoRepeat));
      saveTimerConfiguration(timerConfigFile, timerConfig);
    }
#endif

#ifndef DISABLE_SLEEP_MODE_FEATURE
    // Apply sleep mode config
    if (!doc["sleepMode"].isNull()) {
      JsonObject sleepMode = doc["sleepMode"];
      if (!sleepMode["enabled"].isNull()) {
        strlcpy(sleepModeConfig.enabled, sleepMode["enabled"], sizeof(sleepModeConfig.enabled));
        sleepModeEnabled = (strcmp(sleepModeConfig.enabled, "on") == 0);
      }
      if (!sleepMode["onTime"].isNull()) {
        strlcpy(sleepModeConfig.onTime, sleepMode["onTime"], sizeof(sleepModeConfig.onTime));
      }
      if (!sleepMode["offTime"].isNull()) {
        strlcpy(sleepModeConfig.offTime, sleepMode["offTime"], sizeof(sleepModeConfig.offTime));
      }
      if (!sleepMode["muteOnly"].isNull()) {
        strlcpy(sleepModeConfig.muteOnly, sleepMode["muteOnly"], sizeof(sleepModeConfig.muteOnly));
        sleepModeMuteOnly = (strcmp(sleepModeConfig.muteOnly, "on") == 0);
      }
      saveSleepModeConfiguration(sleepModeConfigFile, sleepModeConfig);

      // Recalculate active state after import
      updateSleepModeState();
    }
#endif

#ifndef DISABLE_ALARM_FEATURE
    // Apply alarm config
    if (!doc["alarm"].isNull()) {
      JsonObject alarm = doc["alarm"];
      if (!alarm["masterEnabled"].isNull()) {
        strlcpy(alarmConfig.masterEnabled, alarm["masterEnabled"], sizeof(alarmConfig.masterEnabled));
        alarmMasterEnabled = (strcmp(alarmConfig.masterEnabled, "on") == 0);
      }

      if (!alarm["alarms"].isNull()) {
        JsonArray alarmsArray = alarm["alarms"].as<JsonArray>();
        int index = 0;
        for (JsonObject alarmItem : alarmsArray) {
          if (index >= MAX_ALARMS) break;

          if (!alarmItem["enabled"].isNull()) {
            strlcpy(alarmConfig.alarms[index].enabled, alarmItem["enabled"], sizeof(alarmConfig.alarms[index].enabled));
          }
          if (!alarmItem["time"].isNull()) {
            strlcpy(alarmConfig.alarms[index].time, alarmItem["time"], sizeof(alarmConfig.alarms[index].time));
          }
          if (!alarmItem["daysOfWeek"].isNull()) {
            strlcpy(alarmConfig.alarms[index].daysOfWeek, alarmItem["daysOfWeek"], sizeof(alarmConfig.alarms[index].daysOfWeek));
          }
          if (!alarmItem["message"].isNull()) {
            strlcpy(alarmConfig.alarms[index].message, alarmItem["message"], sizeof(alarmConfig.alarms[index].message));
          }
          if (!alarmItem["chirpName"].isNull()) {
            strlcpy(alarmConfig.alarms[index].chirpName, alarmItem["chirpName"], sizeof(alarmConfig.alarms[index].chirpName));
          }
          if (!alarmItem["repeatCount"].isNull()) {
            strlcpy(alarmConfig.alarms[index].repeatCount, alarmItem["repeatCount"], sizeof(alarmConfig.alarms[index].repeatCount));
          }
          index++;
        }
      }

      saveAlarmConfiguration(alarmConfigFile, alarmConfig);

      // Reset alarm trigger tracking after import
      for (int i = 0; i < MAX_ALARMS; i++) {
        lastTriggeredAlarmMinute[i] = -1;
      }
    }
#endif

#ifndef DISABLE_WEATHER_FEATURE
    // Apply weather config
    if (!doc["weather"].isNull()) {
      JsonObject weather = doc["weather"];
      if (!weather["enabled"].isNull()) {
        strlcpy(weatherConfig.enabled, weather["enabled"], sizeof(weatherConfig.enabled));
        weatherEnabled = (strcmp(weatherConfig.enabled, "on") == 0);
      }
      if (!weather["location"].isNull()) {
        strlcpy(weatherConfig.location, weather["location"], sizeof(weatherConfig.location));
      }
      if (!weather["latitude"].isNull()) {
        strlcpy(weatherConfig.latitude, weather["latitude"], sizeof(weatherConfig.latitude));
      }
      if (!weather["longitude"].isNull()) {
        strlcpy(weatherConfig.longitude, weather["longitude"], sizeof(weatherConfig.longitude));
      }
      if (!weather["updateIntervalMinutes"].isNull()) {
        strlcpy(weatherConfig.updateIntervalMinutes, weather["updateIntervalMinutes"], sizeof(weatherConfig.updateIntervalMinutes));
      }
      if (!weather["temperatureUnit"].isNull()) {
        strlcpy(weatherConfig.temperatureUnit, weather["temperatureUnit"], sizeof(weatherConfig.temperatureUnit));
      }
      if (!weather["brightness"].isNull()) {
        strlcpy(weatherConfig.brightness, weather["brightness"], sizeof(weatherConfig.brightness));
        weatherBrightness = atoi(weatherConfig.brightness);
      }
      if (!weather["displayDurationSeconds"].isNull()) {
        strlcpy(weatherConfig.displayDurationSeconds, weather["displayDurationSeconds"], sizeof(weatherConfig.displayDurationSeconds));
      }
      saveWeatherConfiguration(weatherConfigFile, weatherConfig);

      // Reset fetch timer to trigger new fetch with new coordinates
      lastWeatherFetch = 0;
    }
#endif

    // Apply recurrent alarm config
    if (!doc["recurrentAlarm"].isNull()) {
      JsonObject recurrentAlarm = doc["recurrentAlarm"];
      if (!recurrentAlarm["enabled"].isNull()) {
        strlcpy(recurrentAlarmConfig.enabled, recurrentAlarm["enabled"], sizeof(recurrentAlarmConfig.enabled));
        recurrentAlarmEnabled = (strcmp(recurrentAlarmConfig.enabled, "on") == 0);
      }
      if (!recurrentAlarm["chirpName"].isNull()) {
        strlcpy(recurrentAlarmConfig.chirpName, recurrentAlarm["chirpName"], sizeof(recurrentAlarmConfig.chirpName));
      }
      if (!recurrentAlarm["interval"].isNull()) {
        strlcpy(recurrentAlarmConfig.interval, recurrentAlarm["interval"], sizeof(recurrentAlarmConfig.interval));
        recurrentAlarmIntervalMinutes = getIntervalMinutes(recurrentAlarmConfig.interval);
      }
      if (!recurrentAlarm["disableWeekends"].isNull()) {
        strlcpy(recurrentAlarmConfig.disableWeekends, recurrentAlarm["disableWeekends"], sizeof(recurrentAlarmConfig.disableWeekends));
        recurrentAlarmDisableWeekends = (strcmp(recurrentAlarmConfig.disableWeekends, "on") == 0);
      }
      saveRecurrentAlarmConfiguration(recurrentAlarmConfigFile, recurrentAlarmConfig);
    }

    // Apply WiFi config
    if (!doc["wifi"].isNull()) {
      JsonObject wifi = doc["wifi"];
      if (!wifi["ssid"].isNull() && !wifi["password"].isNull()) {
        String ssid = wifi["ssid"].as<String>();
        String pass = wifi["password"].as<String>();
        if (ssid.length() > 0) {
           PRINTS("Restoring WiFi credentials...\n");
           // WiFi.begin saves to persistent flash by default on ESP8266
           WiFi.begin(ssid.c_str(), pass.c_str());
        }
      }
    }
    
    PRINTS("Config imported successfully\n");
    serverHttp.send(200, "text/plain", "OK");
    },
    []() {
      // This upload handler runs DURING the POST request to collect the body
      HTTPUpload& upload = serverHttp.upload();
      
      if (upload.status == UPLOAD_FILE_START) {
        uploadedConfigJson = "";  // Clear any previous data
        PRINTS("Starting config upload...\n");
      }
      else if (upload.status == UPLOAD_FILE_WRITE) {
        // Append chunk to buffer
        for (size_t i = 0; i < upload.currentSize; i++) {
          uploadedConfigJson += (char)upload.buf[i];
        }
      }
      else if (upload.status == UPLOAD_FILE_END) {
        PRINTS("Upload complete, size: ");
        Serial.println(uploadedConfigJson.length());
      }
    }
  );
  serverHttp.on("/deviceconfig", []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }
    showChangeCredentialsHttp();
  });
  serverHttp.on("/changecredentials", HTTP_POST, []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }
    usernamePasswordHttp();
    changeWebLoginCredentials();
    serverHttp.send(200, "text/plain", "OK");
  });

  // Clock configuration endpoints
  serverHttp.on("/clockpagevars", []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }
    setClockPageVars();
    serverHttp.send(200, "text/xml", clockPageVars);
  });

  serverHttp.on("/saveclocksettings", HTTP_POST, []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }

    // Track if enabled state or effect changed
    bool wasEnabled = clockEnabled;
    char previousEffect[STDSIZE];
    strlcpy(previousEffect, clockConfig.transitionEffect, sizeof(previousEffect));

    // Parse form parameters
    for (uint8_t i = 0; i < serverHttp.args(); i++) {
      String argName = serverHttp.argName(i);
      String argValue = serverHttp.arg(i);

      if (argName == "ClockEnabled") {
        argValue.toCharArray(clockConfig.enabled, sizeof(clockConfig.enabled));
        clockEnabled = (argValue == "on");
      } else if (argName == "NtpServer") {
        argValue.toCharArray(clockConfig.ntpServer, sizeof(clockConfig.ntpServer));
      } else if (argName == "TzString") {
        argValue.toCharArray(clockConfig.tzString, sizeof(clockConfig.tzString));
      } else if (argName == "DateFormat") {
        argValue.toCharArray(clockConfig.dateFormat, sizeof(clockConfig.dateFormat));
      } else if (argName == "DateAlternate") {
        argValue.toCharArray(clockConfig.dateAlternate, sizeof(clockConfig.dateAlternate));
      } else if (argName == "DateAlternateSeconds") {
        argValue.toCharArray(clockConfig.dateAlternateSeconds, sizeof(clockConfig.dateAlternateSeconds));
      } else if (argName == "ClockAmPm") {
        argValue.toCharArray(clockConfig.clockAmPm, sizeof(clockConfig.clockAmPm));
      } else if (argName == "CustomDateFormat") {
        argValue.toCharArray(clockConfig.customDateFormat, sizeof(clockConfig.customDateFormat));
      } else if (argName == "ClockFace") {
        argValue.toCharArray(clockConfig.clockFace, sizeof(clockConfig.clockFace));
      } else if (argName == "ClockBrightness") {
        argValue.toCharArray(clockConfig.brightness, sizeof(clockConfig.brightness));
        clockBrightness = argValue.toInt();
      } else if (argName == "TransitionDelay") {
        argValue.toCharArray(clockConfig.transitionDelayMs, sizeof(clockConfig.transitionDelayMs));
      } else if (argName == "TransitionEffect") {
        argValue.toCharArray(clockConfig.transitionEffect, sizeof(clockConfig.transitionEffect));
      } else if (argName == "RandomizeTransition") {
        argValue.toCharArray(clockConfig.randomizeTransition, sizeof(clockConfig.randomizeTransition));
      } else if (argName == "TransitionSpeed") {
        argValue.toCharArray(clockConfig.transitionSpeed, sizeof(clockConfig.transitionSpeed));
      }
    }

    // Enforce hardware constraints before saving
    #if MAX_DEVICES == 4
    // TIME_SECONDS + AM/PM exceeds 32px on 4m — force 24h when seconds are shown
    if (strcmp(clockConfig.dateFormat, "TIME_SECONDS") == 0)
      strlcpy(clockConfig.clockAmPm, "off", sizeof(clockConfig.clockAmPm));
    #endif

    // Save configuration to file
    saveClockConfiguration(clockConfigFile, clockConfig);

    // Reset date alternate state to ensure immediate display update
    clockAlternateState = 0;
    lastDateAlternate = millis();

    // Always apply timezone (even if clock display is disabled)
    // This matches MQTT behavior where timezone changes take effect immediately
    applyTimezone();

    // Re-initialize NTP with new settings (handles clock enabled/disabled state internally)
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println(F("Clock settings changed, re-initializing NTP..."));
      initNtpTime();
    }

    // Force display update if clock is currently showing
    if (currentDisplayMode == MODE_CLOCK) {
      displayClock(false);
    }

    PRINT("Clock settings saved - Enabled: ", clockConfig.enabled);
    PRINT(", NTP Server: ", clockConfig.ntpServer);
    PRINTS("\n");

    // Check what changed
    bool enabledChanged = (wasEnabled != clockEnabled);
    bool effectChanged = (strcmp(previousEffect, clockConfig.transitionEffect) != 0);
    
    // If clock was just enabled, trigger entrance animation
    if (enabledChanged && clockEnabled && currentDisplayMode == MODE_MESSAGE) {
      currentDisplayMode = MODE_TRANSITION;
      transitionStartTime = millis();
      P.displayClear();
      Serial.println(F("Clock enabled, triggering entrance animation"));
    }
    
    // If effect changed while clock is enabled and displaying, retrigger entrance animation
    if (!enabledChanged && effectChanged && clockEnabled && currentDisplayMode == MODE_CLOCK) {
      currentDisplayMode = MODE_TRANSITION;
      transitionStartTime = millis();
      P.displayClear();
      Serial.println(F("Transition effect changed, retriggering animation"));
    }
    
    // If clock was just disabled while displaying, animate it off
    if (enabledChanged && !clockEnabled && (currentDisplayMode == MODE_CLOCK || currentDisplayMode == MODE_TRANSITION)) {
      currentDisplayMode = MODE_CLOCK_EXIT;
      
      // Set up exit animation
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
        // Pick a random effect for exit
        int effectIndex = random(0, 25);
        textEffect_t effects[] = {PA_FADE, PA_DISSOLVE, PA_BLINDS, PA_WIPE, PA_GROW_UP, PA_SCROLL_UP, PA_SCAN_HORIZ, PA_OPENING, PA_CLOSING, PA_SCROLL_DOWN, PA_SCROLL_LEFT, PA_SCROLL_RIGHT, PA_SLICE, PA_MESH, PA_GROW_DOWN, PA_SCAN_VERTX, PA_SCROLL_UP_LEFT, PA_SCROLL_UP_RIGHT, PA_OPENING_CURSOR, PA_SCAN_VERT, PA_SCROLL_DOWN_LEFT, PA_WIPE_CURSOR, PA_SCAN_HORIZX, PA_CLOSING_CURSOR, PA_SCROLL_DOWN_RIGHT};
        effect = effects[effectIndex];
      } else {
        effect = getTransitionEffect(clockConfig.transitionEffect);
      }
      uint16_t animSpeed = (scrollDelay < 40) ? 40 : scrollDelay;
      P.displayText(exitTimeString, PA_CENTER, animSpeed, 0, PA_PRINT, effect);
      P.displayReset();
      
      Serial.println(F("Clock disabled, animating off"));
    }

    serverHttp.send(200, "text/plain", "OK");
  });

  // MatrixLight8 font character test — scrolls all converted glyphs once
  serverHttp.on("/matrixfonttest", HTTP_GET, []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }
    repeatCount = 0;
    strcpy(newMessage, "0123456789 AaBbCcDd EeFfGg HhIiJj KkLlMm NnOoPp QqRrSs TtUuVv WwXxYy Zz !?+-:.");
    strcpy(newRepeat, "1");
    strcpy(newBuz, "0");
    strcpy(newDelay, scrollDelayDefault);
    strcpy(newBrightness, ledBrightnessDefault);
    strcpy(newAsciiConv, "0");
    newMessageAvailable = true;
    newRepeatAvailable = true;
    newBuzAvailable = true;
    newDelayAvailable = true;
    newBrightnessAvailable = true;
    newAsciiConvAvailable = true;
    explicitBuzzerCount = true;
    matrixFontTest = true;
    serverHttp.send(204, "");
  });

  // Timer configuration endpoints
  #ifndef DISABLE_TIMER_FEATURE
  serverHttp.on("/timerpagevars", []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }
    setTimerVars();
  });

  serverHttp.on("/savetimersettings", HTTP_POST, []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }

    // Parse form parameters
    for (uint8_t i = 0; i < serverHttp.args(); i++) {
      String argName = serverHttp.argName(i);
      String argValue = serverHttp.arg(i);

      if (argName == "TimerEnabled") {
        argValue.toCharArray(timerConfig.enabled, sizeof(timerConfig.enabled));
        timerEnabled = (argValue == "on");
      } else if (argName == "TimerMode") {
        argValue.toCharArray(timerConfig.mode, sizeof(timerConfig.mode));
      } else if (argName == "DurationSeconds") {
        argValue.toCharArray(timerConfig.durationSeconds, sizeof(timerConfig.durationSeconds));
        timerTargetSeconds = argValue.toInt();
      } else if (argName == "TimerBrightness") {
        argValue.toCharArray(timerConfig.brightness, sizeof(timerConfig.brightness));
        timerBrightness = argValue.toInt();
      } else if (argName == "AlertBuzzer") {
        argValue.toCharArray(timerConfig.alertBuzzer, sizeof(timerConfig.alertBuzzer));
      } else if (argName == "AlertBuzzerCount") {
        argValue.toCharArray(timerConfig.alertBuzzerCount, sizeof(timerConfig.alertBuzzerCount));
      } else if (argName == "AlertChirp") {
        // Validate chirp name exists in library
        if (getChirpByName(argValue.c_str()) != nullptr) {
          argValue.toCharArray(timerConfig.alertChirp, sizeof(timerConfig.alertChirp));
        }
      } else if (argName == "AutoRepeat") {
        argValue.toCharArray(timerConfig.autoRepeat, sizeof(timerConfig.autoRepeat));
      }
    }

    // Save configuration to file
    saveTimerConfiguration(timerConfigFile, timerConfig);

    // Publish new settings to MQTT
    publishTimerEnableState();
    publishTimerModeState();
    publishTimerDurationState();
    publishTimerBrightnessState();
    publishTimerAlertBuzzerState();
    publishTimerAutoRepeatState();
    publishTimerAlertChirpState();

    PRINT("Timer settings saved - Enabled: ", timerConfig.enabled);
    PRINT(", Mode: ", timerConfig.mode);
    PRINTS("\n");

    serverHttp.send(200, "text/plain", "OK");
  });

  // Chirp preview endpoint for timer alerts
  serverHttp.on("/previewchirp", HTTP_POST, []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }

    #ifndef DISABLE_SLEEP_MODE_FEATURE
    // Respect sleep mode - don't play sounds during sleep hours
    if (isSleepModeActive()) {
      PRINTS("Preview blocked: Sleep mode is active\n");
      serverHttp.send(403, "text/plain", "Sleep mode active");
      return;
    }
    #endif

    String chirpName = "";
    String beepCountStr = "";
    for (uint8_t i = 0; i < serverHttp.args(); i++) {
      if (serverHttp.argName(i) == "chirp") {
        chirpName = serverHttp.arg(i);
      } else if (serverHttp.argName(i) == "count") {
        beepCountStr = serverHttp.arg(i);
      }
    }

    // Handle Simple Beep mode
    if (chirpName == "Simple Beep") {
      int beepCount = beepCountStr.toInt();
      if (beepCount < 1) beepCount = 5; // Default to 5 if invalid
      if (beepCount > 20) beepCount = 20; // Cap at 20
      
      PRINTS("\n[DEBUG] Web Preview: Simple Beep (using LEDC/digitalWrite)");
      PRINT("Beep count: ", beepCount);
      PRINTS("\n");
      
      // Send response BEFORE playing to prevent browser timeout/retry
      serverHttp.send(200, "text/plain", "OK");
      
      for (int i = 0; i < beepCount; i++) {
        #ifdef ESP32
        ledcWriteTone(0, 1000);  // 1kHz tone
        delay(100);
        ledcWriteTone(0, 0);
        #else
        digitalWrite(BUZZER_PIN, HIGH);
        delay(100);
        digitalWrite(BUZZER_PIN, LOW);
        #endif
        delay(100);
      }
      PRINTS("[DEBUG] Web Preview Simple Beep completed\n");
    } else if (chirpName.length() > 0 && getChirpByName(chirpName.c_str()) != nullptr) {
      int repeatCount = beepCountStr.toInt();
      if (repeatCount < 1) repeatCount = 1; // Default to 1 if invalid
      if (repeatCount > 20) repeatCount = 20; // Cap at 20
      
      PRINTS("Previewing chirp: ");
      Serial.println(chirpName);
      
      // Send response BEFORE playing to prevent browser timeout/retry
      serverHttp.send(200, "text/plain", "OK");
      
      playChirpByName(chirpName.c_str(), repeatCount);
    } else {
      serverHttp.send(400, "text/plain", "Invalid chirp name");
    }
  });
  #endif

  #ifndef DISABLE_SLEEP_MODE_FEATURE
  serverHttp.on("/sleepmodeparevars", []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }
    setSleepModeVars();
  });

  serverHttp.on("/savesleepmodesettings", HTTP_POST, []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }

    // Parse form parameters
    for (uint8_t i = 0; i < serverHttp.args(); i++) {
      String argName = serverHttp.argName(i);
      String argValue = serverHttp.arg(i);

      if (argName == "SleepModeEnabled") {
        argValue.toCharArray(sleepModeConfig.enabled, sizeof(sleepModeConfig.enabled));
        sleepModeEnabled = (argValue == "on");
      } else if (argName == "SleepModeOnTime") {
        argValue.toCharArray(sleepModeConfig.onTime, sizeof(sleepModeConfig.onTime));
      } else if (argName == "SleepModeOffTime") {
        argValue.toCharArray(sleepModeConfig.offTime, sizeof(sleepModeConfig.offTime));
      } else if (argName == "SleepModeMuteOnly") {
        argValue.toCharArray(sleepModeConfig.muteOnly, sizeof(sleepModeConfig.muteOnly));
        sleepModeMuteOnly = (argValue == "on");
      } else if (argName == "WeekendEnabled") {
        argValue.toCharArray(sleepModeConfig.weekendEnabled, sizeof(sleepModeConfig.weekendEnabled));
      } else if (argName == "WeekendOnTime") {
        argValue.toCharArray(sleepModeConfig.weekendOnTime, sizeof(sleepModeConfig.weekendOnTime));
      } else if (argName == "WeekendOffTime") {
        argValue.toCharArray(sleepModeConfig.weekendOffTime, sizeof(sleepModeConfig.weekendOffTime));
      }
    }

    // Save configuration
    saveSleepModeConfiguration(sleepModeConfigFile, sleepModeConfig);

    // Publish to MQTT (will be implemented in next phase)
    // publishSleepModeState();

    PRINT("Sleep Mode settings saved - Enabled: ", sleepModeConfig.enabled);
    PRINTS("\n");

    serverHttp.send(200, "text/plain", "OK");
  });

  serverHttp.on("/api/sleepmode", HTTP_GET, []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }

    String json = "{";
    json += "\"enabled\":\"" + String(sleepModeConfig.enabled) + "\",";
    json += "\"onTime\":\"" + String(sleepModeConfig.onTime) + "\",";
    json += "\"offTime\":\"" + String(sleepModeConfig.offTime) + "\",";
    json += "\"muteOnly\":\"" + String(sleepModeConfig.muteOnly) + "\",";
    json += "\"weekendEnabled\":\"" + String(sleepModeConfig.weekendEnabled) + "\",";
    json += "\"weekendOnTime\":\"" + String(sleepModeConfig.weekendOnTime) + "\",";
    json += "\"weekendOffTime\":\"" + String(sleepModeConfig.weekendOffTime) + "\",";
    json += "\"active\":" + String(sleepModeActive ? "true" : "false") + ",";
    json += "\"clockEnabled\":" + String(clockEnabled ? "true" : "false");
    json += "}";

    serverHttp.send(200, "application/json", json);
  });
  #endif

  #ifndef DISABLE_ALARM_FEATURE
  // GET /api/alarm/config - Return current alarm configuration as JSON
  serverHttp.on("/api/alarm/config", HTTP_GET, []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }
    String json = getAlarmConfigJson();
    serverHttp.send(200, "application/json", json);
  });

  // POST /api/alarm/config - Save alarm configuration
  serverHttp.on("/api/alarm/config", HTTP_POST, []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }

    if (serverHttp.hasArg("plain")) {
      String body = serverHttp.arg("plain");
      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, body);

      if (error) {
        serverHttp.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
        return;
      }

      // Parse master enable
      const char* masterEnabled = doc["masterEnabled"] | "off";
      strlcpy(alarmConfig.masterEnabled, masterEnabled, STDSIZE);
      alarmMasterEnabled = (strcmp(alarmConfig.masterEnabled, "on") == 0);

      // Parse individual alarms
      JsonArray alarms = doc["alarms"].as<JsonArray>();
      int index = 0;
      for (JsonObject alarm : alarms) {
        if (index >= MAX_ALARMS) break;

        const char* enabled = alarm["enabled"] | "off";
        const char* time = alarm["time"] | "07:00";
        const char* daysOfWeek = alarm["daysOfWeek"] | "";
        const char* message = alarm["message"] | "Alarm!";
        const char* chirpName = alarm["chirpName"] | "Gentle Dawn";
        const char* repeatCount = alarm["repeatCount"] | "3";

        strlcpy(alarmConfig.alarms[index].enabled, enabled, STDSIZE);
        strlcpy(alarmConfig.alarms[index].time, time, STDSIZE);
        strlcpy(alarmConfig.alarms[index].daysOfWeek, daysOfWeek, 8);
        strlcpy(alarmConfig.alarms[index].message, message, ALARM_MESSAGE_SIZE);
        strlcpy(alarmConfig.alarms[index].chirpName, chirpName, STDSIZE);
        strlcpy(alarmConfig.alarms[index].repeatCount, repeatCount, STDSIZE);

        index++;
      }

      // Save to file
      saveAlarmConfiguration(alarmConfigFile, alarmConfig);

      PRINT("Alarm config saved - Master: ", alarmConfig.masterEnabled);
      PRINTS("\n");

      serverHttp.send(200, "application/json", "{\"status\":\"saved\"}");
    } else {
      serverHttp.send(400, "application/json", "{\"error\":\"No body\"}");
    }
  });

  // POST /api/alarm/stop - Stop all active alarms
  serverHttp.on("/api/alarm/stop", HTTP_POST, []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }
    stopAllAlarms();
    serverHttp.send(200, "application/json", "{\"status\":\"stopped\"}");
  });

  // GET /api/alarm/status - Return current alarm status
  serverHttp.on("/api/alarm/status", HTTP_GET, []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }
    String json = getAlarmStatusJson();
    serverHttp.send(200, "application/json", json);
  });

  // GET /api/chirps - Return list of available chirp patterns
  serverHttp.on("/api/chirps", HTTP_GET, []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }
    String json = getChirpsJson();
    serverHttp.send(200, "application/json", json);
  });

  // GET /api/recurrent_alarm/config - Return current recurrent alarm configuration
  serverHttp.on("/api/recurrent_alarm/config", HTTP_GET, []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }

    JsonDocument doc;
    doc["enabled"] = recurrentAlarmConfig.enabled;
    doc["chirpName"] = recurrentAlarmConfig.chirpName;
    doc["interval"] = recurrentAlarmConfig.interval;
    doc["disableWeekends"] = recurrentAlarmConfig.disableWeekends;

    String json;
    serializeJson(doc, json);
    serverHttp.send(200, "application/json", json);
  });

  // POST /api/recurrent_alarm/config - Save recurrent alarm configuration
  serverHttp.on("/api/recurrent_alarm/config", HTTP_POST, []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }

    if (serverHttp.hasArg("plain")) {
      String body = serverHttp.arg("plain");
      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, body);

      if (error) {
        serverHttp.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
        return;
      }

      // Parse configuration
      const char* enabled = doc["enabled"] | "off";
      const char* chirpName = doc["chirpName"] | "Gentle Dawn";
      const char* interval = doc["interval"] | "15min";
      const char* disableWeekends = doc["disableWeekends"] | "off";

      strlcpy(recurrentAlarmConfig.enabled, enabled, STDSIZE);
      strlcpy(recurrentAlarmConfig.chirpName, chirpName, STDSIZE);
      strlcpy(recurrentAlarmConfig.interval, interval, STDSIZE);
      strlcpy(recurrentAlarmConfig.disableWeekends, disableWeekends, STDSIZE);

      // Update runtime variables
      recurrentAlarmEnabled = (strcmp(recurrentAlarmConfig.enabled, "on") == 0);
      recurrentAlarmDisableWeekends = (strcmp(recurrentAlarmConfig.disableWeekends, "on") == 0);

      // Convert interval string to minutes
      if (strcmp(recurrentAlarmConfig.interval, "15min") == 0) {
        recurrentAlarmIntervalMinutes = 15;
      } else if (strcmp(recurrentAlarmConfig.interval, "30min") == 0) {
        recurrentAlarmIntervalMinutes = 30;
      } else if (strcmp(recurrentAlarmConfig.interval, "1hour") == 0) {
        recurrentAlarmIntervalMinutes = 60;
      } else if (strcmp(recurrentAlarmConfig.interval, "3hours") == 0) {
        recurrentAlarmIntervalMinutes = 180;
      } else if (strcmp(recurrentAlarmConfig.interval, "6hours") == 0) {
        recurrentAlarmIntervalMinutes = 360;
      } else if (strcmp(recurrentAlarmConfig.interval, "midday") == 0) {
        recurrentAlarmIntervalMinutes = -1; // Special value for midday trigger
      } else {
        recurrentAlarmIntervalMinutes = 15; // Default
      }

      // Save to file
      saveRecurrentAlarmConfiguration(recurrentAlarmConfigFile, recurrentAlarmConfig);

      // Publish MQTT state updates if HA discovery enabled
      publishRecurrentAlarmEnableState();
      publishRecurrentAlarmDisableWeekendsState();
      publishRecurrentAlarmIntervalState();
      publishRecurrentAlarmChirpState();

      PRINT("Recurrent alarm config saved - Enabled: ", recurrentAlarmConfig.enabled);
      PRINT(", Interval: ", recurrentAlarmConfig.interval);
      PRINTS("\n");

      serverHttp.send(200, "application/json", "{\"status\":\"saved\"}");
    } else {
      serverHttp.send(400, "application/json", "{\"error\":\"No body\"}");
    }
  });

  // POST /api/recurrent_alarm/stop - Stop any active recurrent alarm (no-op for now, but included for API completeness)
  serverHttp.on("/api/recurrent_alarm/stop", HTTP_POST, []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }
    // Recurrent alarm is a one-shot sound, so there's nothing to stop
    // This endpoint exists for API consistency
    serverHttp.send(200, "application/json", "{\"status\":\"stopped\"}");
  });
  #endif

  #ifndef DISABLE_WEATHER_FEATURE
  // GET /api/weather/status - Return current weather status and config as JSON
  serverHttp.on("/api/weather/status", HTTP_GET, []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }
    String json = getWeatherStatusJson();
    serverHttp.send(200, "application/json", json);
  });

  // POST /api/weather/save - Save weather configuration
  serverHttp.on("/api/weather/save", HTTP_POST, []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }

    if (serverHttp.hasArg("plain")) {
      String body = serverHttp.arg("plain");
      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, body);

      if (error) {
        serverHttp.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
        return;
      }

      // Parse configuration values
      const char* enabled = doc["enabled"] | "off";
      const char* location = doc["location"] | "";
      const char* latitude = doc["latitude"] | "";
      const char* longitude = doc["longitude"] | "";
      const char* updateInterval = doc["updateIntervalMinutes"] | "30";
      const char* tempUnit = doc["temperatureUnit"] | "C";
      const char* brightness = doc["brightness"] | "5";
      const char* displayDuration = doc["displayDurationSeconds"] | "10";

      // Update config struct
      strlcpy(weatherConfig.enabled, enabled, STDSIZE);
      strlcpy(weatherConfig.location, location, WEATHER_LOCATION_SIZE);
      strlcpy(weatherConfig.latitude, latitude, WEATHER_COORD_SIZE);
      strlcpy(weatherConfig.longitude, longitude, WEATHER_COORD_SIZE);
      strlcpy(weatherConfig.updateIntervalMinutes, updateInterval, STDSIZE);
      strlcpy(weatherConfig.temperatureUnit, tempUnit, STDSIZE);
      strlcpy(weatherConfig.brightness, brightness, BRI_SIZE);
      strlcpy(weatherConfig.displayDurationSeconds, displayDuration, STDSIZE);

      // Save to file
      saveWeatherConfiguration(weatherConfigFile, weatherConfig);

      // Update global state variables
      weatherEnabled = (strcmp(weatherConfig.enabled, "on") == 0);
      weatherBrightness = atoi(weatherConfig.brightness);

      // Reset fetch timer to trigger new fetch with new coordinates
      lastWeatherFetch = 0;

      PRINT("Weather config saved - Enabled: ", weatherConfig.enabled);
      PRINTS("\n");

      serverHttp.send(200, "application/json", "{\"status\":\"saved\"}");
    } else {
      serverHttp.send(400, "application/json", "{\"error\":\"No body\"}");
    }
  });

  // POST /api/weather/refresh - Trigger immediate weather data refresh
  serverHttp.on("/api/weather/refresh", HTTP_POST, []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }

    Serial.println(F("Weather refresh requested from web UI"));

    // Set flag for main loop to handle (avoids stack overflow on ESP8266)
    weatherRefreshRequested = true;

    // Return current status immediately (data will update on next loop iteration)
    String json = getWeatherStatusJson();
    serverHttp.send(200, "application/json", json);
  });
  #endif

  serverHttp.on("/mqttconfig", []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }
    showChangeMqttConfigHttp();
  });
  serverHttp.on("/mqttpagevars", []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }
    setMqttPageVars();
    serverHttp.send(200, "text/plane", mqttPageVars);
  });
  serverHttp.on("/applymqttconfig", []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }
    onMqttConfigChangeHttp();
    changeMqttConfig();
    serverHttp.send(200, "text/plain", "OK");
  });

#ifdef ESP32
  // Certificate upload endpoint (ESP32 only)
  serverHttp.on("/upload_cert", HTTP_POST, 
    []() {
      if (!serverHttp.authenticate(web_username, web_password)) {
        return serverHttp.requestAuthentication();
      }
      serverHttp.send(200, "text/plain", "OK");
    },
    []() {
      if (!serverHttp.authenticate(web_username, web_password)) {
        return serverHttp.requestAuthentication();
      }
      HTTPUpload& upload = serverHttp.upload();
      if (upload.status == UPLOAD_FILE_START) {
        PRINTS("\nReceiving CA certificate upload: ");
        Serial.println(upload.filename);
        File file = LittleFS.open("/ca_cert.pem", "w");
        if (!file) {
          PRINTS("\nERROR: Failed to open /ca_cert.pem for writing");
          return;
        }
        file.close();
      } else if (upload.status == UPLOAD_FILE_WRITE) {
        File file = LittleFS.open("/ca_cert.pem", "a");
        if (file) {
          file.write(upload.buf, upload.currentSize);
          file.close();
        }
      } else if (upload.status == UPLOAD_FILE_END) {
        PRINTS("\nCA certificate upload complete. Size: ");
        Serial.print(upload.totalSize);
        PRINTS(" bytes\n");
      }
    }
  );

  // Certificate delete endpoint (ESP32 only)
  serverHttp.on("/delete_cert", HTTP_GET, []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }
    if (LittleFS.exists("/ca_cert.pem")) {
      LittleFS.remove("/ca_cert.pem");
      PRINTS("\nCA certificate deleted\n");
      serverHttp.send(200, "text/plain", "Certificate deleted");
    } else {
      serverHttp.send(404, "text/plain", "Certificate not found");
    }
  });
#endif
  serverHttp.on("/system", HTTP_GET, []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }
    serverHttp.sendHeader("Connection", "close");
    serverHttp.send_P(200, "text/html", UPDATE_page);
  });
  serverHttp.on("/reboot", HTTP_GET, []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }
    serverHttp.sendHeader("Connection", "close");
    serverHttp.send_P(200, "text/html", REBOOT_page);
    delay(2000);
    rebootDevice();
  });
  serverHttp.on("/factoryreset", HTTP_GET, []() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }
    serverHttp.sendHeader("Connection", "close");
    serverHttp.send_P(200, "text/html", FACTORYRESET_page);
    factoryReset();
  });
  serverHttp.on(
      "/submitupdate", HTTP_POST,
      []() {
        if (!serverHttp.authenticate(web_username, web_password)) {
          return serverHttp.requestAuthentication();
        }
        serverHttp.sendHeader("Connection", "close");
        serverHttp.send_P(200, "text/html",
                        (Update.hasError()) ? SUBMITUPDATEFAIL_page
                                            : SUBMITUPDATEOK_page);
      },
      []() {
        if (!serverHttp.authenticate(web_username, web_password)) {
          return serverHttp.requestAuthentication();
        }
        HTTPUpload &upload = serverHttp.upload();
        if (upload.status == UPLOAD_FILE_START) {
          Serial.setDebugOutput(true);
#ifdef ESP8266
          WiFiUDP::stopAll();
#endif
          Serial.printf("Update: %s\n", upload.filename.c_str());
#ifdef ESP8266
          uint32_t maxSketchSpace =
              (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
          if (!Update.begin(maxSketchSpace)) { // start with max available size
#elif defined(ESP32)
          if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { // ESP32 uses UPDATE_SIZE_UNKNOWN
#endif
            Update.printError(Serial);
          }
        } else if (upload.status == UPLOAD_FILE_WRITE) {
          if (Update.write(upload.buf, upload.currentSize) !=
              upload.currentSize) {
            Update.printError(Serial);
          }
        } else if (upload.status == UPLOAD_FILE_END) {
          if (Update.end(true)) { // true to set the size to the current
                                  // progress
            Serial.printf("Update Success: %u\nRebooting...\n",
                          upload.totalSize);
            serverHttp.send_P(200, "text/html", SUBMITUPDATESUCCESS_page);
            delay(1000);
            ESP.restart();
          } else {
            Update.printError(Serial);
          }
          Serial.setDebugOutput(false);
        }
        yield();
      });
  serverHttp.onNotFound([]() {
    if (!serverHttp.authenticate(web_username, web_password)) {
      return serverHttp.requestAuthentication();
    }
    onNotFoundUriHttp();
  });

  // Start the http server
  serverHttp.begin();
  PRINTS("HTTP Server started on port 80\n");
  Serial.printf("You can update firmware from the browser opening! -> Open "
                "http://%s/update in your browser\n\n",
                assignedIP);
}

void handleHttpServer() { serverHttp.handleClient(); }

// ################################ END OF SPECIFIC HTTP SERVER FUNCTIONS
// ################################//