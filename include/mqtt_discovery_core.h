#ifndef MQTT_DISCOVERY_CORE_H
#define MQTT_DISCOVERY_CORE_H

#include "config.h"

// Helper Functions
void buildHABaseTopic();
void buildDiscoveryTopic(char* buffer, size_t bufferSize, const char* component, const char* entity);
void buildCommandTopic(char* buffer, size_t bufferSize, const char* entity);
void buildStateTopic(char* buffer, size_t bufferSize, const char* entity);
void addDeviceInfo(JsonDocument& doc);
void addAvailability(JsonDocument& doc);
void formatUptime(unsigned long seconds, char* buffer, size_t size);

// Initialization
void initializeHAParameters();

// Main Discovery Orchestrator
void publishDiscoveryMessages();

#endif // MQTT_DISCOVERY_CORE_H
