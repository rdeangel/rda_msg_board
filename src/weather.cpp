#include "weather.h"
#include "globals.h"
#include "functions.h"
#include "config_manager.h"
 #include "utf8_utils.h"

#ifndef DISABLE_WEATHER_FEATURE

#ifdef ESP8266
  #include <ESP8266HTTPClient.h>
  #include <WiFiClient.h>
#elif defined(ESP32)
  #include <HTTPClient.h>
  #include <WiFiClientSecure.h>
#endif

// Weather code to condition string mapping (WMO Weather interpretation codes)
// Reference: https://open-meteo.com/en/docs
const char* getWeatherCondition(int code) {
  switch (code) {
    case 0: return "Clear";
    case 1: return "Mostly Clear";
    case 2: return "Partly Cloudy";
    case 3: return "Overcast";
    case 45: return "Foggy";
    case 48: return "Icy Fog";
    case 51: return "Light Drizzle";
    case 53: return "Drizzle";
    case 55: return "Heavy Drizzle";
    case 56: return "Freezing Drizzle";
    case 57: return "Freezing Drizzle";
    case 61: return "Light Rain";
    case 63: return "Rain";
    case 65: return "Heavy Rain";
    case 66: return "Freezing Rain";
    case 67: return "Freezing Rain";
    case 71: return "Light Snow";
    case 73: return "Snow";
    case 75: return "Heavy Snow";
    case 77: return "Snow Grains";
    case 80: return "Light Showers";
    case 81: return "Showers";
    case 82: return "Heavy Showers";
    case 85: return "Snow Showers";
    case 86: return "Heavy Snow";
    case 95: return "Thunderstorm";
    case 96: return "Thunderstorm";
    case 99: return "Thunderstorm";
    default: return "Unknown";
  }
}

// Weather code to simple icon (for compact displays)
const char* getWeatherIcon(int code) {
  if (code == 0) return "*";           // Clear - sun
  if (code <= 3) return "~";           // Partly cloudy
  if (code <= 48) return "=";          // Foggy
  if (code <= 67) return "'";          // Rain/drizzle
  if (code <= 77) return "#";          // Snow
  if (code <= 82) return "'";          // Showers
  if (code <= 86) return "#";          // Snow showers
  if (code >= 95) return "!";          // Thunderstorm
  return "?";
}

// Fetch weather data from Open-Meteo API
// forceRefresh bypasses the enabled check (for manual refresh from UI)
void fetchWeatherData(bool forceRefresh) {
  if (!forceRefresh && !weatherEnabled) return;

  // Check if coordinates are configured
  if (strlen(weatherConfig.latitude) == 0 || strlen(weatherConfig.longitude) == 0) {
    PRINTS("\nWeather: No coordinates configured");
    weatherDataValid = false;
    return;
  }

  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    PRINTS("\nWeather: WiFi not connected");
    return;
  }

  PRINTS("\nFetching weather data...");

  // Build API URL using char buffer (stack) to avoid String fragmentation
  // Open-Meteo API: free, no key required
  char url[512];
  bool isFahrenheit = (strcmp(weatherConfig.temperatureUnit, "F") == 0);
  
  snprintf(url, sizeof(url), 
           "http://api.open-meteo.com/v1/forecast?latitude=%s&longitude=%s&current=temperature_2m,weather_code&daily=temperature_2m_max,temperature_2m_min&forecast_days=1%s",
           weatherConfig.latitude, 
           weatherConfig.longitude,
           isFahrenheit ? "&temperature_unit=fahrenheit" : "");

  PRINT("\nWeather URL: ", url);

  #ifdef ESP8266
    WiFiClient client;
    HTTPClient http;
    http.begin(client, url);
  #elif defined(ESP32)
    HTTPClient http;
    http.begin(url);
  #endif

  http.setTimeout(5000);  // Reduce to 5 seconds to avoid long blocking
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    // We use getString() instead of getStream() because getStream() can cause InvalidInput errors
    // with deserializeJson() on some ESP core versions, likely due to stream buffering issues.
    // The payload is small (approx 500-600 bytes) so a temporary String is acceptable.
    String payload = http.getString();
    
    if (payload.length() > 0) {
      PRINT("\nWeather response: ", payload.c_str());

      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, payload);

      if (!error) {
        // Extract current weather
        float temp = doc["current"]["temperature_2m"] | 0.0f;
        int code = doc["current"]["weather_code"] | 0;

        // Extract daily forecast (high/low)
        float tempMax = 0.0f;
        float tempMin = 0.0f;

        if (!doc["daily"]["temperature_2m_max"].isNull()) {
          JsonArray maxArr = doc["daily"]["temperature_2m_max"].as<JsonArray>();
          if (maxArr.size() > 0) tempMax = maxArr[0];
        }
        if (!doc["daily"]["temperature_2m_min"].isNull()) {
          JsonArray minArr = doc["daily"]["temperature_2m_min"].as<JsonArray>();
          if (minArr.size() > 0) tempMin = minArr[0];
        }

        // Store weather data
        weatherCode = code;
        const char* unit = isFahrenheit ? "°F" : "°C";

        // Format temperature string
        snprintf(weatherTemperature, sizeof(weatherTemperature), "%.0f%s", temp, unit);

        // Format condition string
        strlcpy(weatherCondition, getWeatherCondition(code), sizeof(weatherCondition));

        // Format forecast string (high/low)
        snprintf(weatherForecast, sizeof(weatherForecast), "High:%.0f%s Low:%.0f%s", tempMax, unit, tempMin, unit);

        weatherDataValid = true;

        PRINT("\nWeather updated: ", weatherTemperature);
        PRINT(" ", weatherCondition);
        PRINT(" ", weatherForecast);
      } else {
        PRINT("\nWeather JSON parse error: ", error.c_str());
        weatherDataValid = false;
      }
    } else {
      PRINTS("\nWeather error: Empty payload");
      weatherDataValid = false;
    }
  } else {
    PRINT("\nWeather HTTP error: ", httpCode);
    weatherDataValid = false;
  }

  // CRITICAL: Update last fetch time even on failure to prevent rapid-fire retry loop
  // This prevents Denial of Service on self/API and prevents potential WDT resets
  lastWeatherFetch = millis();

  http.end();
}

// Check if weather should be displayed
bool shouldDisplayWeather() {
  if (!weatherEnabled) return false;
  if (!weatherDataValid) return false;
  if (!clockEnabled) return false;  // Weather only shows when clock is enabled
  if (!clockNtpSynced) return false;  // Need time synced

  // Don't show during messages
  if (newMessageAvailable || curMessage[0] != '\0') return false;

  #ifndef DISABLE_TIMER_FEATURE
  // Don't show during timer
  if (timerRunning) return false;
  #endif

  #ifndef DISABLE_ALARM_FEATURE
  // Don't show during alarm
  if (alarmActive) return false;
  #endif

  #ifndef DISABLE_SLEEP_MODE_FEATURE
  // Don't show during full sleep mode
  if (isSleepModeActive() && !sleepModeMuteOnly) return false;
  #endif

  return true;
}

// Display weather on LED matrix
void displayWeather(bool withAnimation) {
  #ifndef DISABLE_SLEEP_MODE_FEATURE
  if (isSleepModeActive() && !sleepModeMuteOnly) {
    P.displayClear();
    return;
  }
  #endif

  if (!weatherDataValid) return;

  P.setFont(nullptr);
  P.setIntensity(getEffectiveBrightness(weatherBrightness));

  // Build display string - always include forecast since we're scrolling
  static char weatherDisplayString[256];
  snprintf(weatherDisplayString, sizeof(weatherDisplayString), "%s %s %s",
           weatherTemperature, weatherCondition, weatherForecast);

  // Convert UTF-8 to extended ASCII for LED display (handles ° symbol)
  utf8Ascii(weatherDisplayString);

  if (withAnimation) {
    P.displayClear();

    int speed = atoi(clockConfig.transitionSpeed);
    uint16_t animSpeed = (speed < 10) ? 10 : speed;

    // Scroll the weather text
    P.displayText(weatherDisplayString, PA_CENTER, animSpeed, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    P.displayReset();
  } else {
    // Just update the text (no animation restart)
    P.displayAnimate();
  }
}

// Main weather update function - called from loop
void updateWeather() {
  // Handle manual refresh request from web UI (done here to avoid stack issues)
  if (weatherRefreshRequested) {
    weatherRefreshRequested = false;
    Serial.println(F("Processing weather refresh request..."));
    fetchWeatherData(true);  // Force refresh bypasses enabled check
    return;
  }

  if (!weatherEnabled) return;

  // Fetch weather data periodically
  unsigned long updateInterval = (unsigned long)atoi(weatherConfig.updateIntervalMinutes) * 60000UL;

  // Initial fetch or periodic update
  if (lastWeatherFetch == 0 || (millis() - lastWeatherFetch >= updateInterval)) {
    fetchWeatherData(false);
  }
}

// Get weather status as JSON for web UI
String getWeatherStatusJson() {
  JsonDocument doc;

  doc["enabled"] = weatherEnabled;
  doc["dataValid"] = weatherDataValid;
  doc["temperature"] = weatherTemperature;
  doc["condition"] = weatherCondition;
  doc["forecast"] = weatherForecast;
  doc["weatherCode"] = weatherCode;
  doc["lastUpdate"] = lastWeatherFetch;
  doc["location"] = weatherConfig.location;
  doc["latitude"] = weatherConfig.latitude;
  doc["longitude"] = weatherConfig.longitude;
  doc["updateInterval"] = weatherConfig.updateIntervalMinutes;
  doc["temperatureUnit"] = weatherConfig.temperatureUnit;
  doc["brightness"] = weatherConfig.brightness;
  doc["displayDuration"] = weatherConfig.displayDurationSeconds;

  String output;
  serializeJson(doc, output);
  return output;
}

// Get weather configuration as JSON
String getWeatherConfigJson() {
  JsonDocument doc;

  doc["enabled"] = weatherConfig.enabled;
  doc["location"] = weatherConfig.location;
  doc["latitude"] = weatherConfig.latitude;
  doc["longitude"] = weatherConfig.longitude;
  doc["updateIntervalMinutes"] = weatherConfig.updateIntervalMinutes;
  doc["temperatureUnit"] = weatherConfig.temperatureUnit;
  doc["brightness"] = weatherConfig.brightness;
  doc["displayDurationSeconds"] = weatherConfig.displayDurationSeconds;

  String output;
  serializeJson(doc, output);
  return output;
}

#endif // DISABLE_WEATHER_FEATURE
