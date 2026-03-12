#ifndef MQTT_DISCOVERY_SENSORS_H
#define MQTT_DISCOVERY_SENSORS_H

#include "config.h"

// Discovery Publishing Functions
void publishLightDiscovery();
void publishTextDiscovery();
void publishNumberDiscoveries();
void publishSwitchDiscoveries();
void publishSensorDiscoveries();
void publishButtonDiscoveries();
void publishBuzzerEnableDiscovery();
void publishBrightnessOverrideEnableDiscovery();
void publishBrightnessOverrideValueDiscovery();
void publishAlertChirpDiscovery();

// State Publishing Functions
void publishLightState();
void publishMessageState();
void publishMessageDisplayedState();
void publishRepeatCountdownState(int count);
void publishRepeatCountdownState();
void publishRepeatState();
void publishScrollDelayState();
void publishBuzzerState();
void publishBuzzerEnableState();
void publishBrightnessOverrideEnableState();
void publishBrightnessOverrideValueState();
void publishAlertChirpState();
void publishAsciiConvState();
void publishForceRepState();
void publishSensorStates();
void publishAllStates();
void updateTelemetryIfNeeded();

// Command Handlers
void handleLightCommand(const char* payload);
void handleMessageCommand(const char* payload);
void handleRepeatCommand(const char* payload);
void handleScrollDelayCommand(const char* payload);
void handleBuzzerCommand(const char* payload);
void handleBuzzerEnableCommand(const char* payload);
void handleBrightnessOverrideEnableCommand(const char* payload);
void handleBrightnessOverrideValueCommand(const char* payload);
void handleAlertChirpCommand(const char* payload);
void handleAsciiConvCommand(const char* payload);
void handleForceRepCommand(const char* payload);
void handleRebootCommand(const char* payload);
void handleClearCommand(const char* payload);
void handleSendCommand(const char* payload);

// Command Subscription & Routing
void subscribeToHACommands();
bool isHACommand(const char* topic);
void routeHACommand(const char* topic, const char* payload);

#ifdef ESP32
void publishTLSStatusState();
#endif

#endif // MQTT_DISCOVERY_SENSORS_H
