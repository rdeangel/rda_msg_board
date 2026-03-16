#include "globals.h"
#ifndef DISABLE_TIMER_FEATURE
#include "timer.h"
#endif

// HARDWARE SPI
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// SOFTWARE SPI
// MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN,
// MAX_DEVICES);

#if ENABLE_FLASH_BUTTON
EasyButton flash_button(FLASH_BUTTON);
#endif

PlatformWebServer serverHttp(HTTP_PORT);

// create clientId / hostname
String clientIdPrefix = "RDA-MSG-";
String chipId = getUniqueChipId();
String clientId;

// HTTP and HTTPS Default Authentication to store in config file
char web_username[STDSIZE] = DEFAULT_WEB_USER;
char web_password[STDSIZE] = DEFAULT_WEB_PASS;
const char *www_realm = "Custom Auth Realm";
String authFailResponse = "Authentication Failed";

const char *webConfigFile = "/web.config";
webConfigObj webConfig;

// Platform-specific WiFi clients and MQTT client
#ifdef ESP8266
  WiFiClient espClient;
  PubSubClient mqttClient(espClient);
#elif defined(ESP32)
  WiFiClientSecure espClientSecure;
  WiFiClient espClientPlain;
  PubSubClient mqttClient;  // Client will be set dynamically based on TLS setting
#endif

char ap_mode_ssid[64];
const char *ap_mode_password;

// MQTT Configuration
char mqttOnOff[STDSIZE] = "off";
char mqttAnonymous[STDSIZE] = "off";
char mqttAlert[STDSIZE] = "off";
char mqttUsername[STDSIZE] = "";
char mqttPassword[STDSIZE] = "";
char mqttServerAddress[STDSIZE] = "192.168.1.1";
char mqttServerPort[STDSIZE] = "1883";
char mqttTopicPrefix[STDSIZE] = "rdadotmatrix";
char mqttTopicDevice[STDSIZE] = "";
char mqttTopicRoot[STDSIZE] = "";

const unsigned long mqttConnectTimeIntervall = 15000;
long mqttLastReconnectAttempt = 0;
int mqttDisconnected = 0;

// MQTT TLS Configuration (ESP32 only)
#ifdef ESP32
char tlsEnabledMqtt[STDSIZE] = "off";  // Default TLS disabled
bool tlsEnabled = false;
#endif

// Home Assistant Discovery
char mqttHaDiscovery[STDSIZE];
bool mqttDiscoveryPublished = false;

// MQTT Messages Display Toggle
char mqttMessagesEnable[STDSIZE];
char newMqttMessagesEnable[STDSIZE];
bool newMqttMessagesEnableAvailable = false;
char haBaseTopic[256] = "";
unsigned long lastTelemetryUpdate = 0;
const unsigned long telemetryInterval = 60000;

// HA Parameter Memory (RAM-only, initialized from flash defaults on boot)
#ifdef ESP8266
char haLastMessage[256] = "";   // Reduced from MSG_SIZE on ESP8266 to save BSS RAM
#else
char haLastMessage[MSG_SIZE] = "";
#endif
int haLastRepeat = -1;
int haLastBuzzer = -1;
int haLastScrollDelay = -1;
int haLastBrightness = -1;
int haLastAsciiConv = -1;
bool haLastForceRep = false;
bool haParametersInitialized = false;

const char *mqttConfigFile = "/mqtt.config";

mqttConfigObj mqttConfig;

const char *defaultsConfigFile = "/defaults.config";
defaultsConfigObj defaultsConfig;

const char *generalConfigFile = "/general.config";
generalConfigObj generalConfig;

const char *clockConfigFile = "/clock.config";
clockConfigObj clockConfig;

static char emptyMsg[] = "";
msgDef_t M[] = {{1, emptyMsg}};

String version = VERSION;
String mainPageVars;
String mqttPageVars;
String changeCredVars;
String updateVars;
String clockPageVars;

char newWebUsername[STDSIZE];
char newWebPassword[STDSIZE];
char newWebHostname[STDSIZE];
bool newWebUsernameAvailable = false;
bool newWebPasswordAvailable = false;
bool newWebHostnameAvailable = false;
bool saveWebConfigAtStart = false;

char newMqttOnOff[STDSIZE];
char newMqttAnonymous[STDSIZE];
char newMqttAlert[STDSIZE];
char newMqttUsername[STDSIZE];
char newMqttPassword[STDSIZE];
char newMqttServerAddress[STDSIZE];
char newMqttServerPort[STDSIZE];
char newMqttTopicPrefix[STDSIZE];
char newMqttHaDiscovery[STDSIZE];
#ifdef ESP32
char newTlsEnabledMqtt[STDSIZE];
#endif
bool newMqttOnOffAvailable = false;
bool newMqttAnonymousAvailable = false;
bool newMqttAlertAvailable = false;
bool newMqttUsernameAvailable = false;
bool newMqttPasswordAvailable = false;
bool newMqttServerAddressAvailable = false;
bool newMqttServerPortAvailable = false;
bool newMqttTopicPrefixAvailable = false;
bool newMqttHaDiscoveryAvailable = false;
#ifdef ESP32
bool newTlsEnabledMqttAvailable = false;
#endif

char mqttStatusMsg[64] = "";
char mqttAlertMessage[512] = "";
bool saveMqttConfigAtStart = false;
bool alertMqttConnect = false;

char curMessage[MSG_SIZE];
char newMessage[MSG_SIZE];
char newRepeat[REP_SIZE];
char repeatDefault[REP_SIZE] = "10";
char newBuz[BUZ_SIZE];
char buzzerDefault[BUZ_SIZE] = "10";
char newDelay[DEL_SIZE];
char scrollDelayDefault[DEL_SIZE] = "35";
char newBrightness[BRI_SIZE];
char ledBrightnessDefault[BRI_SIZE] = "7";
char newAsciiConv[ASC_SIZE];
char asciiConvDefault[ASC_SIZE] = "1";
char newAlertChirp[STDSIZE];
char alertChirpDefault[STDSIZE] = "Fast Beep";
bool newAlertChirpAvailable = false;

// Buzzer Global Control
char newBuzzerEnable[STDSIZE] = "";
bool newBuzzerEnableAvailable = false;

bool forceRepetitions = false;

bool firstMessage = true;
bool firstMessageOff = false;
bool newMessageAvailable = false;
bool newRepeatAvailable = false;
bool newBuzAvailable = false;
bool explicitBuzzerCount = false;
bool newDelayAvailable = false;
bool newBrightnessAvailable = false;
bool newAsciiConvAvailable = false;
bool matrixFontTest = false;
bool endSetupModeMsg = false;
bool afterWiFiConfig = false;

char assignedIP[255];
int scrollDelay = atoi(scrollDelayDefault);
int ledBrightness = atoi(ledBrightnessDefault);
int repeatCount = 0;
IPAddress apModeIP;
String apModeSSID;

// Clock State Variables
DisplayMode currentDisplayMode = MODE_MESSAGE;
unsigned long transitionStartTime = 0;
unsigned long lastClockUpdate = 0;
unsigned long lastColonToggle = 0;
unsigned long lastNtpResync = 0;
bool clockColonVisible = true;
bool clockNtpSynced = false;
bool clockEnabled = false;
int clockBrightness = 5;

// Brightness Override State
bool brightnessOverrideEnabled = false;
int brightnessOverrideValue = 8;

// Clock date display state
unsigned long lastDateAlternate = 0;
int clockAlternateState = 0;  // 0=time, 1=day-of-week, 2=date
char clockDateBuffer[DATE_FORMAT_SIZE] = "";

#ifndef DISABLE_TIMER_FEATURE
// Timer Configuration
const char *timerConfigFile = "/timer_config.json";
timerConfigObj timerConfig;

// Timer State Variables
bool timerRunning = false;
bool timerPaused = false;
unsigned long timerStartTime = 0;
unsigned long timerPausedTime = 0;
unsigned long timerElapsedSeconds = 0;
unsigned long timerTargetSeconds = 60;
unsigned long timerDurationSeconds = 300;
int timerMode = 0; // TIMER_MODE_COUNTDOWN
bool timerAutoRepeat = false;
bool timerAlertBuzzer = true;
int timerAlertBuzzerCount = 5;
bool timerAlertTriggered = false;
bool timerEnabled = false;
int timerBrightness = 5;
unsigned long lastTimerUpdate = 0;
unsigned long lastTimerDisplayToggle = 0;
bool timerShowingSeconds = false;
TimerState currentTimerState = TIMER_STOPPED;
#endif // DISABLE_TIMER_FEATURE

#ifndef DISABLE_SLEEP_MODE_FEATURE
// Sleep Mode Configuration
const char *sleepModeConfigFile = "/sleep_mode.config";
sleepModeConfigObj sleepModeConfig;

// Sleep Mode State Variables
bool sleepModeEnabled = false;
bool sleepModeActive = false;
bool sleepModeMuteOnly = false;
#endif // DISABLE_SLEEP_MODE_FEATURE

#ifndef DISABLE_ALARM_FEATURE
// Alarm Configuration
alarmConfigObj alarmConfig;
const char *alarmConfigFile = "/alarm_config.json";

// Alarm State Variables
bool alarmMasterEnabled = false;
bool alarmActive = false;
int activeAlarmIndex = -1;
unsigned long alarmStartTime = 0;
int alarmRepeatCounter = 0;
int alarmTotalRepeats = 0;
bool alarmMessageShowing = true;
unsigned long lastAlarmToggle = 0;
unsigned long lastAlarmCheck = 0;
char alarmDisplayMessage[ALARM_MESSAGE_SIZE + 16] = "";
int lastTriggeredAlarmMinute[MAX_ALARMS];
bool alarmPending = false;
int pendingAlarmIndex = -1;
unsigned long alarmPendingTime = 0;

// Recurrent Alarm Configuration
recurrentAlarmConfigObj recurrentAlarmConfig;
const char *recurrentAlarmConfigFile = "/recurrent_alarm_config.json";
bool recurrentAlarmEnabled = false;
unsigned long lastRecurrentAlarmTrigger = 0;
int recurrentAlarmIntervalMinutes = 15;
bool recurrentAlarmDisableWeekends = false;
bool recurrentAlarmPending = false;
unsigned long recurrentAlarmPendingTime = 0;
unsigned long recurrentAlarmDisplayStart = 0;
#endif // DISABLE_ALARM_FEATURE

#ifndef DISABLE_WEATHER_FEATURE
// Weather Configuration
weatherConfigObj weatherConfig;
const char *weatherConfigFile = "/weather_config.json";

// Weather State Variables
bool weatherEnabled = false;
int weatherBrightness = 5;
unsigned long lastWeatherFetch = 0;
unsigned long weatherDisplayStart = 0;
char weatherTemperature[WEATHER_TEMP_SIZE] = "";
char weatherCondition[WEATHER_CONDITION_SIZE] = "";
char weatherForecast[WEATHER_FORECAST_SIZE] = "";
bool weatherDataValid = false;
int weatherCode = 0;
bool weatherRefreshRequested = false;
#ifdef ESP32
volatile bool weatherDataReady = false;
volatile bool weatherFetching = false;
#endif
#endif // DISABLE_WEATHER_FEATURE

#ifndef DISABLE_CRYPTO_FEATURE
// Crypto Price Ticker Configuration
cryptoConfigObj cryptoConfig;
const char *cryptoConfigFile = "/crypto_config.json";

// Crypto State Variables
bool cryptoEnabled = false;
int cryptoBrightness = 5;
unsigned long lastCryptoFetch = 0;
unsigned long cryptoDisplayStart = 0;
char cryptoPriceBuffer[CRYPTO_PRICE_BUF_SIZE] = "";
bool cryptoDataValid = false;
bool cryptoRefreshRequested = false;
#ifdef ESP32
volatile bool cryptoDataReady = false;
volatile bool cryptoFetching = false;
#endif
#endif // DISABLE_CRYPTO_FEATURE
