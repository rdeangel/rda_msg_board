#include "config.h"
#include "globals.h"
#include "web_pages.h"
#include "mqtt.h"
#include "mqtt_discovery.h"
#include "web_server.h"
#include "functions.h"
#include "config_manager.h"
#ifndef DISABLE_TIMER_FEATURE
#include "timer.h"
#endif
#ifndef DISABLE_WEATHER_FEATURE
#include "weather.h"
#endif
#ifndef DISABLE_CRYPTO_FEATURE
#include "crypto.h"
#endif
#ifdef ESP32
#include "buzzer_task.h"
#include "http_task.h"
#endif

#ifdef ESP8266
#include <sntp.h>
#endif

void setup() {
  // STRICTLY DISABLE DHCP NTP (Must be done before WiFi connects)
  // Note: ESP32 cannot call esp_sntp functions until TCP/IP stack is ready (after WiFi)
  // ESP32 SNTP DHCP disable is handled in initNtpTime() instead
  #ifdef ESP8266
  sntp_servermode_dhcp(0);
  #endif

  clientId = clientIdPrefix + chipId;
  clientId.toUpperCase();
  clientId.toCharArray(mqttTopicDevice, 128);

  // WiFi Config Portal - AP Mode WiFi Details
  // WiFi Config Portal - AP Mode WiFi Details
  strlcpy(ap_mode_ssid, clientId.c_str(), sizeof(ap_mode_ssid));
  ap_mode_password = "wifi-setup";

#if DEBUG
  Serial.begin(115200);
  PRINTS("\n[MD_MAX72XX WiFi Message Display]\nType a message for the scrolling display from your internet browser");
#endif
  Serial.println("");

#if ENABLE_FLASH_BUTTON
  flash_button.begin();
  // Add the callback function to be called when the button is pressed.
  flash_button.onPressed(onPressed);
#endif

  // Initialize LittleFS library
  bool lfsInitialized = false;
  #ifdef ESP32
    // ESP32: begin(true) enables formatIfMountFailed
    lfsInitialized = LittleFS.begin(true);
  #else
    if (LittleFS.begin()) {
      lfsInitialized = true;
    } else {
      Serial.println(F("LittleFS Mount Failed, formatting..."));
      if (LittleFS.format()) {
        lfsInitialized = LittleFS.begin();
      }
    }
  #endif

  if (!lfsInitialized) {
    Serial.println(F("CRITICAL: Failed to initialize LittleFS! Configs will not load."));
  }

  // You can use the remove lines below to delete existing config files, (especially if config is invalid and crasing device)
  //LittleFS.remove(webConfigFile);
  //LittleFS.remove(mqttConfigFile);

  // Load only the configs needed before WiFiManager runs.
  // On ESP8266, heap is tight and WiFiManager's WiFi scan page requires a large
  // contiguous heap block. Deferring feature configs (timer, weather, alarm, sleep,
  // clock) prevents their JsonDocument allocations from fragmenting the heap and
  // exhausting memory before the scan page can be rendered.
  initDefaultsStoreConfig(); // Needed by configModeCallback display (scrollDelay, brightness defaults)
  initGeneralStoreConfig();  // Needed by configModeCallback display (brightness override)

  // Load brightness override settings (used by display during WiFiManager AP mode)
  brightnessOverrideEnabled = (strcmp(generalConfig.brightnessOverrideEnable, "on") == 0);
  brightnessOverrideValue = atoi(generalConfig.brightnessOverrideValue);

  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

#ifdef ESP32
  // Initialize LEDC channel 0 for buzzer (required before ledcWriteTone())
  ledcSetup(0, 5000, 8);  // channel 0, 5000Hz base frequency, 8-bit resolution
  ledcAttachPin(BUZZER, 0);  // Attach buzzer pin to channel 0
  initBuzzerTask();  // Start FreeRTOS buzzer task for non-blocking sound playback
#endif

  // Display initialisation
  P.begin();

  //P.addChar('¬', degC);
  //P.addChar('&', degF);
  //P.addChar('~', waveSine);
  //P.addChar('+', waveSqar);
  //P.addChar('^', waveTrng);

  curMessage[0] = newMessage[0] = '\0';
  
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP

  // WiFiManager - Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;

  //Comment out and edit line below if you want to change AP Config Portal IP from default 192.168.4.1 to something else
  //wm.setAPStaticIPConfig(IPAddress(192,168,100,1), IPAddress(192,168,100,1), IPAddress(255,255,255,0));

  //wm.setSTAStaticIPConfig(IPAddress(192,168,1,92), IPAddress(192,168,1,1), IPAddress(255,255,255,0), IPAddress(192,168,1,1)); // optional DNS 4th argument

  wm.setHostname(clientId);

  //run code before wifi setup is checked/started (callback NO LONGER displays message)
  wm.setAPCallback(configModeCallback);
  wm.setClass("invert"); // dark theme

  //configure WifiManager portal (essentially specifying the button we want on the captive portal page)
  std::vector<const char *> wm_menu  = {"wifi", "info", "update"};
  wm.setShowInfoUpdate(true);
  wm.setShowInfoErase(true);
  wm.setMenu(wm_menu);

  //try to connect for 60 seconds before starting config portal
  wm.setConnectTimeout(60);

  // Refresh AP SSID in case clientId was updated by config load
  strlcpy(ap_mode_ssid, clientId.c_str(), sizeof(ap_mode_ssid));
  
  // BLOCKING: Start AP either in setup mode or connects to configured wifi network
  // WiFiManager automatically falls back to AP mode if:
  // 1. No credentials saved, OR
  // 2. Saved network not reachable (after 60s timeout)
  // The configModeCallback will display setup message in both cases
  bool res = wm.autoConnect(ap_mode_ssid, ap_mode_password);
  if(!res) {
    Serial.println("Failed to connect");
  }
  else {
    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
    firstMessage = true;
    if(afterWiFiConfig){
      ESP.restart();
    }
  }

  // Load remaining configs now that WiFiManager is done.
  // These were deferred to preserve heap for the WiFiManager WiFi scan page on ESP8266.
  initWebStoreConfig();
  initMqttStoreConfig();
  initClockStoreConfig();
#ifndef DISABLE_TIMER_FEATURE
  initTimerStoreConfig();
#endif
#ifndef DISABLE_SLEEP_MODE_FEATURE
  initSleepModeStoreConfig();
  updateSleepModeState(); // Set initial state (fail-safe if NTP not synced)
#endif
#ifndef DISABLE_ALARM_FEATURE
  initAlarmStoreConfig();
  initRecurrentAlarmStoreConfig();
  for(int i=0; i<MAX_ALARMS; i++) {
    lastTriggeredAlarmMinute[i] = -1;
  }
#endif
#ifndef DISABLE_WEATHER_FEATURE
  initWeatherStoreConfig();
#endif
#ifndef DISABLE_CRYPTO_FEATURE
  initCryptoStoreConfig();
#endif
#ifdef ESP32
  #ifndef DISABLE_CRYPTO_FEATURE
  initCryptoTask();
  #endif
  #ifndef DISABLE_WEATHER_FEATURE
  initWeatherTask();
  #endif
#endif

  //show ip address on serial
  sprintf(assignedIP, "%01d.%01d.%01d.%01d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
  PRINT("\nAssigned IP: ", assignedIP);
  PRINTS("\n\n");

  // Initialize NTP time if clock is enabled
  initNtpTime();

  httpWebDirDef();

#ifdef ESP32
  // Start HTTP server on Core 0 so page loads don't block the display loop on Core 1.
  // handleHttpServer() becomes a no-op on ESP32 after this point.
  initHttpTask();
#endif

  //Scroll first message in message mode (blocking to match AP mode behavior)
  sprintf(newMessage, "Wifi Message Mode - Network: %s - IP: %s - http Username: %s - Password: %s - Version: %s", WiFi.SSID().c_str(), assignedIP, web_username, web_password, VERSION);
  startupBuzzer();
  Serial.println("Displaying Connected message (blocking)...");
  displayBlockingMessage(true, 1); // Scroll once, handle HTTP in background

  if (MDNS.begin(clientId)) {
    MDNS.addService("http", "tcp", 80);
    PRINT("\n\nmDNS responder started correctly, name: ", clientId);
  }
  else {
    PRINTS("\n\nError setting up MDNS responder!");
  }

  initMqtt();
}


void loop() {

#ifdef ESP8266
  MDNS.update();
#endif

  handleHttpServer();

  checkMqtt();

  updateTelemetryIfNeeded();  // Periodic Home Assistant sensor updates

  scrollTextParola();

  updateDisplayMode();  // Handle clock/message state transitions

#ifndef DISABLE_TIMER_FEATURE
  updateTimer();  // Update timer state and display
#endif

  checkWiFiAndResync();  // Periodic NTP resync and WiFi reconnection handling

#ifndef DISABLE_SLEEP_MODE_FEATURE
  updateSleepModeState();  // Update sleep mode active state based on time
#endif

#ifndef DISABLE_ALARM_FEATURE
  updateAlarms();  // Update alarm state and display

  // Check for alarm triggers every second (more frequent to not miss minute boundary)
  static unsigned long lastAlarmSecondCheck = 0;
  if (millis() - lastAlarmSecondCheck >= 1000) {
    lastAlarmSecondCheck = millis();
    if (alarmMasterEnabled && clockEnabled && clockNtpSynced) {
      checkAlarmTriggers();
    }
  }

  // Check recurrent alarm
  checkRecurrentAlarm();
#endif

#ifndef DISABLE_WEATHER_FEATURE
  updateWeather();  // Update weather data periodically
#endif

#ifndef DISABLE_CRYPTO_FEATURE
  updateCrypto();  // Update crypto price data periodically
#endif

  #if ENABLE_FLASH_BUTTON
  flash_button.read();
  #endif

}
