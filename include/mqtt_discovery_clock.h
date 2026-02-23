#ifndef MQTT_DISCOVERY_CLOCK_H
#define MQTT_DISCOVERY_CLOCK_H

#include "config.h"

// Clock Discovery
void publishClockDiscoveries();

// Clock State Publishing Functions
void publishClockEnableState();
void publishClockBrightnessState();
void publishClockNtpServerState();
void publishClockCustomTzState();
void publishClockDateFormatState();
void publishClockDateAlternateSecondsState();
void publishClockCustomFormatState();
void publishClockTransitionDelayState();
void publishClockTransitionSpeedState();
void publishClockTransitionEffectState();
void publishClockRandomizeState();
void publishClockResyncIntervalState();
void publishClockTimeState();
void publishClockNtpSyncedState();
void publishClockDisplayActiveState();
void publishDisplayModeState();
void publishAllClockStates();

// Recurrent Alarm Discovery
void publishRecurrentAlarmDiscoveries();

// Recurrent Alarm State Publishing Functions
void publishRecurrentAlarmEnableState();
void publishRecurrentAlarmIntervalState();
void publishRecurrentAlarmChirpState();
void publishAllRecurrentAlarmStates();

// Recurrent Alarm Command Handlers
void handleRecurrentAlarmEnableCommand(const char* payload);
void handleRecurrentAlarmIntervalCommand(const char* payload);
void handleRecurrentAlarmChirpCommand(const char* payload);

// Clock Command Handlers
void handleClockEnableCommand(const char* payload);
void handleClockBrightnessCommand(const char* payload);
void handleClockNtpServerCommand(const char* payload);
void handleClockCustomTzCommand(const char* payload);
void handleClockDateFormatCommand(const char* payload);
void handleClockDateAlternateSecondsCommand(const char* payload);
void handleClockCustomFormatCommand(const char* payload);
void handleClockTransitionDelayCommand(const char* payload);
void handleClockTransitionSpeedCommand(const char* payload);
void handleClockTransitionEffectCommand(const char* payload);
void handleClockRandomizeCommand(const char* payload);
void handleClockResyncIntervalCommand(const char* payload);

#endif // MQTT_DISCOVERY_CLOCK_H
