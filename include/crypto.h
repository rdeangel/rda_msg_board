#ifndef CRYPTO_H
#define CRYPTO_H

#include "config.h"

#ifndef DISABLE_CRYPTO_FEATURE

// Crypto price ticker update and display functions
void updateCrypto();                             // Main update function called from loop
void fetchCryptoData(bool forceRefresh = false); // Trigger HTTPS fetch (non-blocking on ESP32 via task)
#ifdef ESP32
void initCryptoTask();                           // Create FreeRTOS fetch task (call once from setup)
#endif
void displayCrypto(bool withAnimation);          // Display crypto prices on LED
bool shouldDisplayCrypto();                      // Check if crypto should be shown
String getCryptoStatusJson();                    // JSON status for web UI
String getCryptoConfigJson();                    // JSON config for web UI

#endif // DISABLE_CRYPTO_FEATURE

#endif // CRYPTO_H
