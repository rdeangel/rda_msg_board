#ifndef DISABLE_CRYPTO_FEATURE
#include "mqtt_discovery_crypto.h"
#include "mqtt_discovery_core.h"
#include "globals.h"
#include "mqtt.h"
#include "config_manager.h"

// Publish all crypto MQTT discovery messages
void publishCryptoDiscovery() {
  if (strcmp(mqttOnOff, "on") != 0 || strcmp(mqttHaDiscovery, "on") != 0) {
    return;
  }

  char baseTopic[256];
  snprintf(baseTopic, sizeof(baseTopic), "%s/ha", mqttTopicDevice);

  // Crypto Enable Switch
  {
    JsonDocument doc;
    doc["~"] = baseTopic;
    doc["name"] = "Crypto Enable";
    doc["uniq_id"] = String(haBaseTopic) + "_crypto_enable";
    doc["cmd_t"] = "~/crypto_enable/set";
    doc["stat_t"] = "~/crypto_enable/state";
    doc["payload_on"] = "ON";
    doc["payload_off"] = "OFF";
    doc["icon"] = "mdi:bitcoin";
    doc["ent_cat"] = "config";

    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "switch", "crypto_enable");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished crypto_enable discovery to: ", topic);
  }

  // Crypto Prices Sensor
  {
    JsonDocument doc;
    doc["~"] = baseTopic;
    doc["name"] = "Crypto Prices";
    doc["uniq_id"] = String(haBaseTopic) + "_crypto_prices";
    doc["stat_t"] = "~/crypto_prices/state";
    doc["icon"] = "mdi:currency-usd";

    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "sensor", "crypto_prices");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished crypto_prices discovery to: ", topic);
  }

  // Crypto Currency Sensor (diagnostic)
  {
    JsonDocument doc;
    doc["~"] = baseTopic;
    doc["name"] = "Crypto Currency";
    doc["uniq_id"] = String(haBaseTopic) + "_crypto_currency";
    doc["stat_t"] = "~/crypto_currency/state";
    doc["icon"] = "mdi:cash";
    doc["ent_cat"] = "diagnostic";

    addDeviceInfo(doc);
    addAvailability(doc);

    char topic[256];
    buildDiscoveryTopic(topic, sizeof(topic), "sensor", "crypto_currency");
    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));
    mqttClient.publish(topic, payload, true);
    PRINT("\nPublished crypto_currency discovery to: ", topic);
  }
}

// Publish current crypto prices state
void publishCryptoPriceState() {
  if (strcmp(mqttOnOff, "on") != 0) return;

  char topic[256];
  snprintf(topic, sizeof(topic), "%s/ha/crypto_prices/state", mqttTopicDevice);

  if (cryptoDataValid && strlen(cryptoPriceBuffer) > 0) {
    mqttClient.publish(topic, cryptoPriceBuffer, true);
  } else {
    mqttClient.publish(topic, "unavailable", true);
  }
}

// Publish crypto enabled state
void publishCryptoEnabledState() {
  if (strcmp(mqttOnOff, "on") != 0) return;

  char topic[256];
  snprintf(topic, sizeof(topic), "%s/ha/crypto_enable/state", mqttTopicDevice);
  mqttClient.publish(topic, cryptoEnabled ? "ON" : "OFF", true);
}

// Publish crypto currency state
void publishCryptoCurrencyState() {
  if (strcmp(mqttOnOff, "on") != 0) return;

  char topic[256];
  snprintf(topic, sizeof(topic), "%s/ha/crypto_currency/state", mqttTopicDevice);
  mqttClient.publish(topic, cryptoConfig.currency, true);
}

// Publish all crypto states
void publishCryptoState() {
  publishCryptoEnabledState();
  publishCryptoPriceState();
  publishCryptoCurrencyState();
}

#endif // DISABLE_CRYPTO_FEATURE
