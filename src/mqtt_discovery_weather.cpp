#ifndef DISABLE_WEATHER_FEATURE
#include "mqtt_discovery_weather.h"
#include "mqtt_discovery_core.h"
#include "globals.h"
#include "mqtt.h"
#include "config_manager.h"

// Publish all weather MQTT discovery messages
void publishWeatherDiscovery() {
  if (strcmp(mqttOnOff, "on") != 0 || strcmp(mqttHaDiscovery, "on") != 0) {
    return;
  }

  char baseTopic[256];
  snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);

  // Weather Enable Switch
  {
    JsonDocument doc;
    doc["~"] = baseTopic;
    doc["name"] = "Weather Enable";
    doc["uniq_id"] = String(haBaseTopic) + "_weather_enable";
    doc["cmd_t"] = "~/weather_enable/set";
    doc["stat_t"] = "~/weather_enable/state";
    doc["payload_on"] = "ON";
    doc["payload_off"] = "OFF";
    doc["icon"] = "mdi:weather-partly-cloudy";
    doc["ent_cat"] = "config";

    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "switch", "weather_enable");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished weather_enable discovery to: ", topic);
  }

  // Weather Temperature Sensor
  {
    JsonDocument doc;
    doc["~"] = baseTopic;
    doc["name"] = "Weather Temperature";
    doc["uniq_id"] = String(haBaseTopic) + "_weather_temperature";
    doc["stat_t"] = "~/weather_temperature/state";
    doc["icon"] = "mdi:thermometer";
    doc["dev_cla"] = "temperature";

    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "sensor", "weather_temperature");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished weather_temperature discovery to: ", topic);
  }

  // Weather Condition Sensor
  {
    JsonDocument doc;
    doc["~"] = baseTopic;
    doc["name"] = "Weather Condition";
    doc["uniq_id"] = String(haBaseTopic) + "_weather_condition";
    doc["stat_t"] = "~/weather_condition/state";
    doc["icon"] = "mdi:weather-cloudy";

    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "sensor", "weather_condition");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished weather_condition discovery to: ", topic);
  }

  // Weather Location Sensor
  {
    JsonDocument doc;
    doc["~"] = baseTopic;
    doc["name"] = "Weather Location";
    doc["uniq_id"] = String(haBaseTopic) + "_weather_location";
    doc["stat_t"] = "~/weather_location/state";
    doc["icon"] = "mdi:map-marker";
    doc["ent_cat"] = "diagnostic";

    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "sensor", "weather_location");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished weather_location discovery to: ", topic);
  }
}

// Publish current weather temperature state
void publishWeatherTemperatureState() {
  if (strcmp(mqttOnOff, "on") != 0) return;

  char topic[256];
  snprintf(topic, sizeof(topic), "%s/ha/weather_temperature/state", mqttTopicDevice);

  if (weatherDataValid) {
    mqttClient.publish(topic, weatherTemperature, true);
  } else {
    mqttClient.publish(topic, "unavailable", true);
  }
}

// Publish current weather condition state
void publishWeatherConditionState() {
  if (strcmp(mqttOnOff, "on") != 0) return;

  char topic[256];
  snprintf(topic, sizeof(topic), "%s/ha/weather_condition/state", mqttTopicDevice);

  if (weatherDataValid) {
    mqttClient.publish(topic, weatherCondition, true);
  } else {
    mqttClient.publish(topic, "unavailable", true);
  }
}

// Publish weather enabled state
void publishWeatherEnabledState() {
  if (strcmp(mqttOnOff, "on") != 0) return;

  char topic[256];
  snprintf(topic, sizeof(topic), "%s/ha/weather_enable/state", mqttTopicDevice);
  mqttClient.publish(topic, weatherEnabled ? "ON" : "OFF", true);
}

// Publish weather location state
void publishWeatherLocationState() {
  if (strcmp(mqttOnOff, "on") != 0) return;

  char topic[256];
  snprintf(topic, sizeof(topic), "%s/ha/weather_location/state", mqttTopicDevice);

  if (strlen(weatherConfig.location) > 0) {
    mqttClient.publish(topic, weatherConfig.location, true);
  } else if (strlen(weatherConfig.latitude) > 0 && strlen(weatherConfig.longitude) > 0) {
    char coords[64];
    snprintf(coords, sizeof(coords), "%s, %s", weatherConfig.latitude, weatherConfig.longitude);
    mqttClient.publish(topic, coords, true);
  } else {
    mqttClient.publish(topic, "Not configured", true);
  }
}

// Publish all weather states
void publishWeatherState() {
  publishWeatherEnabledState();
  publishWeatherTemperatureState();
  publishWeatherConditionState();
  publishWeatherLocationState();
}

#endif // DISABLE_WEATHER_FEATURE
