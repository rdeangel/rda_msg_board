#include "functions.h"
#include "globals.h"
#include "web_pages.h"
#include "web_server.h"
#include "mqtt_discovery.h"
#include "config.h"
#include "mqtt.h"
#ifdef ESP32
#include "esp_sntp.h"
#endif
#ifndef DISABLE_TIMER_FEATURE
#include "timer.h"
#endif
#ifndef DISABLE_SLEEP_MODE_FEATURE
#include "mqtt_discovery_sleep.h"
#endif
#ifndef DISABLE_WEATHER_FEATURE
#include "weather.h"
#endif
#ifndef DISABLE_CRYPTO_FEATURE
#include "crypto.h"
#endif
#include "chirp_library.h"
#include "buzzer_utils.h"
// 6-series fonts (centred in 8-row display, compact height — good for 4-module clocks)
#include "MatrixLight6_font.h"
#include "MatrixLight6X_font.h"
#include "MatrixChunky6_font.h"
#include "MatrixChunky6X_font.h"
// 8-series fonts (full 8-row height — best for 8-module clocks)
#include "MatrixLight8_font.h"
#include "MatrixLight8X_font.h"
// Default font variant with '1' padded to fixed width — used only for clock/timer
#include "ClockFixedFont.h"

// Note: Buzzer and UTF-8 functions moved to buzzer_utils.cpp and utf8_utils.cpp

// Forward declaration — defined further down, used in displayText() and displayClock()
static MD_MAX72XX::fontType_t* getFontForFace(const char* face);

// Returns the effective brightness value, considering override settings
int getEffectiveBrightness(int requestedBrightness) {
  if (brightnessOverrideEnabled) {
    return brightnessOverrideValue;
  }
  return requestedBrightness;
}

//Message send based on http URL argument request
void onMessageCallHttp(void) {
  // Removed debug String building to prevent blocking - was causing crashes
  // when called during clock display transitions
  bool messageArg = false;
  bool forceRepArg = false;
  for (uint8_t i = 0; i < serverHttp.args(); i++) {
    if (serverHttp.argName(i) == "MSG") {
      repeatCount = 0;
      serverHttp.arg(i).toCharArray(newMessage, MSG_SIZE);
      newMessageAvailable = true;
      messageArg = true;
    }
    if (serverHttp.argName(i) == "REP") {
      serverHttp.arg(i).toCharArray(newRepeat, REP_SIZE);
      newRepeatAvailable = true;
    }
    if (serverHttp.argName(i) == "BUZ") {
      serverHttp.arg(i).toCharArray(newBuz, BUZ_SIZE);
      newBuzAvailable = true;
      explicitBuzzerCount = true;
    }
    if (serverHttp.argName(i) == "DEL") {
      serverHttp.arg(i).toCharArray(newDelay, DEL_SIZE);
      newDelayAvailable = true;
    }
    if (serverHttp.argName(i) == "BRI") {
      serverHttp.arg(i).toCharArray(newBrightness, DEL_SIZE);
      newBrightnessAvailable = true;
    }
    if (serverHttp.argName(i) == "ASC") {
      serverHttp.arg(i).toCharArray(newAsciiConv, ASC_SIZE);
      newAsciiConvAvailable = true;
    }
    if (serverHttp.argName(i) == "ALERTCHIRP") {
      serverHttp.arg(i).toCharArray(newAlertChirp, STDSIZE);
      newAlertChirpAvailable = true;
    }
    if (serverHttp.argName(i) == "FORCEREP") {
      String val = serverHttp.arg(i);
      val.toLowerCase();
      forceRepetitions = (val == "true" || val == "1" || val == "on");
      forceRepArg = true;
    }
  }
  if (!forceRepArg) {
    forceRepetitions = false;
  }
  if (!messageArg) {
    strcpy(newMessage, "");
    newMessageAvailable = true;
  }
  if (!newRepeatAvailable) {
    strcpy(newRepeat, repeatDefault);
    newRepeatAvailable = true;
  }
  if (!newBuzAvailable) {
    strcpy(newBuz, buzzerDefault);
    newBuzAvailable = true;
    explicitBuzzerCount = false;
  }
  if (!newDelayAvailable) {
    strcpy(newDelay, scrollDelayDefault);
    newDelayAvailable = true;
  }
  if (!newBrightnessAvailable) {
    strcpy(newBrightness, ledBrightnessDefault);
    newBrightnessAvailable = true;
  }
  if (!newAsciiConvAvailable) {
    strcpy(newAsciiConv, asciiConvDefault);
    newAsciiConvAvailable = true;
  }
  if (!newAlertChirpAvailable) {
    strcpy(newAlertChirp, alertChirpDefault);
    newAlertChirpAvailable = true;
  }
  serverHttp.send(204, "");  // No Content - prevents blocking main loop with 69KB page
  PRINTS("\nHTTP message received");  // Minimal logging after response sent
}

void onMessageCallJson(String jsonMsgData){
  // Removed local stack buffer which was causing stack overflow/crushes (3KB on stack is too huge for ESP8266)
  
  JsonDocument doc;
  Serial.println("Json Message Data Received: ");
  Serial.println(jsonMsgData);

  auto error = deserializeJson(doc, jsonMsgData);
  if (error) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(error.c_str());
    return;
  }

  String MSG = doc["MSG"];
  String REP = doc["REP"];
  String BUZ = doc["BUZ"];
  String DEL = doc["DEL"];
  String BRI = doc["BRI"];
  String ASC = doc["ASC"];
  String ALERTCHIRP = doc["ALERTCHIRP"];

  if (!doc["MSG"].isNull()) {
    MSG.toCharArray(newMessage, MSG_SIZE);
    newMessageAvailable = true;
    repeatCount = 0;
  } else { 
    strcpy(newMessage, "");
    newMessageAvailable = true;
  } 

  if (!doc["REP"].isNull()) {
    REP.toCharArray(newRepeat, REP_SIZE);
    repeatCount = 0;
    newRepeatAvailable = true;
  } else { 
    strcpy(newRepeat, repeatDefault);
    newRepeatAvailable = true;
  } 

  if (!doc["BUZ"].isNull()) {
    BUZ.toCharArray(newBuz, BUZ_SIZE);
    newBuzAvailable = true;
    explicitBuzzerCount = true;
  } else {
    strcpy(newBuz, buzzerDefault);
    newBuzAvailable = true;
    explicitBuzzerCount = false;
  }

  if (!doc["DEL"].isNull()) {
    DEL.toCharArray(newDelay, DEL_SIZE);
    newDelayAvailable = true;
  } else {
    strcpy(newDelay, scrollDelayDefault);
    newDelayAvailable = true;
  }

  if (!doc["BRI"].isNull()) {
    BRI.toCharArray(newBrightness, DEL_SIZE);
    newBrightnessAvailable = true;
  } else {
    strcpy(newBrightness, ledBrightnessDefault);
    newBrightnessAvailable = true;
  }
  
  /* BUZZER_ENABLE parsing removed to separate config from message logic */

  if (!doc["ASC"].isNull()) {
    ASC.toCharArray(newAsciiConv, ASC_SIZE);
    newAsciiConvAvailable = true;
  } else {
    strcpy(newAsciiConv, asciiConvDefault);
    newAsciiConvAvailable = true;
  }

  if (!doc["ALERTCHIRP"].isNull()) {
    ALERTCHIRP.toCharArray(newAlertChirp, STDSIZE);
    newAlertChirpAvailable = true;
  } else {
    strcpy(newAlertChirp, alertChirpDefault);
    newAlertChirpAvailable = true;
  }

  if (!doc["FORCEREP"].isNull()) {
    String forceRepVal = doc["FORCEREP"].as<String>();
    forceRepVal.toLowerCase();
    forceRepetitions = (forceRepVal == "true" || forceRepVal == "1" || forceRepVal == "on");
  } else {
    forceRepetitions = false;
  }
}

void displayText() {
  if (matrixFontTest) {
    P.setFont(getFontForFace(clockConfig.clockFace));
    matrixFontTest = false;
  } else {
    P.setFont(nullptr);
  }
  #ifndef DISABLE_SLEEP_MODE_FEATURE
  // Only blackout if sleep mode is active, NOT in mute-only mode, AND time is synced.
  // This ensures boot messages are visible even if sleep mode is 'active' as a fail-safe.
  if (isSleepModeActive() && !sleepModeMuteOnly && clockNtpSynced) {
    P.displayClear();
    return;
  }
  #endif
  strcpy(curMessage, newMessage); // copy it in
  M[0].msg = curMessage;

  // Set display mode to MESSAGE when displaying text (only if there's actually a message)
  if (strlen(curMessage) > 0) {
    currentDisplayMode = MODE_MESSAGE;
  }
  
  if (atoi(newAsciiConv) == 1) {
    utf8Ascii(M[0].msg);
  }
  
  // Publish initial countdown value for this new message cycle
  if (strlen(curMessage) == 0) {
      publishRepeatCountdownState(0);
  } else if (atoi(newRepeat) == 0) {
      publishRepeatCountdownState(-1);
  } else {
      int initialRemaining = atoi(newRepeat) - repeatCount; // Should be total remaining
      publishRepeatCountdownState(initialRemaining);
  }
  
  //M[0].spacing = 1; //Character spacing defaults to 1 but you can change it here
  //P.setScrollSpacing(0);
  PRINTS("\nScrolling Text");
  PRINT("\nCurrent Repeat: ", repeatCount);
  PRINT("\nRequested Repeats: ", newRepeat);
  P.displayText(M[0].msg, PA_CENTER, scrollDelay, PAUSE_TIME, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  P.setIntensity(getEffectiveBrightness(ledBrightness));
  P.setTextBuffer(M[0].msg);
  P.setCharSpacing(M[0].spacing);
  P.displayReset();

  // Notify Home Assistant of message and state changes
  publishMessageState();
  publishMessageDisplayedState();  // Update displayed message sensor
  publishLightState();
}

void scrollTextParola() {
  static char *p;

  // Only process messages when in MESSAGE mode - let clock/timer/alarm exit first
  if (clockEnabled && currentDisplayMode != MODE_MESSAGE) {
    return;
  }

#ifndef DISABLE_ALARM_FEATURE
  // Don't process messages when alarm is active
  if (alarmActive || currentDisplayMode == MODE_ALARM || currentDisplayMode == MODE_ALARM_EXIT) {
    return;
  }
#endif

#ifndef DISABLE_WEATHER_FEATURE
  // Don't process messages when weather is active
  if (currentDisplayMode == MODE_WEATHER || currentDisplayMode == MODE_WEATHER_EXIT) {
    return;
  }
#endif

#ifndef DISABLE_CRYPTO_FEATURE
  // Don't process messages when crypto ticker is active
  if (currentDisplayMode == MODE_CRYPTO || currentDisplayMode == MODE_CRYPTO_EXIT) {
    return;
  }
#endif

  if (newDelayAvailable) {
    scrollDelay = atoi(newDelay);
    newDelayAvailable = false;
  }
  if (newBrightnessAvailable) {
    ledBrightness = atoi(newBrightness);
    newBrightnessAvailable = false;
  }
  p = curMessage;      // reset the pointer to start of message
  if (newMessageAvailable) { // there is a new message waiting
    if ((P.displayAnimate())) {
      if (firstMessage == true) {
        displayText();
        firstMessage = false;
      }
      else {
        firstMessageOff = true;
      }
	    //changed this from previous version
      if ((newBuzAvailable) && (newMessageAvailable)) {
        strcpy(curMessage, newMessage);
        if (*p != '\0') {
          #ifndef DISABLE_SLEEP_MODE_FEATURE
          if (strcmp(generalConfig.buzzerEnable, "on") == 0 && !isSleepModeActive()) {
          #else
          if (strcmp(generalConfig.buzzerEnable, "on") == 0) {
          #endif
            int buzCount = atoi(newBuz);
            PRINT("[DEBUG] newBuz value: ", newBuz);
            PRINT(" buzCount: ", buzCount);
            PRINTS("\n");
            if (buzCount > 0) {
              // Determine which chirp to use
              const char* chirpToUse = newAlertChirpAvailable ? newAlertChirp : alertChirpDefault;
              PRINT("[DEBUG] chirpToUse: ", chirpToUse);
              PRINTS("\n");

              // Check if using Fast Beep (legacy behavior) or Simple Beep
              if (strcmp(chirpToUse, "Fast Beep") == 0) {
                // Legacy behavior: BUZ count controls 10ms HIGH/LOW cycles
                PRINTS("\n[DEBUG] Playing Fast Beep pattern (using LEDC/digitalWrite)");
                PRINT("BUZ count: ", buzCount);
                PRINTS("\n");
                for (int i = 1; i <= buzCount; i++) {
                  PRINTS("\nBUZZ");
                  #ifdef ESP32
                  ledcWriteTone(0, 1000);  // 1kHz tone
                  delay(10);
                  ledcWriteTone(0, 0);
                  #else
                  digitalWrite(BUZZER, HIGH);
                  delay(10);
                  digitalWrite(BUZZER, LOW);
                  #endif
                  delay(10);
                }
                PRINTS("[DEBUG] Fast Beep completed\n");
              } else if (strcmp(chirpToUse, "Simple Beep") == 0) {
                // Simple beep: play beep sound BUZ times (capped at 3 for message alerts)
                int beepCount = (buzCount > 3) ? 3 : buzCount;
                PRINTS("\n[DEBUG] Playing Simple Beep pattern (using LEDC/digitalWrite)");
                PRINT("Original buzCount: ", buzCount);
                PRINT(" Capped beepCount: ", beepCount);
                PRINTS("\n");
                // Show "MSG" indicator while alert plays
                P.print("MSG");
                // Play buzzer while MSG is displayed
                for (int i = 0; i < beepCount; i++) {
                  #ifdef ESP32
                  ledcWriteTone(0, 1000);  // 1kHz tone
                  delay(100);
                  ledcWriteTone(0, 0);
                  #else
                  digitalWrite(BUZZER, HIGH);
                  delay(100);
                  digitalWrite(BUZZER, LOW);
                  #endif
                  if (i < beepCount - 1) delay(100);
                }
                PRINTS("[DEBUG] Simple Beep completed\n");
                // Fade out "MSG" after alert finishes
                P.displayText("MSG", PA_CENTER, 20, 0, PA_PRINT, PA_FADE);
                P.displayReset();
                while (!P.displayAnimate()) { yield(); }
                P.displayClear();
              } else if (strcmp(chirpToUse, "Silent") != 0) {
                // Named chirp pattern: BUZ controls repeat count (capped at 1 for message alerts by default)
                // If user doesn't specify a count, it defaults to 1. If they do explicitly pass a count,
                // we'll still cap it at 3 to prevent abusive long melodies.
                int chirpRepeatCount = 1; 
                if (explicitBuzzerCount) {
                   chirpRepeatCount = (buzCount > 3) ? 3 : buzCount;
                }
                
                // Show "MSG" indicator while alert plays
                P.print("MSG");
                // Play chirp while MSG is displayed
                playChirpByName(chirpToUse, chirpRepeatCount);
                // Fade out "MSG" after alert finishes
                P.displayText("MSG", PA_CENTER, 20, 0, PA_PRINT, PA_FADE);
                P.displayReset();
                while (!P.displayAnimate()) { yield(); }
                P.displayClear();
              }
              // Silent: do nothing
            }
          }
          newBuzAvailable = false;
          newAlertChirpAvailable = false;
        }
        else {
          if ((atoi(newRepeat) == 0)  && (firstMessage == false)) {
            repeatCount = 1;
            strcpy(newRepeat, "1");
          }
          else {
           if (repeatCount < atoi(newRepeat)) {
          // Update countdown (N remaining)
          int total = atoi(newRepeat);
          int remaining = total - repeatCount;
          // Note: repeatCount starts at 0, so if total=3:
          // 0 -> 3 remaining (displays now)
          // 1 -> 2 remaining
          // 2 -> 1 remaining
          
          if (strlen(curMessage) > 0) {
             publishRepeatCountdownState(remaining);
          } else {
             publishRepeatCountdownState(0);
          }

          repeatCount++;
          displayText();
        }
        else {
          curMessage[0] = '\0';
          repeatCount = atoi(newRepeat);
          newMessageAvailable = false;
          // Notify HA that message has finished/cleared
          publishMessageDisplayedState();
          publishRepeatCountdownState(0);
        }
          }
        }
      }
      if (atoi(newRepeat) == 0 && strlen(curMessage) > 0) {
        publishRepeatCountdownState(-1); // Infinite
        repeatCount = 0;
        displayText();
      }
      else {
        if (repeatCount < atoi(newRepeat)) {
          repeatCount++;
          displayText();
        }
        else {
          curMessage[0] = '\0';
          repeatCount = atoi(newRepeat);
          newMessageAvailable = false;
          // Notify HA that message has finished/cleared
          publishMessageDisplayedState();
          publishRepeatCountdownState(0);
        }
      }
    }
  }
  if (newRepeatAvailable) {
    newRepeatAvailable = false;
  }
  if (newAsciiConvAvailable) {
    newAsciiConvAvailable = false;
  }
}

void displaySilentMsg() {
  repeatCount = 0;
  strcpy(newRepeat, "1");
  strcpy(newBuz, "0");
  strcpy(newBrightness, ledBrightnessDefault);
  strcpy(newAsciiConv, asciiConvDefault);
  newRepeatAvailable = true;
  newMessageAvailable = true;
  newBuzAvailable = true;
  newBrightnessAvailable = true;
  newAsciiConvAvailable = true;
  PRINT("\nSilent text to scroll: \n", newMessage);
  
  // BLOCKING: Wait for message to scroll completely before returning
  // This ensures setup message displays BEFORE WiFiManager starts scanning
  firstMessageOff = false;
  while(!firstMessageOff) {
    scrollTextParola();
    yield();
  }
  scrollTextParola();
}

// Blocking display function for Boot/Setup
// handleServer: true if we should call handleHttpServer() during wait (Connected mode), false if not (AP/WiFiManager mode)
// repeats: number of times to scroll the message
void displayBlockingMessage(bool handleServer, int repeats) {
  // Use standard defaults used in silent msg
  repeatCount = 0;
  P.setFont(nullptr);
  
  // Setup display immediately
  P.displayText(newMessage, PA_CENTER, scrollDelay, PAUSE_TIME, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  P.setIntensity(getEffectiveBrightness(atoi(ledBrightnessDefault)));

  // Loop to scroll message N times
  for(int i=0; i<repeats; i++) {
     P.displayReset(); // Ensure start from beginning
     while (!P.displayAnimate()) {
         if (handleServer) handleHttpServer();
         yield(); // Allow system background tasks (WiFi)
     }
  }
}


// this will run just before the wifisetup server is started if there is no wifi ssid setup
void configModeCallback (WiFiManager *myWiFiManager) {
  wifiModeBuzzer();
  sprintf(newMessage, "WiFi Setup Mode - IP: %s - SSID: %s - Password: %s - Version: %s", 
          "192.168.4.1", ap_mode_ssid, ap_mode_password, VERSION);
  displaySilentMsg();
  Serial.println("Entered config mode");
  apModeIP = WiFi.softAPIP();
  apModeSSID = myWiFiManager->getConfigPortalSSID();
  Serial.println(apModeIP);
  Serial.println(apModeSSID);
  afterWiFiConfig = true;
}

void rebootDevice() {
  ESP.restart();
}

//clears all configs
void factoryReset() {
  Serial.println("Factory Reset: Deleting all configuration files...");
  
  // Format the filesystem to ensure ALL files are removed (reliable wipe)
  Serial.println("Formatting LittleFS...");
  if (LittleFS.format()) {
    Serial.println("LittleFS formatted successfully");
  } else {
    Serial.println("Error formatting LittleFS");
  }
  
  // Flush filesystem
  delay(500);
  
  Serial.println("Factory Reset: Erasing WiFi credentials from NVS...");
  // Disconnect AND erase stored WiFi credentials
  // First true: disconnect and erase STA config
  // Second true: also erase AP config
  WiFi.disconnect(true, true);
  
  delay(2000);
  
  Serial.println("Factory Reset: Rebooting...");
  ESP.restart();
}

// Flash button callback function to be called when the button is pressed.
void onPressed() {
  Serial.println("\nFlash button has been pressed, Erasing all Config and Resetting.");
  factoryReset();
}

// Called from web factoryreset url
void webFactoryReset() {
  Serial.println("\nWeb Factory Reset has been initiated, Erasing all Config and Resetting.");
  factoryReset();
}

// ============================================================================
// CLOCK FUNCTIONS
// ============================================================================

// Apply timezone using POSIX TZ string (always applies, even if clock disabled)
void applyTimezone() {
  Serial.print(F("Applying timezone: "));
  Serial.println(clockConfig.tzString);

  // Set timezone using POSIX TZ string
  setenv("TZ", clockConfig.tzString, 1);
  tzset();

  // Give tzset() time to take effect (critical for timezone to be applied)
  delay(250);

  // Force fresh time calculation to verify
  time_t now = time(nullptr);
  if (now > 100000) {
    struct tm* timeinfo = localtime(&now);
    Serial.print(F("Current time with new TZ: "));
    Serial.print(timeinfo->tm_hour);
    Serial.print(F(":"));
    if (timeinfo->tm_min < 10) Serial.print(F("0"));
    Serial.println(timeinfo->tm_min);
  }
}

#ifdef ESP8266
  #include <sntp.h>
#endif

// Initialize NTP time synchronization (ESP8266/ESP32 compatible)
void initNtpTime() {
  // Only configure NTP if clock is enabled
  if (!clockEnabled) {
    // Even if clock is disabled, we must apply the timezone so internal time is correct
    // if the user enables it later without rebooting
    setenv("TZ", clockConfig.tzString, 1);
    tzset();
    return;
  }

  Serial.print(F("Initializing NTP - Server: "));
  Serial.println(clockConfig.ntpServer);
  Serial.print(F("Timezone: "));
  Serial.println(clockConfig.tzString);

  // setenv("TZ", ...) is handled automatically by the new configTime() overload
  // on modern ESP8266/ESP32 cores (v2.7.0+ for ESP8266)
  
  // Use the modern configTime that takes the TZ string directly
  // This is more robust for persistence and correct application
  #ifdef ESP8266
    sntp_servermode_dhcp(0); // Disable NTP from DHCP to ensure static config is used
    configTime(clockConfig.tzString, clockConfig.ntpServer);
  #else
    // ESP32: Stop SNTP if running, reconfigure with static server, restart
    if (esp_sntp_enabled()) {
      esp_sntp_stop();
    }
    esp_sntp_servermode_dhcp(0); // Disable NTP from DHCP to ensure static config is used
    esp_sntp_setoperatingmode(ESP_SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, clockConfig.ntpServer);
    esp_sntp_init();
    // Set timezone
    setenv("TZ", clockConfig.tzString, 1);
    tzset();
  #endif

  // Don't block - just configure and let it sync in background
  Serial.println(F("NTP configured, syncing in background..."));

  // Check if already synced
  time_t now = time(nullptr);
  if (now > 100000) {
    Serial.println(F("NTP synced!"));
    clockNtpSynced = true;
    lastNtpResync = millis();

    struct tm* timeinfo = localtime(&now);
    Serial.print(F("Current time: "));
    Serial.print(timeinfo->tm_hour);
    Serial.print(F(":"));
    if (timeinfo->tm_min < 10) Serial.print(F("0"));
    Serial.print(timeinfo->tm_min);
    Serial.print(F(" ("));
    Serial.print(clockConfig.tzString);
    Serial.println(F(")"));
  } else {
    Serial.println(F("Waiting for NTP sync..."));
    clockNtpSynced = false;
  }
}

// Get current time from NTP (ESP8266/ESP32 compatible)
void getNtpTime(int &hours, int &minutes, int &seconds) {
  time_t now = time(nullptr);
  struct tm* timeinfo = localtime(&now);

  hours = timeinfo->tm_hour;
  minutes = timeinfo->tm_min;
  seconds = timeinfo->tm_sec;
}

// Convert effect name string to MD_Parola effect enum
textEffect_t getTransitionEffect(const char* effectName) {
  if (strcmp(effectName, "FADE") == 0) return PA_FADE;
  if (strcmp(effectName, "DISSOLVE") == 0) return PA_DISSOLVE;
  if (strcmp(effectName, "BLINDS") == 0) return PA_BLINDS;
  if (strcmp(effectName, "WIPE") == 0) return PA_WIPE;
  if (strcmp(effectName, "GROW") == 0) return PA_GROW_UP;
  if (strcmp(effectName, "SCROLL") == 0) return PA_SCROLL_UP;
  if (strcmp(effectName, "SCAN") == 0) return PA_SCAN_HORIZ;
  if (strcmp(effectName, "OPENING") == 0) return PA_OPENING;
  if (strcmp(effectName, "CLOSING") == 0) return PA_CLOSING;
  if (strcmp(effectName, "SCROLL_DOWN") == 0) return PA_SCROLL_DOWN;
  if (strcmp(effectName, "SCROLL_LEFT") == 0) return PA_SCROLL_LEFT;
  if (strcmp(effectName, "SCROLL_RIGHT") == 0) return PA_SCROLL_RIGHT;
  if (strcmp(effectName, "SLICE") == 0) return PA_SLICE;
  if (strcmp(effectName, "MESH") == 0) return PA_MESH;
  if (strcmp(effectName, "GROW_DOWN") == 0) return PA_GROW_DOWN;
  if (strcmp(effectName, "SCAN_VERTX") == 0) return PA_SCAN_VERTX;
  if (strcmp(effectName, "SCROLL_UP_LEFT") == 0) return PA_SCROLL_UP_LEFT;
  if (strcmp(effectName, "SCROLL_UP_RIGHT") == 0) return PA_SCROLL_UP_RIGHT;
  if (strcmp(effectName, "OPENING_CURSOR") == 0) return PA_OPENING_CURSOR;
  if (strcmp(effectName, "SCAN_VERT") == 0) return PA_SCAN_VERT;
  if (strcmp(effectName, "SCROLL_DOWN_LEFT") == 0) return PA_SCROLL_DOWN_LEFT;
  if (strcmp(effectName, "WIPE_CURSOR") == 0) return PA_WIPE_CURSOR;
  if (strcmp(effectName, "SCAN_HORIZX") == 0) return PA_SCAN_HORIZX;
  if (strcmp(effectName, "CLOSING_CURSOR") == 0) return PA_CLOSING_CURSOR;
  if (strcmp(effectName, "SCROLL_DOWN_RIGHT") == 0) return PA_SCROLL_DOWN_RIGHT;
  if (strcmp(effectName, "RANDOM") == 0) return PA_RANDOM;
  if (strcmp(effectName, "PRINT") == 0) return PA_PRINT;
  return PA_FADE; // Default
}

// Display clock on LED matrix
void displayClock(bool withAnimation) {
  #ifndef DISABLE_SLEEP_MODE_FEATURE
  if (isSleepModeActive() && !sleepModeMuteOnly) {
    P.displayClear();
    return;
  }
  #endif

  // Select clock font. Custom bitmap fonts use getFontForFace(). For the DEFAULT
  // Parola font, use ClockFixedFont instead of nullptr so that digit '1' is
  // rendered at a fixed width (same as all other digits), preventing the
  // centred display from shifting when a '1' appears or disappears.
  {
    MD_MAX72XX::fontType_t* f = getFontForFace(clockConfig.clockFace);
    P.setFont(f != nullptr ? f : (MD_MAX72XX::fontType_t*)ClockFixedFont);
  }

  static char timeString[DATE_FORMAT_SIZE]; // Increased from [6]

  getFormattedTime(timeString, sizeof(timeString), true);

  P.setIntensity(getEffectiveBrightness(clockBrightness));

  // For initial display with animation
  if (withAnimation) {
    // Clear display to prevent flicker of old content
    P.displayClear(); 
    
    // Select effect - random or configured
    textEffect_t effect;
    if (strcmp(clockConfig.randomizeTransition, "on") == 0) {
      // Pick a random effect (0-24, excluding PA_PRINT which is instant)
      int effectIndex = random(0, 25);
      textEffect_t effects[] = {PA_FADE, PA_DISSOLVE, PA_BLINDS, PA_WIPE, PA_GROW_UP, PA_SCROLL_UP, PA_SCAN_HORIZ, PA_OPENING, PA_CLOSING, PA_SCROLL_DOWN, PA_SCROLL_LEFT, PA_SCROLL_RIGHT, PA_SLICE, PA_MESH, PA_GROW_DOWN, PA_SCAN_VERTX, PA_SCROLL_UP_LEFT, PA_SCROLL_UP_RIGHT, PA_OPENING_CURSOR, PA_SCAN_VERT, PA_SCROLL_DOWN_LEFT, PA_WIPE_CURSOR, PA_SCAN_HORIZX, PA_CLOSING_CURSOR, PA_SCROLL_DOWN_RIGHT};
      effect = effects[effectIndex];
    } else {
      effect = getTransitionEffect(clockConfig.transitionEffect);
    }
    
    // Use configured transition speed
    int speed = atoi(clockConfig.transitionSpeed);
    uint16_t animSpeed = (speed < 10) ? 10 : speed; 
    P.displayText(timeString, PA_CENTER, animSpeed, 0, effect, PA_NO_EFFECT);
    P.displayReset();
  } else {
    // For updates: just use print (no animation, no reset)
    P.print(timeString);
  }
}

// Update colon blink state (returns true if display needs redraw)
bool updateColonBlink() {
  if (millis() - lastColonToggle >= 500) {
    clockColonVisible = !clockColonVisible;
    lastColonToggle = millis();
    return true; // Needs redraw
  }
  return false;
}

// Update clock display (called from main loop when in clock mode)
void updateClockDisplay() {
  static int lastPublishedMinute = -1;
  bool needsRedraw = false;

  // Update colon blink every 500ms
  if (updateColonBlink()) {
    needsRedraw = true;
  }

  // Check date alternating (4-module builds)
  if (updateDateAlternate()) {
    needsRedraw = true;
  }

  if (needsRedraw) {
    displayClock(false); // Update display with new colon state
  }

  // Publish clock time to MQTT every minute
  int hours, minutes, seconds;
  getNtpTime(hours, minutes, seconds);
  if (minutes != lastPublishedMinute) {
    lastPublishedMinute = minutes;
    publishClockTimeState();
  }
}

// Main state machine for switching between message, clock, and timer modes
void updateDisplayMode() {
  // Priority: Timer (when running) > Clock (when enabled) > Message

  // Scroll repeat counters — reset when entering the respective mode from MODE_CLOCK
#ifndef DISABLE_WEATHER_FEATURE
  static int weatherScrollCount = 0;
#endif
#ifndef DISABLE_CRYPTO_FEATURE
  static int cryptoScrollCount = 0;
#endif

#ifndef DISABLE_TIMER_FEATURE
  // Check if timer should take over from any mode
  if (timerEnabled && timerRunning && currentDisplayMode != MODE_TIMER) {
    // Timer started, transition to timer mode
    currentDisplayMode = MODE_TIMER;
    P.displayClear();
    displayTimer(true);
    Serial.println(F("Transitioning to timer mode"));
  }
#endif
  
  switch (currentDisplayMode) {
    case MODE_MESSAGE:
#ifndef DISABLE_TIMER_FEATURE
      // Check if timer should take over
      if (timerEnabled && timerRunning) {
        currentDisplayMode = MODE_TIMER;
        P.displayClear();
        displayTimer(true);
        Serial.println(F("Timer mode active"));
        break;
      }
#endif
      
      // Check if message finished (no new message, current message empty, clock enabled, animation complete)
      // Additional P.displayAnimate() check ensures we don't interrupt mid-scroll
      #ifndef DISABLE_SLEEP_MODE_FEATURE
      if (!newMessageAvailable && curMessage[0] == '\0' && clockEnabled && (isSleepModeActive() ? sleepModeMuteOnly : true) && P.displayAnimate()) {
      #else
      if (!newMessageAvailable && curMessage[0] == '\0' && clockEnabled && P.displayAnimate()) {
      #endif
        // Message finished, start transition to clock
        currentDisplayMode = MODE_TRANSITION;
        transitionStartTime = millis();
        P.displayClear();
        Serial.println(F("Transitioning to clock mode"));
        publishDisplayModeState();
        publishClockDisplayActiveState();
      }
      break;

    case MODE_TRANSITION:
      // Wait for transition delay
      if (millis() - transitionStartTime >= (unsigned long)atoi(clockConfig.transitionDelayMs)) {
        #ifndef DISABLE_SLEEP_MODE_FEATURE
        // If sleep mode activated during transition, abort and return to message mode
        if (isSleepModeActive() && !sleepModeMuteOnly) {
          currentDisplayMode = MODE_MESSAGE;
          P.displayClear();
          Serial.println(F("Transition aborted - sleep mode active"));
          publishDisplayModeState();
          publishClockDisplayActiveState();
          break;
        }
        #endif
        
        // Transition complete, switch to clock
        currentDisplayMode = MODE_CLOCK;
        lastClockUpdate = millis();
        lastColonToggle = millis();
        clockColonVisible = true;
        displayClock(true); // Initial display with animation
        Serial.println(F("Clock mode active"));
        publishDisplayModeState();
        publishClockDisplayActiveState();
      }
      break;

    case MODE_CLOCK:
#ifndef DISABLE_TIMER_FEATURE
      // Check if timer should take over
      if (timerEnabled && timerRunning) {
        currentDisplayMode = MODE_TIMER;
        P.displayClear();
        displayTimer(true);
        Serial.println(F("Timer mode active"));
        publishDisplayModeState();
        publishClockDisplayActiveState();
        break;
      }
#endif

#ifndef DISABLE_WEATHER_FEATURE
      // Check if weather should be displayed
      {
        static unsigned long lastWeatherDisplayTime = 0;

        unsigned long weatherDisplayInterval = (unsigned long)atoi(weatherConfig.displayIntervalMinutes) * 60000UL;

        if (shouldDisplayWeather() &&
            (lastWeatherDisplayTime == 0 || millis() - lastWeatherDisplayTime >= weatherDisplayInterval)) {
          // Time to show weather
          currentDisplayMode = MODE_WEATHER;
          weatherDisplayStart = millis();
          weatherScrollCount = 0;
          lastWeatherDisplayTime = millis();
          P.displayClear();
          displayWeather(true);
          Serial.println(F("Transitioning to weather mode"));
          publishDisplayModeState();
          break;
        }
      }
#endif

#ifndef DISABLE_CRYPTO_FEATURE
      // Check if crypto prices should be displayed
      {
        static unsigned long lastCryptoDisplayTime = 0;

        unsigned long cryptoDisplayInterval = (unsigned long)atoi(cryptoConfig.displayIntervalMinutes) * 60000UL;

        if (shouldDisplayCrypto() &&
            (lastCryptoDisplayTime == 0 || millis() - lastCryptoDisplayTime >= cryptoDisplayInterval)) {
          currentDisplayMode = MODE_CRYPTO;
          cryptoDisplayStart = millis();
          cryptoScrollCount = 0;
          lastCryptoDisplayTime = millis();
          P.displayClear();
          displayCrypto(true);
          Serial.println(F("Transitioning to crypto mode"));
          publishDisplayModeState();
          break;
        }
      }
#endif

      // Check if new message arrived
      if (newMessageAvailable) {
        #ifndef DISABLE_SLEEP_MODE_FEATURE
        // If sleep mode is active, skip exit animation and go directly to message mode
        if (isSleepModeActive() && !sleepModeMuteOnly) {
          currentDisplayMode = MODE_MESSAGE;
          P.displayClear();
          Serial.println(F("Clock exit skipped - sleep mode active"));
          publishDisplayModeState();
          publishClockDisplayActiveState();
          break;
        }
        #endif
        
        // Message interrupts clock - start exit animation
        currentDisplayMode = MODE_CLOCK_EXIT;

        // Get current time for exit animation
        static char exitTimeString[6];
        int hours, minutes, seconds;
        getNtpTime(hours, minutes, seconds);
        if (!clockNtpSynced) {
          strcpy(exitTimeString, "--:--");
        } else {
          sprintf(exitTimeString, "%02d:%02d", hours, minutes);
        }

        // Set up exit animation (instant in, animated out)
        textEffect_t effect;
        if (strcmp(clockConfig.randomizeTransition, "on") == 0) {
          // Pick a random effect for exit
          int effectIndex = random(0, 25);
          textEffect_t effects[] = {PA_FADE, PA_DISSOLVE, PA_BLINDS, PA_WIPE, PA_GROW_UP, PA_SCROLL_UP, PA_SCAN_HORIZ, PA_OPENING, PA_CLOSING, PA_SCROLL_DOWN, PA_SCROLL_LEFT, PA_SCROLL_RIGHT, PA_SLICE, PA_MESH, PA_GROW_DOWN, PA_SCAN_VERTX, PA_SCROLL_UP_LEFT, PA_SCROLL_UP_RIGHT, PA_OPENING_CURSOR, PA_SCAN_VERT, PA_SCROLL_DOWN_LEFT, PA_WIPE_CURSOR, PA_SCAN_HORIZX, PA_CLOSING_CURSOR, PA_SCROLL_DOWN_RIGHT};
          effect = effects[effectIndex];
        } else {
          effect = getTransitionEffect(clockConfig.transitionEffect);
        }
        int speed = atoi(clockConfig.transitionSpeed);
        uint16_t animSpeed = (speed < 10) ? 10 : speed;
        P.displayText(exitTimeString, PA_CENTER, animSpeed, 0, PA_PRINT, effect);
        P.displayReset();

        Serial.println(F("Clock exiting for new message"));
        publishDisplayModeState();
        publishClockDisplayActiveState();
      } else {
        // Continue animation if running
        if (P.displayAnimate()) {
          // Animation complete or not running, allow static updates
          updateClockDisplay();
        }
      }
      break;

    case MODE_CLOCK_EXIT:
      // Wait for exit animation to complete
      if (P.displayAnimate()) {
        // Exit animation complete, switch to message mode
        currentDisplayMode = MODE_MESSAGE;
        P.displayClear();
        Serial.println(F("Clock exit complete, starting message"));
        publishDisplayModeState();
        publishClockDisplayActiveState();
      }
      break;

#ifndef DISABLE_TIMER_FEATURE
    case MODE_TIMER:
      // Check if timer stopped
      if (!timerRunning) {
        // Timer stopped, transition based on what should take over
        currentDisplayMode = MODE_TIMER_EXIT;
        P.displayClear();
        Serial.println(F("Timer stopped, exiting timer mode"));
      } else {
        // Update timer display
        if (P.displayAnimate()) {
          displayTimer(false);
        }
      }
      break;

    case MODE_TIMER_EXIT:
      // Animate timer exit
      if (P.displayAnimate()) {
        // Exit complete, determine next mode
        if (clockEnabled && !newMessageAvailable && curMessage[0] == '\0') {
          // Go to clock
          currentDisplayMode = MODE_CLOCK;
          lastClockUpdate = millis();
          lastColonToggle = millis();
          clockColonVisible = true;
          displayClock(true);
          Serial.println(F("Timer exit complete, starting clock"));
          publishDisplayModeState();
          publishClockDisplayActiveState();
        } else {
          // Go to message
          currentDisplayMode = MODE_MESSAGE;
          Serial.println(F("Timer exit complete, starting message"));
          publishDisplayModeState();
          publishClockDisplayActiveState();
        }
      }
      break;
#endif

#ifndef DISABLE_ALARM_FEATURE
    case MODE_ALARM:
      // Alarm display is handled by updateAlarms() in main loop
      // This case is here for completeness - no action needed
      break;

    case MODE_ALARM_EXIT:
      // Alarm exit is handled by updateAlarms() in main loop
      // This case is here for completeness - no action needed
      break;
#endif

#ifndef DISABLE_WEATHER_FEATURE
    case MODE_WEATHER:
      // Alarm interrupts immediately (time-critical). Messages and timers wait for the
      // current scroll to finish so weather is never cut off mid-word.
      {
#ifndef DISABLE_ALARM_FEATURE
        if (alarmActive) {
          currentDisplayMode = MODE_WEATHER_EXIT;
          Serial.println(F("Weather interrupted by alarm"));
          break;
        }
#endif

        // Wait for the current scroll animation to complete before making any exit decision.
        // This prevents mid-scroll cut-offs regardless of what triggered the exit.
        if (P.displayAnimate()) {
          weatherScrollCount++;
          int maxScrolls = atoi(weatherConfig.displayRepeatCount);

          // Exit when: all repetitions done, a message is waiting, or timer started
          bool shouldExit = (weatherScrollCount >= maxScrolls) || newMessageAvailable;
#ifndef DISABLE_TIMER_FEATURE
          if (timerRunning) shouldExit = true;
#endif
          if (shouldExit) {
            currentDisplayMode = MODE_WEATHER_EXIT;
            Serial.println(F("Weather scroll(s) complete, exiting"));
          } else {
            displayWeather(true);  // Start next repetition
          }
        }
        // Note: newMessageAvailable / timerRunning deliberately NOT checked mid-scroll.
      }
      break;

    case MODE_WEATHER_EXIT:
      // Transition back to clock mode
      if (P.displayAnimate()) {
        P.displayClear();

        if (newMessageAvailable) {
          // Message waiting, go to message mode
          currentDisplayMode = MODE_MESSAGE;
          Serial.println(F("Weather exit complete, starting message"));
        } else if (clockEnabled) {
          // Return to clock
          currentDisplayMode = MODE_CLOCK;
          lastClockUpdate = millis();
          lastColonToggle = millis();
          clockColonVisible = true;
          displayClock(true);
          Serial.println(F("Weather exit complete, returning to clock"));
        } else {
          // Fallback to message mode
          currentDisplayMode = MODE_MESSAGE;
          Serial.println(F("Weather exit complete, message mode"));
        }
        publishDisplayModeState();
        publishClockDisplayActiveState();
      }
      break;
#endif

#ifndef DISABLE_CRYPTO_FEATURE
    case MODE_CRYPTO:
      // Alarm interrupts immediately (time-critical). Messages and timers wait for the
      // current scroll to finish so prices are never cut off mid-display.
      {
#ifndef DISABLE_ALARM_FEATURE
        if (alarmActive) {
          currentDisplayMode = MODE_CRYPTO_EXIT;
          Serial.println(F("Crypto interrupted by alarm"));
          break;
        }
#endif

        // Wait for the current scroll animation to complete before making any exit decision.
        if (P.displayAnimate()) {
          cryptoScrollCount++;
          int maxScrolls = atoi(cryptoConfig.displayRepeatCount);

          bool shouldExit = (cryptoScrollCount >= maxScrolls) || newMessageAvailable;
#ifndef DISABLE_TIMER_FEATURE
          if (timerRunning) shouldExit = true;
#endif
          if (shouldExit) {
            currentDisplayMode = MODE_CRYPTO_EXIT;
            Serial.println(F("Crypto scroll(s) complete, exiting"));
          } else {
            displayCrypto(true);  // Start next repetition
          }
        }
        // Note: newMessageAvailable / timerRunning deliberately NOT checked mid-scroll.
      }
      break;

    case MODE_CRYPTO_EXIT:
      // Transition back to clock mode
      if (P.displayAnimate()) {
        P.displayClear();

        if (newMessageAvailable) {
          currentDisplayMode = MODE_MESSAGE;
          Serial.println(F("Crypto exit complete, starting message"));
        } else if (clockEnabled) {
          currentDisplayMode = MODE_CLOCK;
          lastClockUpdate = millis();
          lastColonToggle = millis();
          clockColonVisible = true;
          displayClock(true);
          Serial.println(F("Crypto exit complete, returning to clock"));
        } else {
          currentDisplayMode = MODE_MESSAGE;
          Serial.println(F("Crypto exit complete, message mode"));
        }
        publishDisplayModeState();
        publishClockDisplayActiveState();
      }
      break;
#endif
  }
}

// Check WiFi connection and trigger NTP resync if needed
void checkWiFiAndResync() {
  static bool wasConnected = true;
  static unsigned long lastSyncCheck = 0;
  bool isConnected = (WiFi.status() == WL_CONNECTED);

  // Check if NTP has synced in background (check every 5 seconds)
  if (clockEnabled && !clockNtpSynced && isConnected && (millis() - lastSyncCheck >= 5000)) {
    time_t now = time(nullptr);
    if (now > 100000) {
      Serial.println(F("NTP sync completed in background!"));
      clockNtpSynced = true;
      lastNtpResync = millis();

      struct tm* timeinfo = localtime(&now);
      Serial.print(F("Current time: "));
      Serial.print(timeinfo->tm_hour);
      Serial.print(F(":"));
      Serial.println(timeinfo->tm_min);
    }
    lastSyncCheck = millis();
  }

  // Detect WiFi reconnection
  if (!wasConnected && isConnected && clockEnabled) {
    Serial.println(F("WiFi reconnected, triggering NTP resync"));
    initNtpTime();
  }

  // Periodic resync check (every N hours as configured)
  if (clockEnabled && clockNtpSynced && isConnected) {
    unsigned long resyncInterval = (unsigned long)atoi(clockConfig.resyncIntervalHours) * 3600000UL; // Convert hours to ms
    if (millis() - lastNtpResync >= resyncInterval) {
      Serial.println(F("Periodic NTP resync triggered"));
      initNtpTime();
    }
  }

  wasConnected = isConnected;
}

// Returns the MD_MAX72XX font pointer for the configured clock face (nullptr = built-in Parola font)
static MD_MAX72XX::fontType_t* getFontForFace(const char* face) {
  if (strcmp(face, "MATRIX_LIGHT")    == 0) return (MD_MAX72XX::fontType_t*)MatrixLight8Font;
  if (strcmp(face, "MATRIX_LIGHT_6")  == 0) return (MD_MAX72XX::fontType_t*)MatrixLight6Font;
  if (strcmp(face, "MATRIX_LIGHT_6X") == 0) return (MD_MAX72XX::fontType_t*)MatrixLight6XFont;
  if (strcmp(face, "MATRIX_CHUNKY_6") == 0) return (MD_MAX72XX::fontType_t*)MatrixChunky6Font;
  if (strcmp(face, "MATRIX_CHUNKY_6X")== 0) return (MD_MAX72XX::fontType_t*)MatrixChunky6XFont;
  if (strcmp(face, "MATRIX_LIGHT_8X") == 0) return (MD_MAX72XX::fontType_t*)MatrixLight8XFont;
  return nullptr; // DEFAULT or unknown → built-in Parola font
}

// True when clockFace is any custom bitmap font (not the built-in Parola DEFAULT)
static bool isMatrixLightFace() {
  return getFontForFace(clockConfig.clockFace) != nullptr;
}

// Format clock display based on current alternate state and options
void formatClockDate(char* buffer, size_t bufferSize, const char* format) {
  time_t now = time(nullptr);
  struct tm* timeinfo = localtime(&now);

  if (!clockNtpSynced) {
    strlcpy(buffer, "--:--", bufferSize);
    return;
  }

  const char* strftimeFormat = nullptr;
  bool ampm = strcmp(clockConfig.clockAmPm, "on") == 0;

  // When date alternation is active, states 1 and 2 override the time format
  if (strcmp(clockConfig.dateAlternate, "on") == 0) {
    if (clockAlternateState == 1) {
      // Day-of-week step — only reached when Matrix Light font is active
      strftimeFormat = "%A";  // "Monday"
    } else if (clockAlternateState == 2) {
      strftimeFormat = "%b %e";  // "Jan  6"
    }
  }

  // State 0 (or alternation off): show time in the selected format
  if (strftimeFormat == nullptr) {
    #if MAX_DEVICES == 4
      if (strcmp(format, "TIME_SECONDS") == 0) {
        strftimeFormat = "%H:%M.%S";  // AM/PM + seconds exceeds 32px on 4m — always 24h
      } else {
        // TIME_ONLY (and any other format on 4m)
        if (ampm)
          strftimeFormat = clockColonVisible ? "%I:%M %p" : "%I %M %p";
        else
          strftimeFormat = clockColonVisible ? "%H:%M" : "%H %M";
      }
    #elif MAX_DEVICES == 8
      if (strcmp(format, "TIME_DATE") == 0) {
        strftimeFormat = clockColonVisible ? "%H:%M %b %e" : "%H %M %b %e";
      } else if (strcmp(format, "FULL_DATE") == 0) {
        strftimeFormat = "%a %b %e %Y";
      } else if (strcmp(format, "TIME_FULL_DATE") == 0) {
        strftimeFormat = clockColonVisible ? "%H:%M %a %b %e" : "%H %M %a %b %e";
      } else if (strcmp(format, "CUSTOM") == 0) {
        strftimeFormat = clockConfig.customDateFormat;
      } else if (strcmp(format, "TIME_SECONDS") == 0) {
        strftimeFormat = ampm ? "%I:%M.%S %p" : "%H:%M.%S";
      } else {
        // TIME_ONLY
        if (ampm)
          strftimeFormat = clockColonVisible ? "%I:%M %p" : "%I %M %p";
        else
          strftimeFormat = clockColonVisible ? "%H:%M" : "%H %M";
      }
    #endif
  }

  if (strftimeFormat == nullptr) {
    strftimeFormat = clockColonVisible ? "%H:%M" : "%H %M";  // Fallback
  }

  strftime(buffer, bufferSize, strftimeFormat, timeinfo);

  // Remove extra spaces from %e padding
  char* doubleSpace;
  while ((doubleSpace = strstr(buffer, "  ")) != nullptr) {
    memmove(doubleSpace, doubleSpace + 1, strlen(doubleSpace));
  }

#if MAX_DEVICES == 4
  // Truncate long day names to fit 32px display (e.g. "Wednesday" → "Wednes.y")
  if (clockAlternateState == 1 && strlen(buffer) > 7) {
    char last = buffer[strlen(buffer) - 1];
    buffer[6] = '.';
    buffer[7] = last;
    buffer[8] = '\0';
  }
#endif
}

// Get formatted time/date based on configuration
void getFormattedTime(char* buffer, size_t bufferSize, bool includeDate) {
  formatClockDate(buffer, bufferSize, clockConfig.dateFormat);
}

// Advance alternate state and return true when display needs redraw
bool updateDateAlternate() {
  if (strcmp(clockConfig.dateAlternate, "on") != 0) {
    return false;
  }

  // State 0 = time display; states 1/2 = weekday/date — each uses its own duration
  unsigned long interval = (clockAlternateState == 0)
    ? (unsigned long)atoi(clockConfig.clockDisplaySeconds) * 1000UL
    : (unsigned long)atoi(clockConfig.dateAlternateSeconds) * 1000UL;
  if (millis() - lastDateAlternate >= interval) {
    lastDateAlternate = millis();

    if (isMatrixLightFace()) {
      // 3-step cycle: time → day-of-week → date → time
      clockAlternateState = (clockAlternateState + 1) % 3;
    } else {
      // 2-step cycle: time ↔ date
      clockAlternateState = (clockAlternateState == 0) ? 2 : 0;
    }
    return true;
  }
  return false;
}

#ifndef DISABLE_SLEEP_MODE_FEATURE
// Sleep Mode Functions

/**
 * Returns the current sleep mode active state
 * @return true if sleep mode is currently active, false otherwise
 */
bool isSleepModeActive() {
  return sleepModeActive;
}

/**
 * Updates the sleep mode active state based on current time and configuration
 * Called every loop iteration to check if device should be in sleep mode
 *
 * Logic:
 * - If sleep mode disabled: deactivate sleep mode
 * - If clock disabled: deactivate sleep mode (no time reference available)
 * - If clock enabled but not synced: enforce sleep mode when enabled (fail-safe)
 * - If clock available and synced: check if current time is within configured sleep window
 *   - Handles midnight wraparound (e.g., 22:00 to 06:00)
 *   - Updates sleepModeActive boolean accordingly
 */
void updateSleepModeState() {
  // If sleep mode feature is disabled, ensure sleep mode is deactivated
  if (!sleepModeEnabled) {
    if (sleepModeActive) {
      sleepModeActive = false;
      PRINTS("\nSleep Mode DEACTIVATED (feature disabled)");
    }
    return;
  }

  // If clock is disabled, sleep mode cannot function (no time reference)
  // Disable sleep mode to keep display on
  if (!clockEnabled) {
    if (sleepModeActive) {
      sleepModeActive = false;
      PRINTS("\nSleep Mode DEACTIVATED (clock disabled - no time reference)");
    }
    return;
  }

  // Safety-first approach: If clock is enabled but not synced, enforce sleep mode
  // This prevents display from staying on when we can't determine actual time
  if (!clockNtpSynced) {
    if (sleepModeActive != sleepModeEnabled) {
      sleepModeActive = sleepModeEnabled;
      if (sleepModeActive) {
        PRINTS("\nSleep Mode ACTIVATED (NTP not synced - fail-safe mode)");
      }
    }
    return;
  }

  // Validate time string format (should contain colon separator)
  if (strchr(sleepModeConfig.onTime, ':') == NULL ||
      strchr(sleepModeConfig.offTime, ':') == NULL) {
    PRINTS("\nSleep Mode: Invalid time format - enforcing sleep mode");
    sleepModeActive = true;
    return;
  }

  // Determine which times to use (weekday vs weekend)
  const char* activeOnTime = sleepModeConfig.onTime;
  const char* activeOffTime = sleepModeConfig.offTime;
  
  if (strcmp(sleepModeConfig.weekendEnabled, "on") == 0) {
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    int wday = timeinfo->tm_wday; // 0=Sun, 1=Mon, ..., 6=Sat
    
    // If today is Saturday (6) or Sunday (0), use weekend times
    if (wday == 0 || wday == 6) {
      activeOnTime = sleepModeConfig.weekendOnTime;
      activeOffTime = sleepModeConfig.weekendOffTime;
    }
  }

  // Parse configured on/off times
  int onHour = 0, onMin = 0, offHour = 0, offMin = 0;
  int parsedOn = sscanf(activeOnTime, "%d:%d", &onHour, &onMin);
  int parsedOff = sscanf(activeOffTime, "%d:%d", &offHour, &offMin);

  // Validate parsing and bounds
  if (parsedOn != 2 || parsedOff != 2 ||
      onHour < 0 || onHour > 23 || onMin < 0 || onMin > 59 ||
      offHour < 0 || offHour > 23 || offMin < 0 || offMin > 59) {
    PRINTS("\nSleep Mode: Invalid time values - enforcing sleep mode");
    sleepModeActive = true;
    return;
  }

  // Get current time from NTP
  int currentHour = 0, currentMinute = 0, currentSecond = 0;
  getNtpTime(currentHour, currentMinute, currentSecond);

  // Convert times to minutes since midnight for easier comparison
  int currentMinutes = currentHour * 60 + currentMinute;
  int onMinutes = onHour * 60 + onMin;
  int offMinutes = offHour * 60 + offMin;

  // Determine if currently in sleep window
  bool shouldBeActive = false;

  if (onMinutes < offMinutes) {
    // Normal same-day range (e.g., 08:00 to 17:00)
    // Active = current time is between on and off
    shouldBeActive = (currentMinutes >= onMinutes && currentMinutes < offMinutes);
  } else {
    // Crosses midnight (e.g., 22:00 to 06:00)
    // Active = current time is after on OR before off
    shouldBeActive = (currentMinutes >= onMinutes || currentMinutes < offMinutes);
  }

  // Update state and log transitions
  if (sleepModeActive != shouldBeActive) {
    sleepModeActive = shouldBeActive;

    if (sleepModeActive) {
      PRINTS("\nSleep Mode ACTIVATED at ");
      PRINT("", currentHour);
      PRINTS(":");
      if (currentMinute < 10) PRINTS("0");
      PRINT("", currentMinute);
      PRINTS(" (window: ");
      PRINT("", activeOnTime);
      PRINTS(" to ");
      PRINT("", activeOffTime);
      PRINTS(")");
    } else {
      PRINTS("\nSleep Mode DEACTIVATED at ");
      PRINT("", currentHour);
      PRINTS(":");
      if (currentMinute < 10) PRINTS("0");
      PRINT("", currentMinute);
      PRINTS("");
    }

    // Publish all sleep mode states to MQTT when state changes
    publishSleepModeState();
  }
}
#endif

#ifndef DISABLE_ALARM_FEATURE
// ============================================================================
// ALARM CLOCK FUNCTIONS
// ============================================================================

/**
 * Returns whether an alarm is currently active
 */
bool isAlarmActive() {
  return alarmActive;
}

/**
 * Stop all active alarms immediately
 */
void stopAllAlarms() {
  if (alarmActive) {
    PRINTS("\nStopping all alarms");
    alarmActive = false;
    activeAlarmIndex = -1;
    alarmRepeatCounter = 0;
    alarmTotalRepeats = 0;
    alarmDisplayMessage[0] = '\0';

    // Return to appropriate display mode
    if (currentDisplayMode == MODE_ALARM || currentDisplayMode == MODE_ALARM_EXIT) {
      P.displayClear();
      if (clockEnabled && !newMessageAvailable && curMessage[0] == '\0') {
        currentDisplayMode = MODE_CLOCK;
        lastClockUpdate = millis();
        lastColonToggle = millis();
        clockColonVisible = true;
        displayClock(true);
        PRINTS("\nAlarm stopped, returning to clock");
      } else {
        currentDisplayMode = MODE_MESSAGE;
        PRINTS("\nAlarm stopped, returning to message mode");
      }
    }
  }
}

/**
 * Trigger a specific alarm
 */
void triggerAlarm(int alarmIndex) {
  if (alarmIndex < 0 || alarmIndex >= MAX_ALARMS) return;
  if (alarmActive) return; // Already an alarm active

  #ifndef DISABLE_SLEEP_MODE_FEATURE
  // Don't trigger alarms during sleep mode
  if (isSleepModeActive()) {
    PRINTS("\nAlarm trigger blocked - sleep mode active");
    return;
  }
  #endif

  PRINTS("\n=== ALARM TRIGGERED ===");
  PRINT("\nAlarm index: ", alarmIndex);
  PRINT("\nAlarm message: ", alarmConfig.alarms[alarmIndex].message);
  PRINT("\nChirp: ", alarmConfig.alarms[alarmIndex].chirpName);
  PRINT("\nRepeats: ", alarmConfig.alarms[alarmIndex].repeatCount);

  alarmActive = true;
  activeAlarmIndex = alarmIndex;
  alarmStartTime = millis();
  alarmRepeatCounter = 0;
  alarmTotalRepeats = atoi(alarmConfig.alarms[alarmIndex].repeatCount);
  if (alarmTotalRepeats < 1) alarmTotalRepeats = 1;
  if (alarmTotalRepeats > 20) alarmTotalRepeats = 20;
  alarmMessageShowing = true;
  lastAlarmToggle = millis();

  // Build display message
  snprintf(alarmDisplayMessage, sizeof(alarmDisplayMessage), "ALARM: %.128s",
           alarmConfig.alarms[alarmIndex].message);

  // Play chirp sound immediately when alarm triggers (play once)
  if (strcmp(generalConfig.buzzerEnable, "on") == 0) {
    playChirpByName(alarmConfig.alarms[alarmIndex].chirpName, 1);
  }

  // Transition display to alarm mode
  currentDisplayMode = MODE_ALARM;
  P.displayClear();
  displayAlarm(true);
}

/**
 * Display alarm message on LED matrix
 */
void displayAlarm(bool withAnimation) {
  #ifndef DISABLE_SLEEP_MODE_FEATURE
  if (isSleepModeActive() && !sleepModeMuteOnly) {
    P.displayClear();
    return;
  }
  #endif

  if (!alarmActive || alarmDisplayMessage[0] == '\0') return;

  P.setFont(nullptr);

  // Use clock brightness for alarm display
  P.setIntensity(getEffectiveBrightness(clockBrightness));

  if (withAnimation) {
    P.displayClear();
    // Use scroll effect for alarm message
    P.displayText(alarmDisplayMessage, PA_CENTER, scrollDelay, PAUSE_TIME, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    P.displayReset();
  } else {
    // Continue scrolling
    P.displayAnimate();
  }
}

/**
 * Check if any alarms should trigger based on current time
 * Called every minute from main loop
 */
void checkAlarmTriggers() {
  if (!alarmMasterEnabled) return;
  if (!clockEnabled) return;
  if (!clockNtpSynced) return;
  if (alarmActive) return; // Already an alarm active

  #ifndef DISABLE_SLEEP_MODE_FEATURE
  if (isSleepModeActive()) return;
  #endif

  // Get current time
  time_t now = time(nullptr);
  struct tm* timeinfo = localtime(&now);

  int currentHour = timeinfo->tm_hour;
  int currentMinute = timeinfo->tm_min;

  // Convert tm_wday (0=Sunday) to our format (1=Monday, 7=Sunday)
  int currentDayOfWeek = timeinfo->tm_wday;
  int dayNumber = (currentDayOfWeek == 0) ? 7 : currentDayOfWeek;
  char dayChar = '0' + dayNumber;

  // Check each alarm
  for (int i = 0; i < MAX_ALARMS; i++) {
    // Skip disabled alarms
    if (strcmp(alarmConfig.alarms[i].enabled, "on") != 0) continue;

    // Parse alarm time
    int alarmHour = 0, alarmMinute = 0;
    if (sscanf(alarmConfig.alarms[i].time, "%d:%d", &alarmHour, &alarmMinute) != 2) continue;

    // Check if time matches
    if (alarmHour != currentHour || alarmMinute != currentMinute) continue;

    // Check if already triggered this minute
    if (lastTriggeredAlarmMinute[i] == currentMinute) continue;

    // Check day of week (empty = every day)
    const char* days = alarmConfig.alarms[i].daysOfWeek;
    bool dayMatches = (strlen(days) == 0) || (strchr(days, dayChar) != NULL);

    if (!dayMatches) continue;

    // All conditions met - trigger the alarm
    lastTriggeredAlarmMinute[i] = currentMinute;
    triggerAlarm(i);
    break; // Only trigger one alarm at a time
  }

  // Reset triggered minute tracking when minute changes
  static int lastCheckedMinute = -1;
  if (currentMinute != lastCheckedMinute) {
    for (int i = 0; i < MAX_ALARMS; i++) {
      // Reset tracking for alarms whose minute has passed
      if (lastTriggeredAlarmMinute[i] != currentMinute) {
        lastTriggeredAlarmMinute[i] = -1;
      }
    }
    lastCheckedMinute = currentMinute;
  }
}

/**
 * Update alarm state machine - called from main loop
 * Handles alarm display, sound playback, and repeat logic
 */
void updateAlarms() {
  // Handle MODE_ALARM_EXIT even when alarmActive is false
  // (we set alarmActive=false when transitioning to exit mode)
  if (currentDisplayMode == MODE_ALARM_EXIT) {
    // Transition out of alarm mode
    P.displayClear();

    if (clockEnabled && !newMessageAvailable && curMessage[0] == '\0') {
      // Return to clock
      currentDisplayMode = MODE_CLOCK;
      lastClockUpdate = millis();
      lastColonToggle = millis();
      clockColonVisible = true;
      displayClock(true);
      PRINTS("\nAlarm exit complete, returning to clock");
    } else {
      // Return to message mode
      currentDisplayMode = MODE_MESSAGE;
      PRINTS("\nAlarm exit complete, returning to message mode");
    }
    return;
  }

  if (!alarmActive) return;

  #ifndef DISABLE_SLEEP_MODE_FEATURE
  // If sleep mode became active, stop the alarm
  if (isSleepModeActive()) {
    stopAllAlarms();
    return;
  }
  #endif

  // Handle alarm state
  if (currentDisplayMode == MODE_ALARM) {
    // Check if message finished scrolling
    if (P.displayAnimate()) {
      // One repeat completed
      alarmRepeatCounter++;

      PRINT("\nAlarm repeat ", alarmRepeatCounter);
      PRINT(" of ", alarmTotalRepeats);

      if (alarmRepeatCounter >= alarmTotalRepeats) {
        // All repeats done - exit alarm mode
        PRINTS("\nAlarm complete");
        currentDisplayMode = MODE_ALARM_EXIT;
        alarmActive = false;
        activeAlarmIndex = -1;
        P.displayClear();
      } else {
        // Play chirp sound between repeats (play once)
        if (strcmp(generalConfig.buzzerEnable, "on") == 0) {
          playChirpByName(alarmConfig.alarms[activeAlarmIndex].chirpName, 1);
        }

        // Start next scroll
        displayAlarm(true);
      }
    }
  }
}

/**
 * Get alarm status as JSON string
 */
String getAlarmStatusJson() {
  JsonDocument doc;

  doc["active"] = alarmActive;
  doc["activeIndex"] = activeAlarmIndex;
  doc["masterEnabled"] = alarmMasterEnabled;
  doc["repeatCounter"] = alarmRepeatCounter;
  doc["totalRepeats"] = alarmTotalRepeats;

  if (alarmActive && activeAlarmIndex >= 0) {
    doc["activeMessage"] = alarmConfig.alarms[activeAlarmIndex].message;
  }

  // Find next alarm
  String nextAlarmStr = "";
  if (alarmMasterEnabled && clockEnabled && clockNtpSynced) {
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    int currentMinutes = timeinfo->tm_hour * 60 + timeinfo->tm_min;
    int currentDay = (timeinfo->tm_wday == 0) ? 7 : timeinfo->tm_wday;

    int minDiff = 7 * 24 * 60 + 1; // More than a week
    int nextIndex = -1;

    for (int i = 0; i < MAX_ALARMS; i++) {
      if (strcmp(alarmConfig.alarms[i].enabled, "on") != 0) continue;

      int h = 0, m = 0;
      if (sscanf(alarmConfig.alarms[i].time, "%d:%d", &h, &m) != 2) continue;
      int alarmMinutes = h * 60 + m;

      const char* days = alarmConfig.alarms[i].daysOfWeek;

      // Check today and following days
      for (int dayOffset = 0; dayOffset < 8; dayOffset++) {
        int checkDay = ((currentDay - 1 + dayOffset) % 7) + 1;
        char dayChar = '0' + checkDay;

        bool dayMatches = (strlen(days) == 0) || (strchr(days, dayChar) != NULL);
        if (!dayMatches) continue;

        int diff;
        if (dayOffset == 0) {
          // Today
          if (alarmMinutes > currentMinutes) {
            diff = alarmMinutes - currentMinutes;
          } else {
            continue; // Already passed today
          }
        } else {
          diff = dayOffset * 24 * 60 + alarmMinutes - currentMinutes;
          if (diff < 0) diff += 24 * 60;
        }

        if (diff < minDiff) {
          minDiff = diff;
          nextIndex = i;
        }
        break; // Found for this alarm, move to next
      }
    }

    if (nextIndex >= 0) {
      nextAlarmStr = alarmConfig.alarms[nextIndex].time;
    }
  }
  doc["nextAlarm"] = nextAlarmStr;

  String output;
  serializeJson(doc, output);
  return output;
}

/**
 * Get alarm configuration as JSON string
 */
String getAlarmConfigJson() {
  JsonDocument doc;

  doc["masterEnabled"] = alarmConfig.masterEnabled;

  JsonArray alarmsArray = doc["alarms"].to<JsonArray>();
  for (int i = 0; i < MAX_ALARMS; i++) {
    JsonObject alarm = alarmsArray.add<JsonObject>();
    alarm["enabled"] = alarmConfig.alarms[i].enabled;
    alarm["time"] = alarmConfig.alarms[i].time;
    alarm["daysOfWeek"] = alarmConfig.alarms[i].daysOfWeek;
    alarm["message"] = alarmConfig.alarms[i].message;
    alarm["chirpName"] = alarmConfig.alarms[i].chirpName;
    alarm["repeatCount"] = alarmConfig.alarms[i].repeatCount;
  }

  String output;
  serializeJson(doc, output);
  return output;
}

/**
 * Get list of available chirp patterns as JSON
 */
String getChirpsJson() {
  JsonDocument doc;
  JsonArray arr = doc.to<JsonArray>();

  int count = getChirpCount();
  for (int i = 0; i < count; i++) {
    const ChirpPattern* p = getChirpByIndex(i);
    if (p) {
      JsonObject chirp = arr.add<JsonObject>();
      chirp["name"] = p->name;
      chirp["description"] = p->description;
    }
  }

  String output;
  serializeJson(doc, output);
  return output;
}

// ============================================================================
// RECURRENT ALARM FUNCTIONS
// ============================================================================

/**
 * Convert interval string to minutes
 * Returns: 15, 30, 60, 180, 360, or -1 for midday
 */
int getIntervalMinutes(const char* interval) {
  if (strcmp(interval, "15min") == 0) return 15;
  if (strcmp(interval, "30min") == 0) return 30;
  if (strcmp(interval, "1hour") == 0) return 60;
  if (strcmp(interval, "3hours") == 0) return 180;
  if (strcmp(interval, "6hours") == 0) return 360;
  if (strcmp(interval, "midday") == 0) return -1; // Special value for midday trigger
  return 15; // Default
}

/**
 * Trigger the recurrent alarm - play chirp sound once
 * Respects global buzzer enable and sleep mode settings
 */
void triggerRecurrentAlarm() {
  // Check if buzzer is enabled globally
  if (strcmp(generalConfig.buzzerEnable, "on") != 0) {
    PRINTS("\nRecurrent alarm sound blocked: buzzer disabled globally");
    return;
  }

  #ifndef DISABLE_SLEEP_MODE_FEATURE
  // Check if sleep mode is active
  if (isSleepModeActive()) {
    PRINTS("\nRecurrent alarm sound blocked: sleep mode active");
    return;
  }
  #endif

  PRINTS("\n=== RECURRENT ALARM TRIGGERED ===");
  PRINT("\nChirp: ", recurrentAlarmConfig.chirpName);
  PRINT("\nInterval: ", recurrentAlarmConfig.interval);

  // Play the selected chirp sound once
  playChirpByName(recurrentAlarmConfig.chirpName, 1);

  // Update last trigger time
  lastRecurrentAlarmTrigger = millis();
}

/**
 * Check if it's time to trigger the recurrent alarm
 * Called from main loop
 * Uses wall-clock boundaries for interval-based triggers to ensure alignment with time slots
 */
void checkRecurrentAlarm() {
  // Skip if not enabled
  if (!recurrentAlarmEnabled) return;

  // Skip if clock is not enabled or not synced (need time reference)
  if (!clockEnabled || !clockNtpSynced) return;

  // Get current time
  time_t now = time(nullptr);
  struct tm* timeinfo = localtime(&now);

  int currentHour = timeinfo->tm_hour;
  int currentMinute = timeinfo->tm_min;
  int currentSecond = timeinfo->tm_sec;
  int currentDay = timeinfo->tm_yday;
  int currentDayOfWeek = timeinfo->tm_wday; // 0=Sun, 1=Mon, ..., 6=Sat

  // Check if weekend (Saturday=6 or Sunday=0) and disable weekends is enabled
  if (recurrentAlarmDisableWeekends && (currentDayOfWeek == 0 || currentDayOfWeek == 6)) {
    return; // Skip triggering on weekends
  }

  // Handle midday special case
  if (recurrentAlarmIntervalMinutes == -1) {
    // Check if it's exactly 12:00:00 and we haven't triggered today
    static int lastTriggeredDay = -1;
    if (currentHour == 12 && currentMinute == 0 && currentSecond < 5) {
      if (currentDay != lastTriggeredDay) {
        lastTriggeredDay = currentDay;
        triggerRecurrentAlarm();
      }
    }
    return;
  }

  // For interval-based triggers, use wall-clock boundaries
  // Fire when minute % interval == 0 and second == 0
  static int lastTriggeredMinute = -1;
  static int lastTriggeredDay = -1;

  // Check if we're at a trigger boundary (minute aligns with interval, second is 0)
  if (currentMinute % recurrentAlarmIntervalMinutes == 0 && currentSecond == 0) {
    // Guard against multiple firings in the same minute
    if (currentMinute != lastTriggeredMinute || currentDay != lastTriggeredDay) {
      lastTriggeredMinute = currentMinute;
      lastTriggeredDay = currentDay;
      triggerRecurrentAlarm();
    }
  }
}
#endif // DISABLE_ALARM_FEATURE