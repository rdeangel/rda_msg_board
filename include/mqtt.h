#ifndef MQTT_H
#define MQTT_H

#include "config.h"
#include "mqtt_discovery.h"

// MQTT Functions
void plainMsgFunct(String plainMsgString);
void mqttCallBack(const char *topic, byte *payload, unsigned int length);
void mqttConnectProc(char mqttAlertMessageIn[128], bool buzz);
void mqttDisconnectedProc(char mqttAlertMessageIn[128], bool buzzIn);
boolean reconnectMqtt();
void loadMqttConfiguration(const char *mqttConfigFile, mqttConfigObj &mqttConfig);
void saveMqttConfiguration(const char *mqttConfigFile, const mqttConfigObj &mqttConfig);
void printMqttFile(const char *mqttConfigFile);
void changeMqttConfig();
void initMqttStoreConfig();
void initMqtt();
void checkMqtt();

#endif // MQTT_H
