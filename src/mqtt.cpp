#include "mqtt.h"
#include "globals.h"
#include "functions.h"

//processes plain messages
void plainMsgFunct(String plainMsgString) {
  strcpy(newMessage, plainMsgString.c_str());
  PRINT("\nMQTT Plain Message Received!\nMQTT Message:\n", newMessage);
  repeatCount = 0;
  newMessageAvailable = true;
  strcpy(newRepeat, repeatDefault);
  newRepeatAvailable = true;
  strcpy(newBuz, buzzerDefault);
  newBuzAvailable = true;
  strcpy(newDelay, scrollDelayDefault);
  newDelayAvailable = true;
  strcpy(newBrightness, ledBrightnessDefault);
  newBrightnessAvailable = true;
  //strcpy(newAsciiConv, "1");
  strcpy(newAsciiConv, asciiConvDefault);
  newAsciiConvAvailable = true;
  strcpy(newAlertChirp, alertChirpDefault);
  newAlertChirpAvailable = true;
}

//function called when an MQTT message is received
void mqttCallBack(const char *topic, byte *payload, unsigned int length) {
  PRINTS("\nMQTT CALLBACK SEEN!")
  String PayloadString = "";
  payload[length] = '\0';
  for (unsigned int i = 0; i < length; i++) {
    PayloadString += (char)payload[i];
  }

  // Check for Home Assistant commands first
  if (isHACommand(topic)) {
    routeHACommand(topic, PayloadString.c_str());
    return;  // Don't process as legacy MQTT
  }

  // Check if MQTT message display is disabled
  if (strcmp(mqttMessagesEnable, "on") != 0) {
    PRINTS("\nMQTT Messages display is disabled - ignoring message");
    return;
  }

  //matches when messages come in with topic NOT ending in /json and when topic configured is NOT wildcard #
  if ((String(topic).startsWith((String(mqttTopicPrefix))) && ((String(topic).endsWith("/json")) == false)) ||
  ((String(topic).startsWith(String(mqttTopicRoot))) && ((String(topic).endsWith("/json")) == false)) ||
  (strcmp(topic, (char*) (String(mqttTopicDevice) + "").c_str()) == 0)) {
    plainMsgFunct(PayloadString);
  }
  //matches when messages come in with topic ending in /json and when topic configured is NOT wildcard #
  else if ((String(topic).startsWith((String(mqttTopicPrefix))) && ((String(topic).endsWith("/json")) == true)) ||
  ((String(topic).startsWith(String(mqttTopicRoot))) && ((String(topic).endsWith("/json")) == true)) ||
  ((String(topic).startsWith(String(mqttTopicDevice))) && ((String(topic).endsWith("/json")) == true)) ||
  ((String(topic).endsWith("/json")) == true)) {
    PRINTS("\nMQTT JSON Message Arrived!\nMQTT Message: ");
    onMessageCallJson(PayloadString.c_str());
  }
  else {
    plainMsgFunct(PayloadString);
  }
}

void mqttConnectProc(char mqttAlertMessageIn[128], bool buzz) {
  PRINTS("\n\nRestoring MQTT connection...");
  // Attempt to connect
  PRINTS("\n");
  Serial.print(clientId);
  PRINT(" connected to MQTT Server: ", mqttServerAddress);
  PRINT(":", mqttServerPort);

  // Only publish online status if HA Discovery is enabled
  if (strcmp(mqttHaDiscovery, "on") == 0) {
    mqttClient.publish((char*) (String(mqttTopicDevice) + "/status").c_str(), "online", true);
    PRINT("\nPublishing to topic ", (String(mqttTopicDevice)  + "/status"));
    PRINTS(": online");
  }
  //PRINTS("\n");
  // subscribe
  //####mqttClient.subscribe((char*) (TOPIC_PREFIX + "/#").c_str());
  int slashIndex = String(mqttTopicPrefix).indexOf('/');
  int hashIndex = String(mqttTopicPrefix).indexOf('#');
  strcpy(mqttTopicRoot, (char*) (String(mqttTopicPrefix).substring(0, slashIndex)).c_str());
  String strMqttTopicRoot = String(mqttTopicRoot);
  String strMqttTopicPrefix = String(mqttTopicPrefix);
  String strMqttTopicDevice = String(mqttTopicDevice);
  mqttClient.subscribe((char*) strMqttTopicRoot.c_str());
  PRINT("\nSubscribe to topic: ", strMqttTopicRoot);
  if (hashIndex == 1) {
    mqttClient.subscribe((char*) (strMqttTopicRoot + "/json").c_str());
    PRINT("\nSubscribe to topic: ", (strMqttTopicRoot + "/json"));
  }
  if (strMqttTopicPrefix != strMqttTopicRoot) {
    mqttClient.subscribe((char*) (strMqttTopicRoot + "/json").c_str());
    PRINT("\nSubscribe to topic: ", (strMqttTopicRoot + "/json"));
    mqttClient.subscribe((char*) (strMqttTopicPrefix + "").c_str());
    PRINT("\nSubscribe to topic: ", (strMqttTopicPrefix + ""));
  }
  if (hashIndex == -1) {
    mqttClient.subscribe((char*) (strMqttTopicPrefix + "/json").c_str());
    PRINT("\nSubscribe to topic: ", (strMqttTopicPrefix + "/json"));
  }
  mqttClient.subscribe((char*) (strMqttTopicDevice + "").c_str());
  PRINT("\nSubscribe to topic: ", (strMqttTopicDevice + ""));
  mqttClient.subscribe((char*) (strMqttTopicDevice + "/json").c_str());
  PRINT("\nSubscribe to topic: ", (strMqttTopicDevice+ "/json"));
  PRINTS("\n");
  if ((strcmp(mqttAlert, "on") == 0)  and (buzz)) {
    sprintf(newMessage,mqttAlertMessageIn);
    mqttConnectBuzzer();
    displaySilentMsg();
  }
}

void mqttDisconnectedProc(char mqttAlertMessageIn[128], bool buzzIn) {
  PRINTS("\n");
  Serial.print(clientId);
  PRINT(" has lost connection to MQTT Server: ", mqttServerAddress);
  PRINT(":", mqttServerPort);
  if ((strcmp(mqttAlert, "on") == 0) and (buzzIn)){
    sprintf(newMessage,mqttAlertMessageIn);
    mqttDisconnectBuzzer();
    displaySilentMsg();
  }
}

boolean reconnectMqtt() {
  if (strcmp(mqttAnonymous, "off") == 0) {
    // Only set up Last Will Testament (LWT) if HA Discovery is enabled
    bool haEnabled = (strcmp(mqttHaDiscovery, "on") == 0);
    bool connected = false;

    if (haEnabled) {
      char statusTopic[256];
      snprintf(statusTopic, sizeof(statusTopic), "%s/status", mqttTopicDevice);
      connected = mqttClient.connect(clientId.c_str(), mqttUsername, mqttPassword,
                                     statusTopic, 1, true, "offline");
    } else {
      // Connect without LWT when HA Discovery is disabled
      connected = mqttClient.connect(clientId.c_str(), mqttUsername, mqttPassword);
    }

    if (connected) {
      mqttStatusMsg[0] = '\0';
      snprintf(mqttAlertMessage, sizeof(mqttAlertMessage), "%sMQTT connected to server: %s:%s in user mode", mqttStatusMsg, mqttServerAddress, mqttServerPort);
      mqttConnectProc(mqttAlertMessage, true);
      initializeHAParameters();    // Initialize HA params from defaults
      publishDiscoveryMessages();  // Publish HA Discovery (will check if enabled internally)
      mqttDisconnected = 0;
    }
    else {
      mqttDisconnected += 1;
      if (mqttDisconnected == 1) {
        mqttStatusMsg[0] = '\0';
        snprintf(mqttAlertMessage, sizeof(mqttAlertMessage), "%sMQTT disconnected from Server: %s:%s", mqttStatusMsg, mqttServerAddress, mqttServerPort);
        mqttDisconnectedProc(mqttAlertMessage, true);
      }
    }
  }
  else if (strcmp(mqttAnonymous, "on") == 0) {
    // Only set up Last Will Testament (LWT) if HA Discovery is enabled
    bool haEnabled = (strcmp(mqttHaDiscovery, "on") == 0);
    bool connected = false;

    if (haEnabled) {
      char statusTopic[256];
      snprintf(statusTopic, sizeof(statusTopic), "%s/status", mqttTopicDevice);
      connected = mqttClient.connect(clientId.c_str(), statusTopic, 1, true, "offline");
    } else {
      // Connect without LWT when HA Discovery is disabled
      connected = mqttClient.connect(clientId.c_str());
    }

    if (connected) {
      mqttStatusMsg[0] = '\0';
      snprintf(mqttAlertMessage, sizeof(mqttAlertMessage), "%sMQTT connected to server: %s:%s in anonymous mode", mqttStatusMsg, mqttServerAddress, mqttServerPort);
      mqttConnectProc(mqttAlertMessage, true);
      initializeHAParameters();    // Initialize HA params from defaults
      publishDiscoveryMessages();  // Publish HA Discovery (will check if enabled internally)
      mqttDisconnected = 0;
    }
    else {
      mqttDisconnected += 1;
      if (mqttDisconnected == 1) {
        mqttStatusMsg[0] = '\0';
        snprintf(mqttAlertMessage, sizeof(mqttAlertMessage), "%sMQTT disconnected from Server: %s:%s", mqttStatusMsg, mqttServerAddress, mqttServerPort);
        mqttDisconnectedProc(mqttAlertMessage, true);
      }
    }
  }
  return mqttClient.connected();
}

// Loads the configuration from a file
void loadMqttConfiguration(const char *mqttConfigFile, mqttConfigObj &mqttConfig) {

  File file = LittleFS.open(mqttConfigFile, "r");
  if (!file) {
    Serial.println("Failed to open data file");
    return;
  }

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/v7/assistant to compute the capacity.
  JsonDocument doc;

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error)
    Serial.println(F("Failed to read file, using default configuration"));

  // Copy values from the JsonDocument to the mqttConfig
  strlcpy(mqttConfig.onOffMqttHolder,
          doc["onOffMqttHolder"],
          sizeof(mqttConfig.onOffMqttHolder));
  strlcpy(mqttConfig.anonymousMqttHolder,
          doc["anonymousMqttHolder"],
          sizeof(mqttConfig.anonymousMqttHolder));
  strlcpy(mqttConfig.alertMqttHolder,
          doc["alertMqttHolder"],
          sizeof(mqttConfig.alertMqttHolder));
  strlcpy(mqttConfig.usernameMqttHolder,
          doc["usernameMqttHolder"],
          sizeof(mqttConfig.usernameMqttHolder));
  strlcpy(mqttConfig.passwordMqttHolder,
          doc["passwordMqttHolder"],
          sizeof(mqttConfig.passwordMqttHolder));
  strlcpy(mqttConfig.serverAddressMqttHolder,
          doc["serverAddressMqttHolder"],
          sizeof(mqttConfig.serverAddressMqttHolder));
  strlcpy(mqttConfig.serverPortMqttHolder,
          doc["serverPortMqttHolder"],
          sizeof(mqttConfig.serverPortMqttHolder));
  strlcpy(mqttConfig.topicPrefixMqttHolder,
          doc["topicPrefixMqttHolder"],
          sizeof(mqttConfig.topicPrefixMqttHolder));
  strlcpy(mqttConfig.haDiscoveryMqttHolder,
          doc["haDiscoveryMqttHolder"],
          sizeof(mqttConfig.haDiscoveryMqttHolder));
  strlcpy(mqttConfig.mqttMessagesEnableHolder,
          doc["mqttMessagesEnableHolder"] | "on",  // Default to "on"
          sizeof(mqttConfig.mqttMessagesEnableHolder));
  strlcpy(mqttConfig.tlsEnabledMqttHolder,
          doc["tlsEnabledMqttHolder"] | "off",
          sizeof(mqttConfig.tlsEnabledMqttHolder));


  // Close the file (Curiously, File's destructor doesn't close the file)
  file.close();
}

// Saves the configuration to a file
void saveMqttConfiguration(const char *mqttConfigFile, const mqttConfigObj &mqttConfig) {
  // Delete existing file, otherwise the configuration is appended to the file
  //LittleFS.remove(mqttConfigFile);

  // Open file for writing
  File file = LittleFS.open(mqttConfigFile, "w");
  if (!file) {
    Serial.println("Failed to open config file for writing");
    return;
  }
  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/assistant to compute the capacity.
  JsonDocument doc;
  
  // Set the values in the document
  doc["onOffMqttHolder"] = mqttConfig.onOffMqttHolder;
  doc["anonymousMqttHolder"] = mqttConfig.anonymousMqttHolder;
  doc["alertMqttHolder"] = mqttConfig.alertMqttHolder;
  doc["usernameMqttHolder"] = mqttConfig.usernameMqttHolder;
  doc["passwordMqttHolder"] = mqttConfig.passwordMqttHolder;
  doc["serverAddressMqttHolder"] = mqttConfig.serverAddressMqttHolder;
  doc["serverPortMqttHolder"] = mqttConfig.serverPortMqttHolder;
  doc["topicPrefixMqttHolder"] = mqttConfig.topicPrefixMqttHolder;
  doc["haDiscoveryMqttHolder"] = mqttConfig.haDiscoveryMqttHolder;
  doc["mqttMessagesEnableHolder"] = mqttConfig.mqttMessagesEnableHolder;
  doc["tlsEnabledMqttHolder"] = mqttConfig.tlsEnabledMqttHolder;

  // Serialize JSON to file
  if (serializeJson(doc, file) == 0) {
    Serial.println(F("Failed to write to file"));
  }
  
  // Close the file
  file.close();
}

// Prints the content of a file to the Serial
void printMqttFile(const char *mqttConfigFile) {
  // Open file for reading

  File file = LittleFS.open(mqttConfigFile, "r");
  if (!file) {
    Serial.println("Failed to open data file");
    return;
  }
  // Extract each characters by one by one
  while (file.available()) {
    Serial.print((char)file.read());
  }
  Serial.println();

  // Close the file
  file.close();
}

//change login credentials and store into config file
void changeMqttConfig() {
  //set new mqtt config values from webpage to config object
  if (newMqttOnOffAvailable) {
    strlcpy(mqttConfig.onOffMqttHolder, newMqttOnOff, sizeof(mqttConfig.onOffMqttHolder));
  }
  if (newMqttAnonymousAvailable) {
    strlcpy(mqttConfig.anonymousMqttHolder, newMqttAnonymous, sizeof(mqttConfig.anonymousMqttHolder));
  }
  if (newMqttAlertAvailable) {
    strlcpy(mqttConfig.alertMqttHolder, newMqttAlert, sizeof(mqttConfig.alertMqttHolder));
  }
  if (newMqttUsernameAvailable) {
    strlcpy(mqttConfig.usernameMqttHolder, newMqttUsername, sizeof(mqttConfig.usernameMqttHolder));
  }
  if (newMqttPasswordAvailable) {
    strlcpy(mqttConfig.passwordMqttHolder, newMqttPassword, sizeof(mqttConfig.passwordMqttHolder));
  }
  if (newMqttServerAddressAvailable) {
    strlcpy(mqttConfig.serverAddressMqttHolder, newMqttServerAddress, sizeof(mqttConfig.serverAddressMqttHolder));
  }
  if (newMqttServerPortAvailable) {
    strlcpy(mqttConfig.serverPortMqttHolder, newMqttServerPort, sizeof(mqttConfig.serverPortMqttHolder));
  }
  if (newMqttTopicPrefixAvailable) {
    strlcpy(mqttConfig.topicPrefixMqttHolder, newMqttTopicPrefix, sizeof(mqttConfig.topicPrefixMqttHolder));
  }
  if (newMqttHaDiscoveryAvailable) {
    // Check if HA Discovery is being turned off
    bool wasEnabled = (strcmp(mqttHaDiscovery, "on") == 0);
    bool willBeEnabled = (strcmp(newMqttHaDiscovery, "on") == 0);

    if (wasEnabled && !willBeEnabled && mqttClient.connected()) {
      // Publish offline status before disabling
      char statusTopic[256];
      snprintf(statusTopic, sizeof(statusTopic), "%s/status", mqttTopicDevice);
      mqttClient.publish(statusTopic, "offline", true);
      PRINTS("\nHA Discovery disabled - published offline status");
      delay(100);  // Brief delay to ensure message is sent
    }

    strlcpy(mqttConfig.haDiscoveryMqttHolder, newMqttHaDiscovery, sizeof(mqttConfig.haDiscoveryMqttHolder));
  }
  if (newMqttMessagesEnableAvailable) {
    strlcpy(mqttConfig.mqttMessagesEnableHolder, newMqttMessagesEnable, sizeof(mqttConfig.mqttMessagesEnableHolder));
    strlcpy(mqttMessagesEnable, newMqttMessagesEnable, sizeof(mqttMessagesEnable));
    newMqttMessagesEnableAvailable = false;
  }
#ifdef ESP32
  if (newTlsEnabledMqttAvailable) {
    strlcpy(mqttConfig.tlsEnabledMqttHolder, newTlsEnabledMqtt, sizeof(mqttConfig.tlsEnabledMqttHolder));
  }
#endif
  //save new mqtt config values set from config object to config file
  saveMqttConfiguration(mqttConfigFile, mqttConfig);
  //apply the new mqtt config values as running values
  strlcpy(mqttOnOff, mqttConfig.onOffMqttHolder, sizeof(mqttOnOff));
  strlcpy(mqttAnonymous, mqttConfig.anonymousMqttHolder, sizeof(mqttAnonymous));
  strlcpy(mqttAlert, mqttConfig.alertMqttHolder, sizeof(mqttAlert));
  strlcpy(mqttUsername, mqttConfig.usernameMqttHolder, sizeof(mqttUsername));
  strlcpy(mqttPassword, mqttConfig.passwordMqttHolder, sizeof(mqttPassword));
  strlcpy(mqttServerAddress, mqttConfig.serverAddressMqttHolder, sizeof(mqttServerAddress));
  strlcpy(mqttServerPort, mqttConfig.serverPortMqttHolder, sizeof(mqttServerPort));
  strlcpy(mqttTopicPrefix, mqttConfig.topicPrefixMqttHolder, sizeof(mqttTopicPrefix));
  strlcpy(mqttHaDiscovery, mqttConfig.haDiscoveryMqttHolder, sizeof(mqttHaDiscovery));
  strlcpy(mqttMessagesEnable, mqttConfig.mqttMessagesEnableHolder, sizeof(mqttMessagesEnable));
#ifdef ESP32
  strlcpy(tlsEnabledMqtt, mqttConfig.tlsEnabledMqttHolder, sizeof(tlsEnabledMqtt));
  tlsEnabled = (strcmp(tlsEnabledMqtt, "on") == 0);
#endif

  // Dump config file
  PRINTS("MQTT config changed.\nPrinting config file:\n");
  printMqttFile(mqttConfigFile);

  if (strcmp(mqttOnOff, "on") == 0) {
    PRINTS("\n");
    Serial.print(clientId);
    PRINTS("MQTT Enabled!");
    sprintf(mqttStatusMsg,  "MQTT ENABLED. ");
  }
  else if (strcmp(mqttOnOff, "off") == 0) {
    mqttClient.disconnect();
    PRINTS("\n");
    Serial.print(clientId);
    PRINTS("MQTT Disable!");
    strcpy(mqttStatusMsg, "MQTT DISABLED. ");
    snprintf(mqttAlertMessage, sizeof(mqttAlertMessage), "%sMQTT disconnected from Server: %s:%s", mqttStatusMsg, mqttServerAddress, mqttServerPort);
    mqttDisconnectedProc(mqttAlertMessage, true);
    mqttStatusMsg[0] = '\0';
  }
}

void initMqttStoreConfig() {
  //load config stored in config file
  Serial.println(F("Loading Mqtt configuration...\n"));
  loadMqttConfiguration(mqttConfigFile, mqttConfig);
  //if no onoff is defined in config file store default
  if ((mqttConfig.onOffMqttHolder != NULL) && (mqttConfig.onOffMqttHolder[0] == '\0')) {
    PRINT("no onoff set, setting default onoff: ", mqttOnOff);
    strlcpy(mqttConfig.onOffMqttHolder, mqttOnOff, sizeof(mqttConfig.onOffMqttHolder));
    saveMqttConfigAtStart = true;
  }
  //if no anonymous is defined in config file store default
  if ((mqttConfig.anonymousMqttHolder != NULL) && (mqttConfig.anonymousMqttHolder[0] == '\0')) {
    PRINT("no anonymous set, setting default anonymous: ", mqttAnonymous);
    strlcpy(mqttConfig.anonymousMqttHolder, mqttAnonymous, sizeof(mqttConfig.anonymousMqttHolder));
    saveMqttConfigAtStart = true;
  }
  //if no alert is defined in config file store default
  if ((mqttConfig.alertMqttHolder != NULL) && (mqttConfig.alertMqttHolder[0] == '\0')) {
    PRINT("no alert set, setting default alert: ", mqttAlert);
    strlcpy(mqttConfig.alertMqttHolder, mqttAlert, sizeof(mqttConfig.alertMqttHolder));
    saveMqttConfigAtStart = true;
  }
  //if no username is defined in config file store default
  if ((mqttConfig.usernameMqttHolder != NULL) && (mqttConfig.usernameMqttHolder[0] == '\0')) {
    PRINTS("\n")
    PRINT("no username set, setting default username: ", mqttUsername);
    strlcpy(mqttConfig.usernameMqttHolder, mqttUsername, sizeof(mqttConfig.usernameMqttHolder));
    saveMqttConfigAtStart = true;
  }
  //if no password is defined in config file store default
  if ((mqttConfig.passwordMqttHolder != NULL) && (mqttConfig.passwordMqttHolder[0] == '\0')) {
    PRINTS("\n")
    PRINT("no password set, setting default password: ", mqttPassword);
    strlcpy(mqttConfig.passwordMqttHolder, mqttPassword, sizeof(mqttConfig.passwordMqttHolder));
    saveMqttConfigAtStart = true;
  }
  //if no mqtt server address is defined in config file store default
  if ((mqttConfig.serverAddressMqttHolder != NULL) && (mqttConfig.serverAddressMqttHolder[0] == '\0')) {
    PRINTS("\n")
    PRINT("no mqtt server address set, setting default mqtt server address: ", mqttServerAddress);
    strlcpy(mqttConfig.serverAddressMqttHolder, mqttServerAddress, sizeof(mqttConfig.serverAddressMqttHolder));
    saveMqttConfigAtStart = true;
  }
  //if no mqtt server port is defined in config file store default
  if ((mqttConfig.serverPortMqttHolder != NULL) && (mqttConfig.serverPortMqttHolder[0] == '\0')) {
    PRINTS("\n")
    PRINT("no mqtt server port set, setting default mqtt server port: ", mqttServerPort);
    strlcpy(mqttConfig.serverPortMqttHolder, mqttServerPort, sizeof(mqttConfig.serverPortMqttHolder));
    saveMqttConfigAtStart = true;
  }
  //if no mqtt topic prefix is defined in config file store default
  if ((mqttConfig.topicPrefixMqttHolder != NULL) && (mqttConfig.topicPrefixMqttHolder[0] == '\0')) {
    PRINTS("\n")
    PRINT("no mqtt topic prefix set, setting default mqtt topic prefix: ", mqttTopicPrefix);
    strlcpy(mqttConfig.topicPrefixMqttHolder, mqttTopicPrefix, sizeof(mqttConfig.topicPrefixMqttHolder));
    saveMqttConfigAtStart = true;
  }
  //if no HA discovery is defined in config file store default
  if ((mqttConfig.haDiscoveryMqttHolder != NULL) && (mqttConfig.haDiscoveryMqttHolder[0] == '\0')) {
    PRINTS("\n")
    PRINT("no HA discovery set, setting default HA discovery: ", mqttHaDiscovery);
    strlcpy(mqttConfig.haDiscoveryMqttHolder, mqttHaDiscovery, sizeof(mqttConfig.haDiscoveryMqttHolder));
    saveMqttConfigAtStart = true;
  }
  //if no MQTT Messages Enable is defined in config file store default
  if ((mqttConfig.mqttMessagesEnableHolder != NULL) && (mqttConfig.mqttMessagesEnableHolder[0] == '\0')) {
    PRINTS("\n")
    PRINTS("no MQTT Messages Enable set, setting default: on");
    strlcpy(mqttConfig.mqttMessagesEnableHolder, "on", sizeof(mqttConfig.mqttMessagesEnableHolder));
    saveMqttConfigAtStart = true;
  }
#ifdef ESP32
  //if no TLS enabled is defined in config file store default
  if ((mqttConfig.tlsEnabledMqttHolder != NULL) && (mqttConfig.tlsEnabledMqttHolder[0] == '\0')) {
    PRINTS("\n")
    PRINT("no TLS enabled set, setting default TLS enabled: ", tlsEnabledMqtt);
    strlcpy(mqttConfig.tlsEnabledMqttHolder, tlsEnabledMqtt, sizeof(mqttConfig.tlsEnabledMqttHolder));
    saveMqttConfigAtStart = true;
  }
#endif
  PRINTS("\n")
  strlcpy(mqttOnOff, mqttConfig.onOffMqttHolder, sizeof(mqttOnOff));
  strlcpy(mqttAnonymous, mqttConfig.anonymousMqttHolder, sizeof(mqttAnonymous));
  strlcpy(mqttAlert, mqttConfig.alertMqttHolder, sizeof(mqttAlert));
  strlcpy(mqttUsername, mqttConfig.usernameMqttHolder, sizeof(mqttUsername));
  strlcpy(mqttPassword, mqttConfig.passwordMqttHolder, sizeof(mqttPassword));
  strlcpy(mqttServerAddress, mqttConfig.serverAddressMqttHolder, sizeof(mqttServerAddress));
  strlcpy(mqttServerPort, mqttConfig.serverPortMqttHolder, sizeof(mqttServerPort));
  strlcpy(mqttTopicPrefix, mqttConfig.topicPrefixMqttHolder, sizeof(mqttTopicPrefix));
  strlcpy(mqttHaDiscovery, mqttConfig.haDiscoveryMqttHolder, sizeof(mqttHaDiscovery));
  strlcpy(mqttMessagesEnable, mqttConfig.mqttMessagesEnableHolder, sizeof(mqttMessagesEnable));
  PRINT("\nMQTT Messages Enable: ", mqttMessagesEnable);
#ifdef ESP32
  strlcpy(tlsEnabledMqtt, mqttConfig.tlsEnabledMqttHolder, sizeof(tlsEnabledMqtt));
  tlsEnabled = (strcmp(tlsEnabledMqtt, "on") == 0);
#endif

  // Create mqtt configuration file
  if (saveMqttConfigAtStart) {
    Serial.println(F("Saving mqtt configuration..."));
    saveMqttConfiguration(mqttConfigFile, mqttConfig);
  }

  // Dump mqtt config file
  Serial.println(F("Print mqtt config file...\n"));
  printMqttFile(mqttConfigFile);
}

void initMqtt(){
  if (strcmp(mqttOnOff, "on") == 0) {
    mqttDisconnected = 0;
    
#ifdef ESP32
    // ESP32: Choose client based on TLS setting
    if (tlsEnabled) {
      // Always use secure client container, even if cert loading fails (prevents fallback to retained plain client)
      mqttClient.setClient(espClientSecure);
      
      // Load CA certificate from LittleFS
      if (LittleFS.exists("/ca_cert.pem")) {
        File ca = LittleFS.open("/ca_cert.pem", "r");
        if (ca) {
          String cert = ca.readString();
          espClientSecure.setCACert(cert.c_str());
          ca.close();
          PRINTS("\nTLS enabled - using secure MQTT connection");
        } else {
          PRINTS("\nERROR: Failed to open CA certificate file");
          // Strict Mode: Do NOT fall back to plain
          PRINTS("\nTLS Connection aborted due to cert error");
        }
      } else {
        PRINTS("\nWARNING: TLS enabled but no CA certificate found at /ca_cert.pem");
        PRINTS("\nTLS Connection aborted due to missing cert");
      }
    } else {
      // TLS disabled, use plain client
      mqttClient.setClient(espClientPlain);
      PRINTS("\nTLS disabled - using plain MQTT connection");
    }
#elif defined(ESP8266)
    // ESP8266: Always use plain client
    mqttClient.setClient(espClient);
#endif
    
    mqttClient.setServer(mqttServerAddress, atoi(mqttServerPort));  //1883 or 8883
    mqttClient.setCallback(mqttCallBack);
    mqttClient.setBufferSize(1024);
  }
}

void checkMqtt() {
  if (newMqttOnOffAvailable) {
    newMqttOnOffAvailable = false;
    if (mqttClient.connected()) {
      mqttClient.disconnect();
    }
    else {
      mqttDisconnected = 1;
    }
    if (strcmp(mqttOnOff, "on") == 0) {
      initMqtt();
    }
  }
  if (mqttDisconnected == 1) {
    if (((newMqttOnOffAvailable == false) and (firstMessage == true)) or ((newMqttOnOffAvailable == true) and (firstMessage == false))){
      mqttStatusMsg[0] = '\0';
      snprintf(mqttAlertMessage, sizeof(mqttAlertMessage), "%sMQTT disconnected from Server: %s:%s", mqttStatusMsg, mqttServerAddress, mqttServerPort);
      mqttDisconnectedProc(mqttAlertMessage, true);
    }
  }
  if (strcmp(mqttOnOff, "on") == 0) {
    if (!mqttClient.connected()) {
      long now = millis();
      if ((unsigned long)(now - mqttLastReconnectAttempt) > mqttConnectTimeIntervall) {
        mqttLastReconnectAttempt = now;
        if (reconnectMqtt()) {
          mqttLastReconnectAttempt = 0;
        }
      }
    }
    mqttClient.loop();
  }
}