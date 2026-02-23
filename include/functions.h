#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "config.h"
#include "utf8_utils.h"
#include "buzzer_utils.h"

// Note: Buzzer & UTF8 functions moved to respective headers

// Brightness Override Helper
int getEffectiveBrightness(int requestedBrightness);

// Message Functions
void onMessageCallHttp();
void onMessageCallJson(String jsonMsgData);
void displayText();
void scrollTextParola();
void displaySilentMsg();
void displayBlockingMessage(bool handleServer, int repeats);

// WiFi Callback
void configModeCallback(WiFiManager *myWiFiManager);

// Device Management Functions
void rebootDevice();
void factoryReset();
void webFactoryReset();

// Button Callback
void onPressed();

// Clock Functions
void applyTimezone();
void initNtpTime();
void getNtpTime(int &hours, int &minutes, int &seconds);
void displayClock(bool withAnimation);
void updateClockDisplay();
bool updateColonBlink();
void updateDisplayMode();
textEffect_t getTransitionEffect(const char* effectName);
void checkWiFiAndResync();
void formatClockDate(char* buffer, size_t bufferSize, const char* format);
void getFormattedTime(char* buffer, size_t bufferSize, bool includeDate);
bool updateDateAlternate();

#ifndef DISABLE_SLEEP_MODE_FEATURE
// Sleep Mode Functions
void updateSleepModeState();
bool isSleepModeActive();
#endif

#ifndef DISABLE_ALARM_FEATURE
// Alarm Functions
void updateAlarms();
void checkAlarmTriggers();
void triggerAlarm(int alarmIndex);
void stopAllAlarms();
void displayAlarm(bool withAnimation);
bool isAlarmActive();
String getAlarmStatusJson();
String getAlarmConfigJson();
String getChirpsJson();

// Recurrent Alarm Functions
void checkRecurrentAlarm();
void triggerRecurrentAlarm();
int getIntervalMinutes(const char* interval);
#endif

#endif // FUNCTIONS_H
