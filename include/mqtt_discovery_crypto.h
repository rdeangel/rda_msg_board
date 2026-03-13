#ifndef MQTT_DISCOVERY_CRYPTO_H
#define MQTT_DISCOVERY_CRYPTO_H

#include "config.h"

#ifndef DISABLE_CRYPTO_FEATURE

// MQTT Discovery functions for Home Assistant Crypto Price integration
void publishCryptoDiscovery();     // Publish discovery topics for crypto entities
void publishCryptoEnabledState();  // Publish enabled state
void publishCryptoPriceState();    // Publish current price string
void publishCryptoCurrencyState(); // Publish configured currency
void publishCryptoState();         // Publish all crypto states

#endif // DISABLE_CRYPTO_FEATURE

#endif // MQTT_DISCOVERY_CRYPTO_H
