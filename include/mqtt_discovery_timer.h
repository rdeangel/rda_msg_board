#ifndef MQTT_DISCOVERY_TIMER_H
#define MQTT_DISCOVERY_TIMER_H

#ifndef DISABLE_TIMER_FEATURE
#include "config.h"

// Timer MQTT Discovery
void publishTimerDiscoveries();

// Timer state publishers
void publishTimerEnableState();
void publishTimerModeState();
void publishTimerDurationState();
void publishTimerBrightnessState();
void publishTimerAlertBuzzerState();
void publishTimerAutoRepeatState();
void publishTimerAlertChirpState();
void publishTimerAlertBeepCountState();
void publishTimerStateState();
void publishTimerDisplayState();
void publishTimerRemainingState();
void publishTimerElapsedState();

// Timer command handlers
void handleTimerEnableCommand(const char* payload);
void handleTimerModeCommand(const char* payload);
void handleTimerDurationCommand(const char* payload);
void handleTimerStartCommand();
void handleTimerPauseCommand();
void handleTimerStopCommand();
void handleTimerResetCommand();
void handleTimerBrightnessCommand(const char* payload);
void handleTimerAlertBuzzerCommand(const char* payload);
void handleTimerAutoRepeatCommand(const char* payload);
void handleTimerAlertChirpCommand(const char* payload);
void handleTimerAlertBeepCountCommand(const char* payload);

#endif // DISABLE_TIMER_FEATURE
#endif // MQTT_DISCOVERY_TIMER_H
