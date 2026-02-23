#ifndef MQTT_DISCOVERY_SLEEP_H
#define MQTT_DISCOVERY_SLEEP_H

#ifndef DISABLE_SLEEP_MODE_FEATURE
#include "config.h"

// Sleep Mode MQTT Discovery
void publishSleepModeDiscoveries();

// Sleep Mode state publishers
void publishSleepModeState();  // Publishes all sleep mode states
void publishSleepModeEnableState();
void publishSleepModeOnTimeState();
void publishSleepModeOffTimeState();
void publishSleepModeMuteOnlyState();
void publishSleepModeStatusState();

// Sleep Mode command handlers
void handleSleepModeEnableCommand(const char* payload);
void handleSleepModeOnTimeCommand(const char* payload);
void handleSleepModeOffTimeCommand(const char* payload);
void handleSleepModeMuteOnlyCommand(const char* payload);

#endif // DISABLE_SLEEP_MODE_FEATURE
#endif // MQTT_DISCOVERY_SLEEP_H
