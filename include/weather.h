#ifndef WEATHER_H
#define WEATHER_H

#include "config.h"

#ifndef DISABLE_WEATHER_FEATURE

// Weather update and display functions
void updateWeather();                              // Main update function called from loop
void fetchWeatherData(bool forceRefresh = false);  // HTTP API call to weather service
void displayWeather(bool withAnimation); // Display weather on LED
bool shouldDisplayWeather();             // Check if weather should be shown
String getWeatherStatusJson();           // JSON status for web UI
String getWeatherConfigJson();           // JSON config for web UI

// Weather code to condition string conversion
const char* getWeatherCondition(int code);

// Weather code to icon/emoji conversion (for 8-module displays)
const char* getWeatherIcon(int code);

#endif // DISABLE_WEATHER_FEATURE

#endif // WEATHER_H
