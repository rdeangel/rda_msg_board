#ifndef MQTT_DISCOVERY_H
#define MQTT_DISCOVERY_H

// Main MQTT Discovery Module - includes all sub-modules
#include "mqtt_discovery_core.h"
#include "mqtt_discovery_sensors.h"
#ifndef DISABLE_TIMER_FEATURE
#include "mqtt_discovery_timer.h"
#endif
#include "mqtt_discovery_clock.h"

#endif // MQTT_DISCOVERY_H
