#include "config_manager.h"
#include "globals.h"
#ifndef DISABLE_TIMER_FEATURE
#include "timer.h"
#endif

// Loads the configuration from a file
void loadConfiguration(const char *webConfigFile, webConfigObj &webConfig) {

  File file = LittleFS.open(webConfigFile, "r");
  if (!file) {
    Serial.println("Failed to open data file");
    return;
  }

  // Allocate a temporary JsonDocument
  JsonDocument doc;

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error)
    Serial.println(F("Failed to read file, using default configuration"));

  // Copy values from the JsonDocument to the webConfig
  strlcpy(webConfig.usernameWebHolder,          // <- destination
          doc["usernameWebHolder"],             // <- source
          sizeof(webConfig.usernameWebHolder)); // <- destination's capacity
  strlcpy(webConfig.passwordWebHolder,          // <- destination
          doc["passwordWebHolder"],             // <- source
          sizeof(webConfig.passwordWebHolder)); // <- destination's capacity

  // Close the file (Curiously, File's destructor doesn't close the file)
  if (!doc["hostnameWebHolder"].isNull()) {
    strlcpy(webConfig.hostnameWebHolder, doc["hostnameWebHolder"],
            sizeof(webConfig.hostnameWebHolder));
  }
  file.close();
}

// Saves the configuration to a file
void saveConfiguration(const char *webConfigFile,
                       const webConfigObj &webConfig) {
  // Delete existing file, otherwise the configuration is appended to the file
  // LittleFS.remove(webConfigFile);

  // Open file for writing
  File file = LittleFS.open(webConfigFile, "w");
  if (!file) {
    Serial.println("Failed to open config file for writing");
    return;
  }
  // Allocate a temporary JsonDocument
  JsonDocument doc;

  // Set the values in the document
  doc["usernameWebHolder"] = webConfig.usernameWebHolder;
  doc["passwordWebHolder"] = webConfig.passwordWebHolder;
  doc["hostnameWebHolder"] = webConfig.hostnameWebHolder;

  // Serialize JSON to file
  if (serializeJson(doc, file) == 0) {
    Serial.println(F("Failed to write to file"));
  }

  // Close the file
  file.close();
}

// Prints the content of a file to the Serial
void printWebFile(const char *webConfigFile) {
  // Open file for reading

  File file = LittleFS.open(webConfigFile, "r");
  if (!file) {
    Serial.println("Failed to open data file");
    return;
  }
  // Extract each characters by one by one
  while (file.available()) {
    Serial.print((char)file.read());
  }
  Serial.println();

  // Close the file
  file.close();
}

// change login credentials and store into config file
void changeWebLoginCredentials() {
  // set hostname from webpage to config object (if provided)
  if (newWebHostnameAvailable && strlen(newWebHostname) > 0) {
    strlcpy(webConfig.hostnameWebHolder, newWebHostname,
            sizeof(webConfig.hostnameWebHolder));
    clientId = String(webConfig.hostnameWebHolder);
  }
  
  // only update username if a new one was provided (not empty)
  if (newWebUsernameAvailable && strlen(newWebUsername) > 0) {
    strlcpy(webConfig.usernameWebHolder, newWebUsername,
            sizeof(webConfig.usernameWebHolder));
  }
  
  // only update password if a new one was provided (not empty)
  if (newWebPasswordAvailable && strlen(newWebPassword) > 0) {
    strlcpy(webConfig.passwordWebHolder, newWebPassword,
            sizeof(webConfig.passwordWebHolder));
  }
  
  // save username and password from config object to config file
  saveConfiguration(webConfigFile, webConfig);
  // set the http/https credentials
  strlcpy(web_username, webConfig.usernameWebHolder, sizeof(web_username));
  strlcpy(web_password, webConfig.passwordWebHolder, sizeof(web_password));
  // Dump config file
  PRINTS("Username and Password changed\nPrinting web user config file:\n");
  printWebFile(webConfigFile);
}

void initWebStoreConfig() {
  // load config stored in config file
  Serial.println(F("Loading web configuration...\n"));
  loadConfiguration(webConfigFile, webConfig);
  // if no username is defined in config file store default
  if ((webConfig.usernameWebHolder != NULL) &&
      (webConfig.usernameWebHolder[0] == '\0')) {
    PRINT("no username set, setting default username: ", web_username);
    strlcpy(webConfig.usernameWebHolder, web_username,
            sizeof(webConfig.usernameWebHolder));
    saveWebConfigAtStart = true;
  }
  // if no password is defined in config file store default
  if ((webConfig.passwordWebHolder != NULL) &&
      (webConfig.passwordWebHolder[0] == '\0')) {
    PRINTS("\n")
    PRINT("no password set, setting default password: ", web_password);
    strlcpy(webConfig.passwordWebHolder, web_password,
            sizeof(webConfig.passwordWebHolder));
    saveWebConfigAtStart = true;
  }
  // Handle Hostname
  if ((webConfig.hostnameWebHolder != NULL) &&
      (webConfig.hostnameWebHolder[0] != '\0')) {
    clientId = String(webConfig.hostnameWebHolder);
  } else {
    clientId.toCharArray(webConfig.hostnameWebHolder,
                         sizeof(webConfig.hostnameWebHolder));
    saveWebConfigAtStart = true;
  }

  PRINTS("\n")
  // set http/https server to config file defined values or defined default
  strlcpy(web_username, webConfig.usernameWebHolder, sizeof(web_username));
  strlcpy(web_password, webConfig.passwordWebHolder, sizeof(web_password));

  // Create configuration file
  if (saveWebConfigAtStart) {
    Serial.println(F("Saving web user configuration..."));
    saveConfiguration(webConfigFile, webConfig);
  }

  // Dump config file
  Serial.println(F("Print web user config file...\n"));
  printWebFile(webConfigFile);
}

// ################################ DEFAULTS CONFIG FUNCTIONS ################################//

// Loads the defaults configuration from a file
void loadDefaultsConfiguration(const char *configFile, defaultsConfigObj &config) {
  File file = LittleFS.open(configFile, "r");
  if (!file) {
    Serial.println("Failed to open defaults config file");
    return;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file);
  if (error)
    Serial.println(F("Failed to read defaults file, using defaults"));

  if (!doc["repeatDefault"].isNull())
    strlcpy(config.repeatDefaultHolder, doc["repeatDefault"], sizeof(config.repeatDefaultHolder));
  if (!doc["buzzerDefault"].isNull())
    strlcpy(config.buzzerDefaultHolder, doc["buzzerDefault"], sizeof(config.buzzerDefaultHolder));
  if (!doc["scrollDelayDefault"].isNull())
    strlcpy(config.scrollDelayDefaultHolder, doc["scrollDelayDefault"], sizeof(config.scrollDelayDefaultHolder));
  if (!doc["brightnessDefault"].isNull())
    strlcpy(config.brightnessDefaultHolder, doc["brightnessDefault"], sizeof(config.brightnessDefaultHolder));
  if (!doc["alertChirpDefault"].isNull())
    strlcpy(config.alertChirpDefaultHolder, doc["alertChirpDefault"], sizeof(config.alertChirpDefaultHolder));

  file.close();
}

// Saves the defaults configuration to a file
void saveDefaultsConfiguration(const char *configFile, const defaultsConfigObj &config) {
  File file = LittleFS.open(configFile, "w");
  if (!file) {
    Serial.println("Failed to open defaults config file for writing");
    return;
  }

  JsonDocument doc;
  doc["repeatDefault"] = config.repeatDefaultHolder;
  doc["buzzerDefault"] = config.buzzerDefaultHolder;
  doc["scrollDelayDefault"] = config.scrollDelayDefaultHolder;
  doc["brightnessDefault"] = config.brightnessDefaultHolder;
  doc["alertChirpDefault"] = config.alertChirpDefaultHolder;

  if (serializeJson(doc, file) == 0) {
    Serial.println(F("Failed to write defaults file"));
  }

  file.close();
}

// Initialize defaults from flash or use hardcoded defaults
void initDefaultsStoreConfig() {
  Serial.println(F("Loading defaults configuration...\n"));
  loadDefaultsConfiguration(defaultsConfigFile, defaultsConfig);
  
  bool saveAtStart = false;
  
  // If no repeat default set, use hardcoded default
  if (defaultsConfig.repeatDefaultHolder[0] == '\0') {
    strlcpy(defaultsConfig.repeatDefaultHolder, repeatDefault, sizeof(defaultsConfig.repeatDefaultHolder));
    saveAtStart = true;
  } else {
    strlcpy(repeatDefault, defaultsConfig.repeatDefaultHolder, sizeof(repeatDefault));
  }
  
  // If no buzzer default set, use hardcoded default
  if (defaultsConfig.buzzerDefaultHolder[0] == '\0') {
    strlcpy(defaultsConfig.buzzerDefaultHolder, buzzerDefault, sizeof(defaultsConfig.buzzerDefaultHolder));
    saveAtStart = true;
  } else {
    strlcpy(buzzerDefault, defaultsConfig.buzzerDefaultHolder, sizeof(buzzerDefault));
  }
  
  // If no scroll delay default set, use hardcoded default
  if (defaultsConfig.scrollDelayDefaultHolder[0] == '\0') {
    strlcpy(defaultsConfig.scrollDelayDefaultHolder, scrollDelayDefault, sizeof(defaultsConfig.scrollDelayDefaultHolder));
    saveAtStart = true;
  } else {
    strlcpy(scrollDelayDefault, defaultsConfig.scrollDelayDefaultHolder, sizeof(scrollDelayDefault));
  }
  
  // If no brightness default set, use hardcoded default
  if (defaultsConfig.brightnessDefaultHolder[0] == '\0') {
    strlcpy(defaultsConfig.brightnessDefaultHolder, ledBrightnessDefault, sizeof(defaultsConfig.brightnessDefaultHolder));
    saveAtStart = true;
  } else {
    strlcpy(ledBrightnessDefault, defaultsConfig.brightnessDefaultHolder, sizeof(ledBrightnessDefault));
  }

  // If no alert chirp default set, use hardcoded default
  if (defaultsConfig.alertChirpDefaultHolder[0] == '\0') {
    strlcpy(defaultsConfig.alertChirpDefaultHolder, alertChirpDefault, sizeof(defaultsConfig.alertChirpDefaultHolder));
    saveAtStart = true;
  } else {
    strlcpy(alertChirpDefault, defaultsConfig.alertChirpDefaultHolder, sizeof(alertChirpDefault));
  }

  if (saveAtStart) {
    Serial.println(F("Saving initial defaults configuration..."));
    saveDefaultsConfiguration(defaultsConfigFile, defaultsConfig);
  }

  PRINT("Defaults loaded - Repeat: ", repeatDefault);
  PRINT(", Buzzer: ", buzzerDefault);
  PRINT(", Delay: ", scrollDelayDefault);
  PRINT(", Brightness: ", ledBrightnessDefault);
  PRINT(", Alert Chirp: ", alertChirpDefault);
  PRINTS("\n");
}

// ################################ GENERAL CONFIG FUNCTIONS ################################//

void loadGeneralConfiguration(const char *configFile, generalConfigObj &config) {
  File file = LittleFS.open(configFile, "r");
  if (!file) {
    Serial.println("Failed to open general config file");
    return;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file);
  if (error)
    Serial.println(F("Failed to read general file, using defaults"));

  if (!doc["buzzerEnable"].isNull())
    strlcpy(config.buzzerEnable, doc["buzzerEnable"], sizeof(config.buzzerEnable));
  if (!doc["brightnessOverrideEnable"].isNull())
    strlcpy(config.brightnessOverrideEnable, doc["brightnessOverrideEnable"], sizeof(config.brightnessOverrideEnable));
  if (!doc["brightnessOverrideValue"].isNull())
    strlcpy(config.brightnessOverrideValue, doc["brightnessOverrideValue"], sizeof(config.brightnessOverrideValue));
    
  file.close();
}

void saveGeneralConfiguration(const char *configFile, const generalConfigObj &config) {
  File file = LittleFS.open(configFile, "w");
  if (!file) {
    Serial.println("Failed to open general config file for writing");
    return;
  }

  JsonDocument doc;
  doc["buzzerEnable"] = config.buzzerEnable;
  doc["brightnessOverrideEnable"] = config.brightnessOverrideEnable;
  doc["brightnessOverrideValue"] = config.brightnessOverrideValue;

  if (serializeJson(doc, file) == 0) {
    Serial.println(F("Failed to write general file"));
  }

  file.close();
}

void initGeneralStoreConfig() {
  Serial.println(F("Loading general configuration...\n"));
  loadGeneralConfiguration(generalConfigFile, generalConfig);
  
  bool saveAtStart = false;
  
  // Custom Global Buzzer Enable logic
  if (generalConfig.buzzerEnable[0] == '\0') {
    strlcpy(generalConfig.buzzerEnable, "on", sizeof(generalConfig.buzzerEnable));
    saveAtStart = true;
  }
  
  // Brightness Override Enable logic
  if (generalConfig.brightnessOverrideEnable[0] == '\0') {
    strlcpy(generalConfig.brightnessOverrideEnable, "off", sizeof(generalConfig.brightnessOverrideEnable));
    saveAtStart = true;
  }
  
  // Brightness Override Value logic
  if (generalConfig.brightnessOverrideValue[0] == '\0') {
    strlcpy(generalConfig.brightnessOverrideValue, "8", sizeof(generalConfig.brightnessOverrideValue));
    saveAtStart = true;
  }
  
  if (saveAtStart) {
    Serial.println(F("Saving initial general configuration..."));
    saveGeneralConfiguration(generalConfigFile, generalConfig);
  }
  
  Serial.print(F("General settings loaded - Buzzer Enable: "));
  Serial.print(generalConfig.buzzerEnable);
  Serial.print(F(", Brightness Override: "));
  Serial.print(generalConfig.brightnessOverrideEnable);
  Serial.print(F(", Override Value: "));
  Serial.println(generalConfig.brightnessOverrideValue);
}

// ============================================================================
// CLOCK CONFIGURATION FUNCTIONS
// ============================================================================

// Load clock configuration from file
void loadClockConfiguration(const char *configFile, clockConfigObj &config) {
  File file = LittleFS.open(configFile, "r");
  if (!file) {
    Serial.println("Clock config file not found, using defaults");
    return;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    Serial.println(F("Failed to read clock config file, using defaults"));
    file.close();
    return;
  }

  if (!doc["enabled"].isNull())
    strlcpy(config.enabled, doc["enabled"], sizeof(config.enabled));
  if (!doc["ntpServer"].isNull())
    strlcpy(config.ntpServer, doc["ntpServer"], sizeof(config.ntpServer));
  if (!doc["tzString"].isNull())
    strlcpy(config.tzString, doc["tzString"], sizeof(config.tzString));
  if (!doc["brightness"].isNull())
    strlcpy(config.brightness, doc["brightness"], sizeof(config.brightness));
  if (!doc["transitionDelayMs"].isNull())
    strlcpy(config.transitionDelayMs, doc["transitionDelayMs"], sizeof(config.transitionDelayMs));
  if (!doc["transitionEffect"].isNull())
    strlcpy(config.transitionEffect, doc["transitionEffect"], sizeof(config.transitionEffect));
  if (!doc["randomizeTransition"].isNull())
    strlcpy(config.randomizeTransition, doc["randomizeTransition"], sizeof(config.randomizeTransition));
  if (!doc["resyncIntervalHours"].isNull())
    strlcpy(config.resyncIntervalHours, doc["resyncIntervalHours"], sizeof(config.resyncIntervalHours));
  if (!doc["transitionSpeed"].isNull())
    strlcpy(config.transitionSpeed, doc["transitionSpeed"], sizeof(config.transitionSpeed));
  if (!doc["dateFormat"].isNull())
    strlcpy(config.dateFormat, doc["dateFormat"], sizeof(config.dateFormat));
  if (!doc["dateAlternate"].isNull())
    strlcpy(config.dateAlternate, doc["dateAlternate"], sizeof(config.dateAlternate));
  if (!doc["dateAlternateSeconds"].isNull())
    strlcpy(config.dateAlternateSeconds, doc["dateAlternateSeconds"], sizeof(config.dateAlternateSeconds));
  if (!doc["customDateFormat"].isNull())
    strlcpy(config.customDateFormat, doc["customDateFormat"], sizeof(config.customDateFormat));
  if (!doc["clockFace"].isNull())
    strlcpy(config.clockFace, doc["clockFace"], sizeof(config.clockFace));
  if (!doc["clockAmPm"].isNull())
    strlcpy(config.clockAmPm, doc["clockAmPm"], sizeof(config.clockAmPm));

  // Migration: TIME_ALTERNATE format → TIME_ONLY + dateAlternate=on
  if (strcmp(config.dateFormat, "TIME_ALTERNATE") == 0) {
    strlcpy(config.dateFormat, "TIME_ONLY", sizeof(config.dateFormat));
    strlcpy(config.dateAlternate, "on", sizeof(config.dateAlternate));
  }

  file.close();
}

// Save clock configuration to file
void saveClockConfiguration(const char *configFile, const clockConfigObj &config) {
  File file = LittleFS.open(configFile, "w");
  if (!file) {
    Serial.println("Failed to open clock config file for writing");
    return;
  }

  JsonDocument doc;
  doc["enabled"] = config.enabled;
  doc["ntpServer"] = config.ntpServer;
  doc["tzString"] = config.tzString;
  doc["brightness"] = config.brightness;
  doc["transitionDelayMs"] = config.transitionDelayMs;
  doc["transitionEffect"] = config.transitionEffect;
  doc["randomizeTransition"] = config.randomizeTransition;
  doc["resyncIntervalHours"] = config.resyncIntervalHours;
  doc["transitionSpeed"] = config.transitionSpeed;
  doc["dateFormat"] = config.dateFormat;
  doc["dateAlternate"] = config.dateAlternate;
  doc["dateAlternateSeconds"] = config.dateAlternateSeconds;
  doc["customDateFormat"] = config.customDateFormat;
  doc["clockFace"] = config.clockFace;
  doc["clockAmPm"] = config.clockAmPm;

  if (serializeJson(doc, file) == 0) {
    Serial.println(F("Failed to write clock config file"));
  }

  file.close();
}

// Initialize clock configuration from flash or use defaults
void initClockStoreConfig() {
  Serial.println(F("Loading clock configuration...\n"));
  loadClockConfiguration(clockConfigFile, clockConfig);

  bool saveAtStart = false;

  // Set defaults if not configured
  if (clockConfig.enabled[0] == '\0') {
    strlcpy(clockConfig.enabled, "off", sizeof(clockConfig.enabled));
    saveAtStart = true;
  }

  if (clockConfig.ntpServer[0] == '\0') {
    strlcpy(clockConfig.ntpServer, "pool.ntp.org", sizeof(clockConfig.ntpServer));
    saveAtStart = true;
  }

  if (clockConfig.tzString[0] == '\0') {
    strlcpy(clockConfig.tzString, DEFAULT_TZ_STRING, sizeof(clockConfig.tzString));
    saveAtStart = true;
  }

  if (clockConfig.dateFormat[0] == '\0') {
    #if MAX_DEVICES == 4
      strlcpy(clockConfig.dateFormat, "TIME_ONLY", sizeof(clockConfig.dateFormat));
    #elif MAX_DEVICES == 8
      strlcpy(clockConfig.dateFormat, "TIME_DATE", sizeof(clockConfig.dateFormat));
    #else
      strlcpy(clockConfig.dateFormat, "TIME_ONLY", sizeof(clockConfig.dateFormat));
    #endif
    saveAtStart = true;
  }

  if (clockConfig.dateAlternateSeconds[0] == '\0') {
    strlcpy(clockConfig.dateAlternateSeconds, "5", sizeof(clockConfig.dateAlternateSeconds));
    saveAtStart = true;
  }

  if (clockConfig.dateAlternate[0] == '\0') {
    strlcpy(clockConfig.dateAlternate, "off", sizeof(clockConfig.dateAlternate));
    saveAtStart = true;
  }

  if (clockConfig.clockFace[0] == '\0') {
    strlcpy(clockConfig.clockFace, "DEFAULT", sizeof(clockConfig.clockFace));
    saveAtStart = true;
  }

  if (clockConfig.clockAmPm[0] == '\0') {
    strlcpy(clockConfig.clockAmPm, "off", sizeof(clockConfig.clockAmPm));
    saveAtStart = true;
  }

  if (clockConfig.customDateFormat[0] == '\0') {
    strlcpy(clockConfig.customDateFormat, "%H:%M %b %e", sizeof(clockConfig.customDateFormat));
    saveAtStart = true;
  }

  if (clockConfig.brightness[0] == '\0') {
    strlcpy(clockConfig.brightness, "5", sizeof(clockConfig.brightness));
    saveAtStart = true;
  }

  if (clockConfig.transitionDelayMs[0] == '\0') {
    strlcpy(clockConfig.transitionDelayMs, "2000", sizeof(clockConfig.transitionDelayMs));
    saveAtStart = true;
  }

  if (clockConfig.transitionEffect[0] == '\0') {
    strlcpy(clockConfig.transitionEffect, "FADE", sizeof(clockConfig.transitionEffect));
    saveAtStart = true;
  }

  if (clockConfig.randomizeTransition[0] == '\0') {
    strlcpy(clockConfig.randomizeTransition, "off", sizeof(clockConfig.randomizeTransition));
    saveAtStart = true;
  }

  if (clockConfig.resyncIntervalHours[0] == '\0') {
    strlcpy(clockConfig.resyncIntervalHours, "6", sizeof(clockConfig.resyncIntervalHours));
    saveAtStart = true;
  }

  if (clockConfig.transitionSpeed[0] == '\0') {
    strlcpy(clockConfig.transitionSpeed, "40", sizeof(clockConfig.transitionSpeed));
    saveAtStart = true;
  }

  if (saveAtStart) {
    Serial.println(F("Saving initial clock configuration..."));
    saveClockConfiguration(clockConfigFile, clockConfig);
  }

  // Apply loaded configuration to global variables
  clockEnabled = (strcmp(clockConfig.enabled, "on") == 0);
  clockBrightness = atoi(clockConfig.brightness);

  PRINT("Clock loaded - Enabled: ", clockConfig.enabled);
  PRINT(", NTP Server: ", clockConfig.ntpServer);
  PRINT(", TZ: ", clockConfig.tzString);
  PRINT(", Brightness: ", clockConfig.brightness);
  PRINTS("\n");
}

#ifndef DISABLE_TIMER_FEATURE
// ============================================================================
// Timer Configuration
// ============================================================================

void loadTimerConfiguration(const char *configFile, timerConfigObj &config) {
  File file = LittleFS.open(configFile, "r");
  if (!file) {
    Serial.println(F("Failed to open timer config file for reading"));
    return;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    Serial.print(F("Failed to parse timer config: "));
    Serial.println(error.c_str());
    return;
  }

  // Load timer configuration
  strlcpy(config.enabled, doc["enabled"] | "off", STDSIZE);
  strlcpy(config.mode, doc["mode"] | "countdown", STDSIZE);
  strlcpy(config.durationSeconds, doc["durationSeconds"] | "300", STDSIZE);
  strlcpy(config.brightness, doc["brightness"] | "7", BRI_SIZE);
  strlcpy(config.alertBuzzer, doc["alertBuzzer"] | "on", STDSIZE);
  strlcpy(config.alertBuzzerCount, doc["alertBuzzerCount"] | "5", BUZ_SIZE);
  strlcpy(config.autoRepeat, doc["autoRepeat"] | "off", STDSIZE);
  strlcpy(config.alertChirp, doc["alertChirp"] | "Gentle Dawn", STDSIZE);

#if DEBUG == 1
  Serial.println(F("\n=== Timer Configuration Loaded ==="));
  Serial.print(F("Enabled: ")); Serial.println(config.enabled);
  Serial.print(F("Mode: ")); Serial.println(config.mode);
  Serial.print(F("Duration (seconds): ")); Serial.println(config.durationSeconds);
  Serial.print(F("Brightness: ")); Serial.println(config.brightness);
  Serial.print(F("Alert Buzzer: ")); Serial.println(config.alertBuzzer);
  Serial.print(F("Alert Buzzer Count: ")); Serial.println(config.alertBuzzerCount);
  Serial.print(F("Auto Repeat: ")); Serial.println(config.autoRepeat);
  Serial.print(F("Alert Chirp: ")); Serial.println(config.alertChirp);
  Serial.println(F("==================================\n"));
#endif
}

void saveTimerConfiguration(const char *configFile, const timerConfigObj &config) {
  JsonDocument doc;

  // Save timer configuration
  doc["enabled"] = config.enabled;
  doc["mode"] = config.mode;
  doc["durationSeconds"] = config.durationSeconds;
  doc["brightness"] = config.brightness;
  doc["alertBuzzer"] = config.alertBuzzer;
  doc["alertBuzzerCount"] = config.alertBuzzerCount;
  doc["autoRepeat"] = config.autoRepeat;
  doc["alertChirp"] = config.alertChirp;

  File file = LittleFS.open(configFile, "w");
  if (!file) {
    Serial.println(F("Failed to open timer config file for writing"));
    return;
  }

  if (serializeJson(doc, file) == 0) {
    Serial.println(F("Failed to write timer config"));
  }

  file.close();

#if DEBUG == 1
  Serial.println(F("\n=== Timer Configuration Saved ==="));
  Serial.print(F("Enabled: ")); Serial.println(config.enabled);
  Serial.print(F("Mode: ")); Serial.println(config.mode);
  Serial.print(F("Duration (seconds): ")); Serial.println(config.durationSeconds);
  Serial.print(F("Brightness: ")); Serial.println(config.brightness);
  Serial.print(F("Alert Buzzer: ")); Serial.println(config.alertBuzzer);
  Serial.print(F("Alert Buzzer Count: ")); Serial.println(config.alertBuzzerCount);
  Serial.print(F("Auto Repeat: ")); Serial.println(config.autoRepeat);
  Serial.print(F("Alert Chirp: ")); Serial.println(config.alertChirp);
  Serial.println(F("==================================\n"));
#endif
}

void initTimerStoreConfig() {
  const char *configFile = "/timer_config.json";

  // Check if config file exists
  if (!LittleFS.exists(configFile)) {
    Serial.println(F("Timer config file not found, creating defaults"));

    // Set default values
    strlcpy(timerConfig.enabled, "off", STDSIZE);
    strlcpy(timerConfig.mode, "countdown", STDSIZE);
    strlcpy(timerConfig.durationSeconds, "300", STDSIZE);  // 5 minutes default
    strlcpy(timerConfig.brightness, "7", BRI_SIZE);
    strlcpy(timerConfig.alertBuzzer, "on", STDSIZE);
    strlcpy(timerConfig.alertBuzzerCount, "5", BUZ_SIZE);
    strlcpy(timerConfig.autoRepeat, "off", STDSIZE);
    strlcpy(timerConfig.alertChirp, "Gentle Dawn", STDSIZE);

    // Save defaults to file
    saveTimerConfiguration(configFile, timerConfig);
  } else {
    // Load existing configuration
    loadTimerConfiguration(configFile, timerConfig);
  }

  // Apply configuration to runtime variables
  timerEnabled = (strcmp(timerConfig.enabled, "on") == 0);
  timerMode = (strcmp(timerConfig.mode, "stopwatch") == 0) ? TIMER_MODE_STOPWATCH : TIMER_MODE_COUNTDOWN;
  timerDurationSeconds = atoi(timerConfig.durationSeconds);
  timerTargetSeconds = timerDurationSeconds;
  timerBrightness = atoi(timerConfig.brightness);
  timerAlertBuzzer = (strcmp(timerConfig.alertBuzzer, "on") == 0);
  timerAlertBuzzerCount = atoi(timerConfig.alertBuzzerCount);
  timerAutoRepeat = (strcmp(timerConfig.autoRepeat, "on") == 0);

  PRINT("Alert Chirp: ", timerConfig.alertChirp);
  PRINTS("\n");
}
#endif // DISABLE_TIMER_FEATURE

#ifndef DISABLE_SLEEP_MODE_FEATURE
// ============================================================================
// Sleep Mode Configuration
// ============================================================================

void loadSleepModeConfiguration(const char *configFile, sleepModeConfigObj &config) {
  File file = LittleFS.open(configFile, "r");
  if (!file) {
    Serial.println(F("Failed to open sleep mode config file for reading"));
    return;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    Serial.print(F("Failed to parse sleep mode config: "));
    Serial.println(error.c_str());
    return;
  }

  // Load sleep mode configuration with defaults
  strlcpy(config.enabled, doc["enabled"] | "off", STDSIZE);
  strlcpy(config.onTime, doc["onTime"] | "22:00", STDSIZE);
  strlcpy(config.offTime, doc["offTime"] | "06:00", STDSIZE);
  strlcpy(config.muteOnly, doc["muteOnly"] | "off", STDSIZE);
  strlcpy(config.weekendEnabled, doc["weekendEnabled"] | "off", STDSIZE);
  strlcpy(config.weekendOnTime, doc["weekendOnTime"] | "22:00", STDSIZE);
  strlcpy(config.weekendOffTime, doc["weekendOffTime"] | "08:00", STDSIZE);

#if DEBUG == 1
  Serial.println(F("\n=== Sleep Mode Configuration Loaded ==="));
  Serial.print(F("Enabled: ")); Serial.println(config.enabled);
  Serial.print(F("On Time: ")); Serial.println(config.onTime);
  Serial.print(F("Off Time: ")); Serial.println(config.offTime);
  Serial.println(F("========================================\n"));
#endif
}

void saveSleepModeConfiguration(const char *configFile, const sleepModeConfigObj &config) {
  JsonDocument doc;

  // Save sleep mode configuration
  doc["enabled"] = config.enabled;
  doc["onTime"] = config.onTime;
  doc["offTime"] = config.offTime;
  doc["muteOnly"] = config.muteOnly;
  doc["weekendEnabled"] = config.weekendEnabled;
  doc["weekendOnTime"] = config.weekendOnTime;
  doc["weekendOffTime"] = config.weekendOffTime;

  File file = LittleFS.open(configFile, "w");
  if (!file) {
    Serial.println(F("Failed to open sleep mode config file for writing"));
    return;
  }

  if (serializeJson(doc, file) == 0) {
    Serial.println(F("Failed to write sleep mode config"));
  }

  file.close();

#if DEBUG == 1
  Serial.println(F("\n=== Sleep Mode Configuration Saved ==="));
  Serial.print(F("Enabled: ")); Serial.println(config.enabled);
  Serial.print(F("On Time: ")); Serial.println(config.onTime);
  Serial.print(F("Off Time: ")); Serial.println(config.offTime);
  Serial.println(F("=======================================\n"));
#endif
}

void initSleepModeStoreConfig() {
  const char *configFile = "/sleep_mode.config";

  // Check if config file exists
  if (!LittleFS.exists(configFile)) {
    Serial.println(F("Sleep mode config file not found, creating defaults"));

    // Set default values
    strlcpy(sleepModeConfig.enabled, "off", STDSIZE);
    strlcpy(sleepModeConfig.onTime, "22:00", STDSIZE);
    strlcpy(sleepModeConfig.offTime, "06:00", STDSIZE);
    strlcpy(sleepModeConfig.muteOnly, "off", STDSIZE);
    strlcpy(sleepModeConfig.weekendEnabled, "off", STDSIZE);
    strlcpy(sleepModeConfig.weekendOnTime, "22:00", STDSIZE);
    strlcpy(sleepModeConfig.weekendOffTime, "08:00", STDSIZE);

    // Save defaults to file
    saveSleepModeConfiguration(configFile, sleepModeConfig);
  } else {
    // Load existing configuration
    loadSleepModeConfiguration(configFile, sleepModeConfig);
  }

  // Apply configuration to runtime variables
  sleepModeEnabled = (strcmp(sleepModeConfig.enabled, "on") == 0);
  sleepModeMuteOnly = (strcmp(sleepModeConfig.muteOnly, "on") == 0);

  Serial.print(F("Sleep Mode loaded - Enabled: "));
  Serial.print(sleepModeConfig.enabled);
  Serial.print(F(", On Time: "));
  Serial.print(sleepModeConfig.onTime);
  Serial.print(F(", Off Time: "));
  Serial.print(sleepModeConfig.offTime);
  Serial.print(F(", Mute Only: "));
  Serial.print(sleepModeConfig.muteOnly);
  Serial.print(F(", Weekend Enabled: "));
  Serial.print(sleepModeConfig.weekendEnabled);
  Serial.print(F(", Weekend On: "));
  Serial.print(sleepModeConfig.weekendOnTime);
  Serial.print(F(", Weekend Off: "));
  Serial.println(sleepModeConfig.weekendOffTime);
}
#endif // DISABLE_SLEEP_MODE_FEATURE

#ifndef DISABLE_ALARM_FEATURE
// ============================================================================
// Alarm Configuration
// ============================================================================

void loadAlarmConfiguration(const char *configFile, alarmConfigObj &config) {
  File file = LittleFS.open(configFile, "r");
  if (!file) {
    Serial.println(F("Failed to open alarm config file for reading"));
    return;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    Serial.print(F("Failed to parse alarm config: "));
    Serial.println(error.c_str());
    return;
  }

  // Load master enable
  strlcpy(config.masterEnabled, doc["masterEnabled"] | "off", STDSIZE);

  // Load individual alarms
  JsonArray alarmsArray = doc["alarms"].as<JsonArray>();
  for (int i = 0; i < MAX_ALARMS; i++) {
    if (i < (int)alarmsArray.size()) {
      JsonObject alarmObj = alarmsArray[i].as<JsonObject>();
      strlcpy(config.alarms[i].enabled, alarmObj["enabled"] | "off", STDSIZE);
      strlcpy(config.alarms[i].time, alarmObj["time"] | "07:00", STDSIZE);
      strlcpy(config.alarms[i].daysOfWeek, alarmObj["daysOfWeek"] | "", 8);
      strlcpy(config.alarms[i].message, alarmObj["message"] | "Alarm!", ALARM_MESSAGE_SIZE);
      strlcpy(config.alarms[i].chirpName, alarmObj["chirpName"] | "Gentle Dawn", STDSIZE);
      strlcpy(config.alarms[i].repeatCount, alarmObj["repeatCount"] | "3", STDSIZE);
    } else {
      // Initialize unset alarms with defaults
      strlcpy(config.alarms[i].enabled, "off", STDSIZE);
      strlcpy(config.alarms[i].time, "07:00", STDSIZE);
      strlcpy(config.alarms[i].daysOfWeek, "", 8);
      strlcpy(config.alarms[i].message, "Alarm!", ALARM_MESSAGE_SIZE);
      strlcpy(config.alarms[i].chirpName, "Gentle Dawn", STDSIZE);
      strlcpy(config.alarms[i].repeatCount, "3", STDSIZE);
    }
  }

#if DEBUG == 1
  Serial.println(F("\n=== Alarm Configuration Loaded ==="));
  Serial.print(F("Master Enabled: ")); Serial.println(config.masterEnabled);
  for (int i = 0; i < MAX_ALARMS; i++) {
    if (strcmp(config.alarms[i].enabled, "on") == 0) {
      Serial.print(F("Alarm ")); Serial.print(i + 1);
      Serial.print(F(": ")); Serial.print(config.alarms[i].time);
      Serial.print(F(" - ")); Serial.println(config.alarms[i].message);
    }
  }
  Serial.println(F("==================================\n"));
#endif
}

void saveAlarmConfiguration(const char *configFile, const alarmConfigObj &config) {
  JsonDocument doc;

  // Save master enable
  doc["masterEnabled"] = config.masterEnabled;

  // Save individual alarms as array
  JsonArray alarmsArray = doc["alarms"].to<JsonArray>();
  for (int i = 0; i < MAX_ALARMS; i++) {
    JsonObject alarmObj = alarmsArray.add<JsonObject>();
    alarmObj["enabled"] = config.alarms[i].enabled;
    alarmObj["time"] = config.alarms[i].time;
    alarmObj["daysOfWeek"] = config.alarms[i].daysOfWeek;
    alarmObj["message"] = config.alarms[i].message;
    alarmObj["chirpName"] = config.alarms[i].chirpName;
    alarmObj["repeatCount"] = config.alarms[i].repeatCount;
  }

  File file = LittleFS.open(configFile, "w");
  if (!file) {
    Serial.println(F("Failed to open alarm config file for writing"));
    return;
  }

  if (serializeJson(doc, file) == 0) {
    Serial.println(F("Failed to write alarm config"));
  }

  file.close();

#if DEBUG == 1
  Serial.println(F("\n=== Alarm Configuration Saved ==="));
  Serial.print(F("Master Enabled: ")); Serial.println(config.masterEnabled);
  Serial.println(F("=================================\n"));
#endif
}

void initAlarmStoreConfig() {
  const char *configFile = "/alarm_config.json";

  // Check if config file exists
  if (!LittleFS.exists(configFile)) {
    Serial.println(F("Alarm config file not found, creating defaults"));

    // Set default values
    strlcpy(alarmConfig.masterEnabled, "off", STDSIZE);

    // Initialize all alarms with defaults
    for (int i = 0; i < MAX_ALARMS; i++) {
      strlcpy(alarmConfig.alarms[i].enabled, "off", STDSIZE);
      strlcpy(alarmConfig.alarms[i].time, "07:00", STDSIZE);
      strlcpy(alarmConfig.alarms[i].daysOfWeek, "", 8);
      strlcpy(alarmConfig.alarms[i].message, "Alarm!", ALARM_MESSAGE_SIZE);
      strlcpy(alarmConfig.alarms[i].chirpName, "Gentle Dawn", STDSIZE);
      strlcpy(alarmConfig.alarms[i].repeatCount, "3", STDSIZE);
    }

    // Save defaults to file
    saveAlarmConfiguration(configFile, alarmConfig);
  } else {
    // Load existing configuration
    loadAlarmConfiguration(configFile, alarmConfig);
  }

  // Apply configuration to runtime variables
  alarmMasterEnabled = (strcmp(alarmConfig.masterEnabled, "on") == 0);

  Serial.print(F("Alarm loaded - Master Enabled: "));
  Serial.println(alarmConfig.masterEnabled);

  // Count enabled alarms
  int enabledCount = 0;
  for (int i = 0; i < MAX_ALARMS; i++) {
    if (strcmp(alarmConfig.alarms[i].enabled, "on") == 0) {
      enabledCount++;
    }
  }
  Serial.print(F("Enabled alarms: "));
  Serial.println(enabledCount);
}

// ============================================================================
// Recurrent Alarm Configuration
// ============================================================================

void loadRecurrentAlarmConfiguration(const char *configFile, recurrentAlarmConfigObj &config) {
  File file = LittleFS.open(configFile, "r");
  if (!file) {
    Serial.println(F("Failed to open recurrent alarm config file for reading"));
    return;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    Serial.print(F("Failed to parse recurrent alarm config: "));
    Serial.println(error.c_str());
    return;
  }

  // Load recurrent alarm configuration with defaults
  strlcpy(config.enabled, doc["enabled"] | "off", STDSIZE);
  strlcpy(config.chirpName, doc["chirpName"] | "Gentle Dawn", STDSIZE);
  strlcpy(config.interval, doc["interval"] | "15min", STDSIZE);
  strlcpy(config.disableWeekends, doc["disableWeekends"] | "off", STDSIZE);

#if DEBUG == 1
  Serial.println(F("\n=== Recurrent Alarm Configuration Loaded ==="));
  Serial.print(F("Enabled: ")); Serial.println(config.enabled);
  Serial.print(F("Chirp Name: ")); Serial.println(config.chirpName);
  Serial.print(F("Interval: ")); Serial.println(config.interval);
  Serial.print(F("Disable Weekends: ")); Serial.println(config.disableWeekends);
  Serial.println(F("============================================\n"));
#endif
}

void saveRecurrentAlarmConfiguration(const char *configFile, const recurrentAlarmConfigObj &config) {
  JsonDocument doc;

  // Save recurrent alarm configuration
  doc["enabled"] = config.enabled;
  doc["chirpName"] = config.chirpName;
  doc["interval"] = config.interval;
  doc["disableWeekends"] = config.disableWeekends;

  File file = LittleFS.open(configFile, "w");
  if (!file) {
    Serial.println(F("Failed to open recurrent alarm config file for writing"));
    return;
  }

  if (serializeJson(doc, file) == 0) {
    Serial.println(F("Failed to write recurrent alarm config"));
  }

  file.close();

#if DEBUG == 1
  Serial.println(F("\n=== Recurrent Alarm Configuration Saved ==="));
  Serial.print(F("Enabled: ")); Serial.println(config.enabled);
  Serial.print(F("Chirp Name: ")); Serial.println(config.chirpName);
  Serial.print(F("Interval: ")); Serial.println(config.interval);
  Serial.print(F("Disable Weekends: ")); Serial.println(config.disableWeekends);
  Serial.println(F("===========================================\n"));
#endif
}

void initRecurrentAlarmStoreConfig() {
  const char *configFile = "/recurrent_alarm_config.json";

  // Check if config file exists
  if (!LittleFS.exists(configFile)) {
    Serial.println(F("Recurrent alarm config file not found, creating defaults"));

    // Set default values
    strlcpy(recurrentAlarmConfig.enabled, "off", STDSIZE);
    strlcpy(recurrentAlarmConfig.chirpName, "Gentle Dawn", STDSIZE);
    strlcpy(recurrentAlarmConfig.interval, "15min", STDSIZE);
    strlcpy(recurrentAlarmConfig.disableWeekends, "off", STDSIZE);

    // Save defaults to file
    saveRecurrentAlarmConfiguration(configFile, recurrentAlarmConfig);
  } else {
    // Load existing configuration
    loadRecurrentAlarmConfiguration(configFile, recurrentAlarmConfig);
  }

  // Apply configuration to runtime variables
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

  Serial.print(F("Recurrent Alarm loaded - Enabled: "));
  Serial.print(recurrentAlarmConfig.enabled);
  Serial.print(F(", Interval: "));
  Serial.print(recurrentAlarmConfig.interval);
  Serial.print(F(" ("));
  Serial.print(recurrentAlarmIntervalMinutes);
  Serial.println(F(" min)"));
  Serial.print(F("Disable Weekends: "));
  Serial.println(recurrentAlarmConfig.disableWeekends);
}
#endif // DISABLE_ALARM_FEATURE

#ifndef DISABLE_WEATHER_FEATURE
// ============================================================================
// Weather Configuration
// ============================================================================

void loadWeatherConfiguration(const char *configFile, weatherConfigObj &config) {
  File file = LittleFS.open(configFile, "r");
  if (!file) {
    Serial.println(F("Failed to open weather config file for reading"));
    return;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    Serial.print(F("Failed to parse weather config: "));
    Serial.println(error.c_str());
    return;
  }

  // Load weather configuration with defaults
  strlcpy(config.enabled, doc["enabled"] | "off", STDSIZE);
  strlcpy(config.location, doc["location"] | "", WEATHER_LOCATION_SIZE);
  strlcpy(config.latitude, doc["latitude"] | "", WEATHER_COORD_SIZE);
  strlcpy(config.longitude, doc["longitude"] | "", WEATHER_COORD_SIZE);
  strlcpy(config.updateIntervalMinutes, doc["updateIntervalMinutes"] | "30", REP_SIZE);
  strlcpy(config.displayIntervalMinutes, doc["displayIntervalMinutes"] | "5", REP_SIZE);
  strlcpy(config.temperatureUnit, doc["temperatureUnit"] | "C", STDSIZE);
  strlcpy(config.brightness, doc["brightness"] | "5", BRI_SIZE);
  strlcpy(config.displayRepeatCount, doc["displayRepeatCount"] | "2", REP_SIZE);

#if DEBUG == 1
  Serial.println(F("\n=== Weather Configuration Loaded ==="));
  Serial.print(F("Enabled: ")); Serial.println(config.enabled);
  Serial.print(F("Location: ")); Serial.println(config.location);
  Serial.print(F("Latitude: ")); Serial.println(config.latitude);
  Serial.print(F("Longitude: ")); Serial.println(config.longitude);
  Serial.print(F("Update Interval: ")); Serial.println(config.updateIntervalMinutes);
  Serial.print(F("Temperature Unit: ")); Serial.println(config.temperatureUnit);
  Serial.print(F("Brightness: ")); Serial.println(config.brightness);
  Serial.print(F("Display Repeat Count: ")); Serial.println(config.displayRepeatCount);
  Serial.println(F("====================================\n"));
#endif
}

void saveWeatherConfiguration(const char *configFile, const weatherConfigObj &config) {
  JsonDocument doc;

  // Save weather configuration
  doc["enabled"] = config.enabled;
  doc["location"] = config.location;
  doc["latitude"] = config.latitude;
  doc["longitude"] = config.longitude;
  doc["updateIntervalMinutes"] = config.updateIntervalMinutes;
  doc["displayIntervalMinutes"] = config.displayIntervalMinutes;
  doc["temperatureUnit"] = config.temperatureUnit;
  doc["brightness"] = config.brightness;
  doc["displayRepeatCount"] = config.displayRepeatCount;

  File file = LittleFS.open(configFile, "w");
  if (!file) {
    Serial.println(F("Failed to open weather config file for writing"));
    return;
  }

  if (serializeJson(doc, file) == 0) {
    Serial.println(F("Failed to write weather config"));
  }

  file.close();

#if DEBUG == 1
  Serial.println(F("\n=== Weather Configuration Saved ==="));
  Serial.print(F("Enabled: ")); Serial.println(config.enabled);
  Serial.print(F("Location: ")); Serial.println(config.location);
  Serial.print(F("Latitude: ")); Serial.println(config.latitude);
  Serial.print(F("Longitude: ")); Serial.println(config.longitude);
  Serial.println(F("===================================\n"));
#endif
}

void initWeatherStoreConfig() {
  const char *configFile = "/weather_config.json";

  // Check if config file exists
  if (!LittleFS.exists(configFile)) {
    Serial.println(F("Weather config file not found, creating defaults"));

    // Set default values
    strlcpy(weatherConfig.enabled, "off", STDSIZE);
    strlcpy(weatherConfig.location, "", WEATHER_LOCATION_SIZE);
    strlcpy(weatherConfig.latitude, "", WEATHER_COORD_SIZE);
    strlcpy(weatherConfig.longitude, "", WEATHER_COORD_SIZE);
    strlcpy(weatherConfig.updateIntervalMinutes, "30", REP_SIZE);
    strlcpy(weatherConfig.displayIntervalMinutes, "5", REP_SIZE);
    strlcpy(weatherConfig.temperatureUnit, "C", STDSIZE);
    strlcpy(weatherConfig.brightness, "5", BRI_SIZE);
    strlcpy(weatherConfig.displayRepeatCount, "2", REP_SIZE);

    // Save defaults to file
    saveWeatherConfiguration(configFile, weatherConfig);
  } else {
    // Load existing configuration
    loadWeatherConfiguration(configFile, weatherConfig);
  }

  // Apply configuration to runtime variables
  weatherEnabled = (strcmp(weatherConfig.enabled, "on") == 0);
  weatherBrightness = atoi(weatherConfig.brightness);

  Serial.print(F("Weather loaded - Enabled: "));
  Serial.print(weatherConfig.enabled);
  Serial.print(F(", Location: "));
  Serial.print(weatherConfig.location);
  Serial.print(F(", Coords: "));
  Serial.print(weatherConfig.latitude);
  Serial.print(F(", "));
  Serial.println(weatherConfig.longitude);
}
#endif // DISABLE_WEATHER_FEATURE

#ifndef DISABLE_CRYPTO_FEATURE
// ============================================================================
// Crypto Price Ticker Configuration
// ============================================================================

void loadCryptoConfiguration(const char *configFile, cryptoConfigObj &config) {
  File file = LittleFS.open(configFile, "r");
  if (!file) {
    Serial.println(F("Failed to open crypto config file for reading"));
    return;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    Serial.print(F("Failed to parse crypto config: "));
    Serial.println(error.c_str());
    return;
  }

  strlcpy(config.enabled, doc["enabled"] | "off", FLAG_SIZE);
  strlcpy(config.apiKey, doc["apiKey"] | "", CRYPTO_API_KEY_SIZE);
  strlcpy(config.coins, doc["coins"] | "btc-bitcoin,eth-ethereum", CRYPTO_COINS_SIZE);
  strlcpy(config.currency, doc["currency"] | "USD", CRYPTO_CURRENCY_SIZE);
  strlcpy(config.updateIntervalMinutes, doc["updateIntervalMinutes"] | "30", REP_SIZE);
  strlcpy(config.displayIntervalMinutes, doc["displayIntervalMinutes"] | "5", REP_SIZE);
  strlcpy(config.displayRepeatCount, doc["displayRepeatCount"] | "2", REP_SIZE);
  strlcpy(config.brightness, doc["brightness"] | "5", BRI_SIZE);

#if DEBUG == 1
  Serial.println(F("\n=== Crypto Configuration Loaded ==="));
  Serial.print(F("Enabled: ")); Serial.println(config.enabled);
  Serial.print(F("Coins: ")); Serial.println(config.coins);
  Serial.print(F("Currency: ")); Serial.println(config.currency);
  Serial.print(F("Update Interval: ")); Serial.println(config.updateIntervalMinutes);
  Serial.print(F("Brightness: ")); Serial.println(config.brightness);
  Serial.println(F("====================================\n"));
#endif
}

void saveCryptoConfiguration(const char *configFile, const cryptoConfigObj &config) {
  JsonDocument doc;

  doc["enabled"] = config.enabled;
  doc["apiKey"] = config.apiKey;
  doc["coins"] = config.coins;
  doc["currency"] = config.currency;
  doc["updateIntervalMinutes"] = config.updateIntervalMinutes;
  doc["displayIntervalMinutes"] = config.displayIntervalMinutes;
  doc["displayRepeatCount"] = config.displayRepeatCount;
  doc["brightness"] = config.brightness;

  File file = LittleFS.open(configFile, "w");
  if (!file) {
    Serial.println(F("Failed to open crypto config file for writing"));
    return;
  }

  if (serializeJson(doc, file) == 0) {
    Serial.println(F("Failed to write crypto config"));
  }

  file.close();

#if DEBUG == 1
  Serial.println(F("\n=== Crypto Configuration Saved ==="));
  Serial.print(F("Enabled: ")); Serial.println(config.enabled);
  Serial.print(F("Coins: ")); Serial.println(config.coins);
  Serial.println(F("===================================\n"));
#endif
}

void initCryptoStoreConfig() {
  const char *configFile = "/crypto_config.json";

  if (!LittleFS.exists(configFile)) {
    Serial.println(F("Crypto config file not found, creating defaults"));

    strlcpy(cryptoConfig.enabled, "off", FLAG_SIZE);
    strlcpy(cryptoConfig.apiKey, "", CRYPTO_API_KEY_SIZE);
    // Default: Bitcoin and Ethereum only — add more via the web UI (up to 10 total)
    strlcpy(cryptoConfig.coins, "btc-bitcoin,eth-ethereum", CRYPTO_COINS_SIZE);
    strlcpy(cryptoConfig.currency, "USD", CRYPTO_CURRENCY_SIZE);
    strlcpy(cryptoConfig.updateIntervalMinutes, "30", REP_SIZE);
    strlcpy(cryptoConfig.displayIntervalMinutes, "5", REP_SIZE);
    strlcpy(cryptoConfig.displayRepeatCount, "2", REP_SIZE);
    strlcpy(cryptoConfig.brightness, "5", BRI_SIZE);

    saveCryptoConfiguration(configFile, cryptoConfig);
  } else {
    loadCryptoConfiguration(configFile, cryptoConfig);
  }

  // Apply configuration to runtime variables
  cryptoEnabled = (strcmp(cryptoConfig.enabled, "on") == 0);
  cryptoBrightness = atoi(cryptoConfig.brightness);

  Serial.print(F("Crypto loaded - Enabled: "));
  Serial.print(cryptoConfig.enabled);
  Serial.print(F(", Coins: "));
  Serial.print(cryptoConfig.coins);
  Serial.print(F(", Currency: "));
  Serial.println(cryptoConfig.currency);
}
#endif // DISABLE_CRYPTO_FEATURE
