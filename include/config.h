#ifndef CONFIG_H
#define CONFIG_H

// Platform verification
#if !defined(ESP8266) && !defined(ESP32)
  #error "Neither ESP8266 nor ESP32 platform defined"
#endif
#if defined(ESP8266) && defined(ESP32)
  #error "Both ESP8266 and ESP32 platforms defined"
#endif

// Flash Button Settings (defined early for conditional includes)
#define ENABLE_FLASH_BUTTON 0
#if ENABLE_FLASH_BUTTON
  #define FLASH_BUTTON 0
#endif

#include <Arduino.h>
// Define hardware SPI interface to suppress library warning
#define USE_HARDWARE_SPI
#include <ArduinoJson.h>
// Platform-specific web server and mDNS
#ifdef ESP8266
  #include <ESP8266WebServer.h>
  #include <ESP8266mDNS.h>
#elif defined(ESP32)
  #include <WebServer.h>
  #include <ESPmDNS.h>
  #include <WiFiClientSecure.h>  // For TLS/SSL support
#endif
#if ENABLE_FLASH_BUTTON
  #include <EasyButton.h>
#endif
#include <LittleFS.h>
#include <MD_MAX72xx.h>
#include <MD_Parola.h>
#include <PubSubClient.h>
#include <SPI.h>
#ifndef WM_DEBUG_LEVEL
  #define WM_DEBUG_LEVEL 0
#endif
#include <WiFiManager.h>
#include <time.h>

// Platform-specific type abstractions
#ifdef ESP8266
  typedef ESP8266WebServer PlatformWebServer;
#elif defined(ESP32)
  typedef WebServer PlatformWebServer;
#endif

// Platform-specific chip ID extraction
#ifdef ESP8266
  inline String getUniqueChipId() {
    return String(ESP.getChipId(), HEX);
  }
#elif defined(ESP32)
  inline String getUniqueChipId() {
    uint64_t mac = ESP.getEfuseMac();
    uint32_t chipId = 0;
    for(int i=0; i<17; i=i+8) {
      chipId |= ((mac >> (40 - i)) & 0xff) << i;
    }
    return String(chipId, HEX);
  }
#endif

#include <stdio.h>

// Version
#ifndef VERSION
  #define VERSION "DEMO"
#endif

// Hardware Configuration
#ifndef MAX_DEVICES
  #define MAX_DEVICES 4  // Number of MAX7219 modules (can override via -DMAX_DEVICES=8)
#endif
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW

// Pin Configuration (platform-specific defaults, overridable via build flags)
#ifndef CLK_PIN
  #ifdef ESP8266
    #define CLK_PIN D5          // GPIO 14
  #elif defined(ESP32)
    #define CLK_PIN 18          // VSPI CLK
  #endif
#endif
#ifndef DATA_PIN
  #ifdef ESP8266
    #define DATA_PIN D7         // GPIO 13
  #elif defined(ESP32)
    #define DATA_PIN 23         // VSPI MOSI
  #endif
#endif
#ifndef CS_PIN
  #ifdef ESP8266
    #define CS_PIN D8           // GPIO 15
  #elif defined(ESP32)
    #define CS_PIN 5            // VSPI CS
  #endif
#endif
#ifndef BUZZER_PIN
  #ifdef ESP8266
    #define BUZZER_PIN D1       // GPIO 5
  #elif defined(ESP32)
    #define BUZZER_PIN 4        // General purpose GPIO
  #endif
#endif
// Legacy alias for backward compatibility
#define BUZZER BUZZER_PIN

// WiFiManager Settings
#define WM_MDNS 1
// Debug level is now defined before including WiFiManager.h

// Debug Settings
#ifndef DEBUG
#define DEBUG 0
#endif
#define PRINT_CALLBACK 0
#define PAUSE_TIME 0

// Server Settings
#define HTTP_PORT 80

// Default Web Credentials
#define DEFAULT_WEB_USER "admin"
#define DEFAULT_WEB_PASS "msgboard"

// Buffer Sizes

#ifdef ESP8266
  #define STDSIZE 64          // Reduced from 128 to save RAM
  #define MSG_SIZE 1024       // Reduced from 3000 to save RAM
  #define MSG_JSON_SIZE 1024  // Reduced to 1024 to save RAM
#else
  #define STDSIZE 128
  #define MSG_SIZE 3072
  #define MSG_JSON_SIZE 3072
#endif
#define REP_SIZE 12  // Increased to handle max int (10 digits + null + margin)
#define BUZ_SIZE 12
#define DEL_SIZE 12
#define BRI_SIZE 12
#define ASC_SIZE 12

// Tightly-sized constants for fields with fixed, bounded vocabularies.
// Using STDSIZE for these is wasteful — the actual data never exceeds these sizes.
#define FLAG_SIZE       8   // "on" / "off" (3 chars + null, 4 margin)
#define TIME_SIZE       8   // "HH:MM" (5 chars + null, 2 margin)
#define CHIRP_SIZE      24  // chirp pattern names — longest is "Close Encounters" (16 chars)
#define TIMER_MODE_SIZE 16  // "countdown" / "stopwatch" (9 chars + null, 6 margin)
#define INTERVAL_SIZE   12  // "15min" / "30min" / "1hour" / "3hours" / "midday" (max 6 chars)

// Debug Macros
#if DEBUG
#define PRINT(s, v)                                                            \
  {                                                                            \
    Serial.print(F(s));                                                        \
    Serial.print(v);                                                           \
  }
#define PRINTS(s)                                                              \
  {                                                                            \
    Serial.print(F(s));                                                        \
  }
#else
#define PRINT(s, v)
#define PRINTS(s)
#endif

// Message Structure
typedef struct {
  uint8_t spacing;
  char *msg;
} msgDef_t;

// Config Structures
struct webConfigObj {
  char usernameWebHolder[STDSIZE];
  char passwordWebHolder[STDSIZE];
  char hostnameWebHolder[STDSIZE];
};

struct mqttConfigObj {
  char onOffMqttHolder[FLAG_SIZE];         // "on" or "off"
  char anonymousMqttHolder[FLAG_SIZE];     // "on" or "off"
  char alertMqttHolder[FLAG_SIZE];         // "on" or "off"
  char usernameMqttHolder[STDSIZE];        // user credential (keep large)
  char passwordMqttHolder[STDSIZE];        // user credential (keep large)
  char serverAddressMqttHolder[STDSIZE];   // hostname or IP (keep large)
  char serverPortMqttHolder[REP_SIZE];     // "1883" (max 5 digits)
  char topicPrefixMqttHolder[STDSIZE];     // user-defined topic prefix (keep large)
  char haDiscoveryMqttHolder[FLAG_SIZE];   // "on" or "off"
  char mqttMessagesEnableHolder[FLAG_SIZE]; // "on" or "off"
  char tlsEnabledMqttHolder[FLAG_SIZE];    // "on" or "off"
};

// MQTT Discovery Settings
#define MQTT_DISCOVERY_PREFIX "homeassistant"

struct defaultsConfigObj {
  char repeatDefaultHolder[REP_SIZE];
  char buzzerDefaultHolder[BUZ_SIZE];
  char scrollDelayDefaultHolder[DEL_SIZE];
  char brightnessDefaultHolder[BRI_SIZE];
  char alertChirpDefaultHolder[CHIRP_SIZE]; // Default chirp pattern name for message alerts
};

struct generalConfigObj {
  char buzzerEnable[FLAG_SIZE];            // Global Buzzer Enable ("on" or "off")
  char brightnessOverrideEnable[FLAG_SIZE]; // Brightness Override Enable ("on" or "off")
  char brightnessOverrideValue[BRI_SIZE];  // Brightness Override Value ("0" to "15")
};

// Date format buffer size (for strftime output)
#define DATE_FORMAT_SIZE 32

// Default TZ strings
#define DEFAULT_TZ_STRING "UTC0"

// Clock Configuration
struct clockConfigObj {
  char enabled[FLAG_SIZE];            // "on" or "off"
  char ntpServer[STDSIZE];            // "pool.ntp.org" or IP (user text, keep large)
  char tzString[STDSIZE];             // POSIX TZ string, e.g. "EST5EDT,M3.2.0,M11.1.0" (keep large)
  char brightness[BRI_SIZE];          // "0" to "15"
  char transitionDelayMs[REP_SIZE];   // "2000" (milliseconds)
  char transitionEffect[FLAG_SIZE];   // "FADE", "WIPE", "GROW", "SCROLL", "PRINT" (max 6 chars)
  char randomizeTransition[FLAG_SIZE]; // "on" or "off"
  char transitionSpeed[REP_SIZE];     // "40" (milliseconds)
  char resyncIntervalHours[REP_SIZE]; // "6" (hours)
  char dateFormat[TIMER_MODE_SIZE];   // "TIME_ONLY", "TIME_SECONDS" etc. (max 14 chars)
  char dateAlternate[FLAG_SIZE];      // "on" / "off" — rotate through date displays
  char dateAlternateSeconds[REP_SIZE]; // "5" (seconds per step)
  char customDateFormat[STDSIZE];     // Custom strftime format string (user text, keep large)
  char clockFace[TIMER_MODE_SIZE];    // "DEFAULT", "MATRIX_LIGHT", "MATRIX_LIGHT_6"
  char clockAmPm[FLAG_SIZE];          // "on" / "off" — 12-hour AM/PM mode
};

#ifndef DISABLE_TIMER_FEATURE
// Timer Configuration
struct timerConfigObj {
  char enabled[FLAG_SIZE];            // "on" or "off"
  char mode[TIMER_MODE_SIZE];         // "countdown" or "stopwatch"
  char durationSeconds[REP_SIZE];     // Total countdown duration in seconds (e.g. "300")
  char brightness[BRI_SIZE];          // "0" to "15"
  char alertBuzzer[FLAG_SIZE];        // "on" or "off" - sound when timer completes
  char alertBuzzerCount[BUZ_SIZE];    // Number of beeps on completion (deprecated, use alertChirp)
  char autoRepeat[FLAG_SIZE];         // "on" or "off" - restart after completion
  char alertChirp[CHIRP_SIZE];        // Chirp pattern name for timer completion alert
};
#endif

#ifndef DISABLE_SLEEP_MODE_FEATURE
// Sleep Mode Configuration
struct sleepModeConfigObj {
  char enabled[FLAG_SIZE];            // "on" or "off"
  char onTime[TIME_SIZE];             // "HH:MM" format (e.g., "22:00")
  char offTime[TIME_SIZE];            // "HH:MM" format (e.g., "06:00")
  char muteOnly[FLAG_SIZE];           // "on" or "off" - if "on", only mute sounds, keep LEDs active
  char weekendEnabled[FLAG_SIZE];     // "on" or "off"
  char weekendOnTime[TIME_SIZE];      // "HH:MM"
  char weekendOffTime[TIME_SIZE];     // "HH:MM"
};
#endif

#ifndef DISABLE_ALARM_FEATURE
// Alarm Configuration
#ifdef ESP8266
  #define MAX_ALARMS 1
#else
  #define MAX_ALARMS 10
#endif
#define ALARM_MESSAGE_SIZE 128

struct alarmItemObj {
  char enabled[FLAG_SIZE];            // "on" or "off" for this specific alarm
  char time[TIME_SIZE];               // "HH:MM" format (24-hour)
  char daysOfWeek[8];                 // "1234567" where 1=Monday, 7=Sunday, empty=daily
  char message[ALARM_MESSAGE_SIZE];   // Message to display on LED
  char chirpName[CHIRP_SIZE];         // Chirp pattern name from chirp_library
  char repeatCount[BUZ_SIZE];         // Number of times to repeat sound+message
};

struct alarmConfigObj {
  char masterEnabled[FLAG_SIZE];      // "on" or "off" - master switch for all alarms
  alarmItemObj alarms[MAX_ALARMS];    // Array of up to 1 alarm (ESP8266) / 10 (ESP32)
};

// Recurrent Alarm Configuration
struct recurrentAlarmConfigObj {
  char enabled[FLAG_SIZE];            // "on" or "off"
  char chirpName[CHIRP_SIZE];         // Chirp pattern name from chirp_library
  char interval[INTERVAL_SIZE];       // "15min", "30min", "1hour", "3hours", "6hours", "midday"
  char disableWeekends[FLAG_SIZE];    // "on" or "off" - disable alarm on Saturday and Sunday
};
#endif

#ifndef DISABLE_WEATHER_FEATURE
// Weather Configuration
#define WEATHER_LOCATION_SIZE 64
#define WEATHER_COORD_SIZE 16
#define WEATHER_TEMP_SIZE 32
#define WEATHER_CONDITION_SIZE 64
#define WEATHER_FORECAST_SIZE 128

struct weatherConfigObj {
  char enabled[FLAG_SIZE];            // "on" or "off"
  char location[WEATHER_LOCATION_SIZE]; // City name or description (user text, keep large)
  char latitude[WEATHER_COORD_SIZE];  // Decimal latitude (e.g., "40.7128")
  char longitude[WEATHER_COORD_SIZE]; // Decimal longitude (e.g., "-74.0060")
  char updateIntervalMinutes[REP_SIZE]; // How often to fetch weather (e.g., "30")
  char temperatureUnit[FLAG_SIZE];    // "C" for Celsius, "F" for Fahrenheit
  char brightness[BRI_SIZE];          // "0" to "15"
  char displayDurationSeconds[REP_SIZE]; // How long to show weather before returning to clock
};
#endif

// Display Mode States
enum DisplayMode {
  MODE_MESSAGE,     // Displaying scrolling message
  MODE_TRANSITION,  // Transitioning from message to clock
  MODE_CLOCK,       // Displaying clock
  MODE_CLOCK_EXIT   // Animating clock exit before message
#ifndef DISABLE_TIMER_FEATURE
  ,
  MODE_TIMER,       // Displaying timer
  MODE_TIMER_EXIT   // Animating timer exit before message/clock
#endif
#ifndef DISABLE_ALARM_FEATURE
  ,
  MODE_ALARM,       // Displaying alarm
  MODE_ALARM_EXIT   // Animating alarm exit
#endif
#ifndef DISABLE_WEATHER_FEATURE
  ,
  MODE_WEATHER,     // Displaying weather
  MODE_WEATHER_EXIT // Animating weather exit
#endif
};

#endif // CONFIG_H
