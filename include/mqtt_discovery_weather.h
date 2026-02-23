#ifndef MQTT_DISCOVERY_WEATHER_H
#define MQTT_DISCOVERY_WEATHER_H

#include "config.h"

#ifndef DISABLE_WEATHER_FEATURE

// MQTT Discovery functions for Home Assistant Weather integration
void publishWeatherDiscovery();        // Publish discovery topics for weather entities
void publishWeatherTemperatureState(); // Publish current temperature
void publishWeatherConditionState();   // Publish current condition
void publishWeatherEnabledState();     // Publish enabled state
void publishWeatherLocationState();    // Publish location info
void publishWeatherState();            // Publish all weather states

#endif // DISABLE_WEATHER_FEATURE

#endif // MQTT_DISCOVERY_WEATHER_H
