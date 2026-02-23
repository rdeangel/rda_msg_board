#ifndef GLOBALS_H
#define GLOBALS_H

#include "config.h"

// Global Objects
extern MD_Parola P;
#if ENABLE_FLASH_BUTTON
extern EasyButton flash_button;
#endif

extern PlatformWebServer serverHttp;

// Platform-specific WiFi clients
#ifdef ESP8266
  extern WiFiClient espClient;
#elif defined(ESP32)
  extern WiFiClientSecure espClientSecure;  // For TLS connections
  extern WiFiClient espClientPlain;         // For plain connections
#endif

extern PubSubClient mqttClient;

// Client ID / Hostname
extern String clientIdPrefix;
extern String chipId;
extern String clientId;

// Web Authentication
extern char web_username[STDSIZE];
extern char web_password[STDSIZE];
extern const char *www_realm;
extern String authFailResponse;

// Config Files
extern const char *webConfigFile;
extern const char *mqttConfigFile;

// Config Objects
extern webConfigObj webConfig;
extern mqttConfigObj mqttConfig;
extern defaultsConfigObj defaultsConfig;
extern generalConfigObj generalConfig;
extern clockConfigObj clockConfig;

// Config Files
extern const char *defaultsConfigFile;
extern const char *generalConfigFile;
extern const char *clockConfigFile;

// AP Mode
extern char ap_mode_ssid[64];
extern const char *ap_mode_password;

// MQTT Settings
extern char mqttOnOff[STDSIZE];
extern char mqttAnonymous[STDSIZE];
extern char mqttAlert[STDSIZE];
extern char mqttUsername[STDSIZE];
extern char mqttPassword[STDSIZE];
extern char mqttServerAddress[STDSIZE];
extern char mqttServerPort[STDSIZE];
extern char mqttTopicPrefix[STDSIZE];
extern char mqttTopicDevice[STDSIZE];
extern char mqttTopicRoot[STDSIZE];
extern const unsigned long mqttConnectTimeIntervall;
extern long mqttLastReconnectAttempt;
extern int mqttDisconnected;

// MQTT TLS Settings (ESP32 only)
#ifdef ESP32
extern char tlsEnabledMqtt[STDSIZE];
extern bool tlsEnabled;
#endif

// Home Assistant Discovery
extern char mqttHaDiscovery[STDSIZE];
extern bool mqttDiscoveryPublished;

// MQTT Messages Display Toggle
extern char mqttMessagesEnable[STDSIZE];
extern char newMqttMessagesEnable[STDSIZE];
extern bool newMqttMessagesEnableAvailable;
extern char haBaseTopic[256];
extern unsigned long lastTelemetryUpdate;
extern const unsigned long telemetryInterval;

// HA Parameter Memory (RAM-only, initialized from flash defaults on boot)
extern char haLastMessage[MSG_SIZE];
extern int haLastRepeat;
extern int haLastBuzzer;
extern int haLastScrollDelay;
extern int haLastBrightness;
extern int haLastAsciiConv;
extern bool haParametersInitialized;

// Message Arrays
extern msgDef_t M[];

// Page Variables
extern String version;
extern String mainPageVars;
extern String mqttPageVars;
extern String changeCredVars;
extern String updateVars;
extern String clockPageVars;

// Web Config Variables
extern char newWebUsername[STDSIZE];
extern char newWebPassword[STDSIZE];
extern char newWebHostname[STDSIZE];
extern bool newWebUsernameAvailable;
extern bool newWebPasswordAvailable;
extern bool newWebHostnameAvailable;
extern bool saveWebConfigAtStart;

// MQTT Config Variables
extern char newMqttOnOff[STDSIZE];
extern char newMqttAnonymous[STDSIZE];
extern char newMqttAlert[STDSIZE];
extern char newMqttUsername[STDSIZE];
extern char newMqttPassword[STDSIZE];
extern char newMqttServerAddress[STDSIZE];
extern char newMqttServerPort[STDSIZE];
extern char newMqttTopicPrefix[STDSIZE];
extern char newMqttHaDiscovery[STDSIZE];
#ifdef ESP32
extern char newTlsEnabledMqtt[STDSIZE];
#endif
extern bool newMqttOnOffAvailable;
extern bool newMqttAnonymousAvailable;
extern bool newMqttAlertAvailable;
extern bool newMqttUsernameAvailable;
extern bool newMqttPasswordAvailable;
extern bool newMqttServerAddressAvailable;
extern bool newMqttServerPortAvailable;
extern bool newMqttTopicPrefixAvailable;
extern bool newMqttHaDiscoveryAvailable;
#ifdef ESP32
extern bool newTlsEnabledMqttAvailable;
#endif
extern char mqttStatusMsg[64];
extern char mqttAlertMessage[512];
extern bool saveMqttConfigAtStart;
extern bool alertMqttConnect;

// Message Variables
extern char curMessage[MSG_SIZE];
extern char newMessage[MSG_SIZE];
extern char newRepeat[REP_SIZE];
extern char repeatDefault[REP_SIZE];
extern char newBuz[BUZ_SIZE];
extern char buzzerDefault[BUZ_SIZE];
extern char newDelay[DEL_SIZE];
extern char scrollDelayDefault[DEL_SIZE];
extern char newBrightness[BRI_SIZE];
extern char ledBrightnessDefault[BRI_SIZE];
extern char newAsciiConv[ASC_SIZE];
extern char asciiConvDefault[ASC_SIZE];
extern char newAlertChirp[STDSIZE];
extern char alertChirpDefault[STDSIZE];
extern bool newAlertChirpAvailable;

// State Flags
extern bool firstMessage;
extern bool firstMessageOff;
extern bool newMessageAvailable;
extern bool newRepeatAvailable;
extern bool newBuzAvailable;
extern bool explicitBuzzerCount;
extern bool newDelayAvailable;
extern bool newBrightnessAvailable;
extern bool newAsciiConvAvailable;
extern bool endSetupModeMsg;
extern bool afterWiFiConfig;

// Buzzer Global Control
extern char newBuzzerEnable[STDSIZE];
extern bool newBuzzerEnableAvailable;

// Other Variables
extern char assignedIP[255];
extern int scrollDelay;
extern int ledBrightness;
extern int repeatCount;
extern IPAddress apModeIP;
extern String apModeSSID;

// Clock Variables
extern DisplayMode currentDisplayMode;
extern unsigned long transitionStartTime;
extern unsigned long lastClockUpdate;
extern unsigned long lastColonToggle;
extern unsigned long lastNtpResync;
extern bool clockColonVisible;
extern bool clockNtpSynced;
extern bool clockEnabled;
extern int clockBrightness;

// Brightness Override Variables
extern bool brightnessOverrideEnabled;
extern int brightnessOverrideValue;

// Clock date display variables
extern unsigned long lastDateAlternate;
extern bool showingDate;
extern char clockDateBuffer[DATE_FORMAT_SIZE];

#ifndef DISABLE_TIMER_FEATURE
// Timer Configuration
extern timerConfigObj timerConfig;
extern const char *timerConfigFile;

// Timer State Variables
extern bool timerRunning;
extern bool timerPaused;
extern unsigned long timerStartTime;
extern unsigned long timerPausedTime;
extern unsigned long timerElapsedSeconds;
extern unsigned long timerTargetSeconds;
extern unsigned long timerDurationSeconds;
extern int timerMode;
extern bool timerAutoRepeat;
extern bool timerAlertBuzzer;
extern int timerAlertBuzzerCount;
extern bool timerAlertTriggered;
extern bool timerEnabled;
extern int timerBrightness;
extern unsigned long lastTimerUpdate;
extern unsigned long lastTimerDisplayToggle;
extern bool timerShowingSeconds;
#endif // DISABLE_TIMER_FEATURE

#ifndef DISABLE_SLEEP_MODE_FEATURE
// Sleep Mode Configuration
extern sleepModeConfigObj sleepModeConfig;
extern const char *sleepModeConfigFile;

// Sleep Mode State Variables
extern bool sleepModeEnabled;        // Whether sleep mode feature is enabled
extern bool sleepModeActive;         // Whether sleep mode is currently active (based on time)
extern bool sleepModeMuteOnly;       // Whether sleep mode only mutes sounds (keeps LEDs active)
#endif // DISABLE_SLEEP_MODE_FEATURE

#ifndef DISABLE_ALARM_FEATURE
// Alarm Configuration
extern alarmConfigObj alarmConfig;
extern const char *alarmConfigFile;

// Alarm State Variables
extern bool alarmMasterEnabled;
extern bool alarmActive;                    // Currently triggering an alarm
extern int activeAlarmIndex;                // Which alarm is currently active (-1 if none)
extern unsigned long alarmStartTime;        // When the current alarm started
extern int alarmRepeatCounter;              // Current repeat iteration
extern int alarmTotalRepeats;               // Total repeats for current alarm
extern bool alarmMessageShowing;            // Toggle for flashing message
extern unsigned long lastAlarmToggle;       // Last time message toggled
extern unsigned long lastAlarmCheck;        // Last time we checked for alarms
extern char alarmDisplayMessage[ALARM_MESSAGE_SIZE + 16]; // Buffer for alarm message display
extern int lastTriggeredAlarmMinute[MAX_ALARMS]; // Track last triggered minute per alarm to prevent re-triggering

// Recurrent Alarm Configuration
extern recurrentAlarmConfigObj recurrentAlarmConfig;
extern const char *recurrentAlarmConfigFile;

// Recurrent Alarm State Variables
extern bool recurrentAlarmEnabled;
extern unsigned long lastRecurrentAlarmTrigger;
extern int recurrentAlarmIntervalMinutes;  // Converted interval in minutes
#endif // DISABLE_ALARM_FEATURE

#ifndef DISABLE_WEATHER_FEATURE
// Weather Configuration
extern weatherConfigObj weatherConfig;
extern const char *weatherConfigFile;

// Weather State Variables
extern bool weatherEnabled;              // Whether weather feature is enabled
extern int weatherBrightness;            // Brightness for weather display
extern unsigned long lastWeatherFetch;   // Last time weather data was fetched
extern unsigned long weatherDisplayStart; // When weather display started
extern char weatherTemperature[WEATHER_TEMP_SIZE];      // Current temperature string
extern char weatherCondition[WEATHER_CONDITION_SIZE];   // Current condition (e.g., "Sunny")
extern char weatherForecast[WEATHER_FORECAST_SIZE];     // Forecast info (high/low temps)
extern bool weatherDataValid;            // Whether we have valid weather data
extern int weatherCode;                  // Weather condition code from API
extern bool weatherRefreshRequested;     // Flag to request refresh from main loop
#endif // DISABLE_WEATHER_FEATURE

#endif // GLOBALS_H
