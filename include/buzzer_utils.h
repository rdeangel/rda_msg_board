#ifndef BUZZER_UTILS_H
#define BUZZER_UTILS_H

#include "config.h"
#include "chirp_library.h"

// Buzzer Control Functions
// Buzzer Control Functions
void wifiModeBuzzer();
void startupBuzzer();
void mqttConnectBuzzer();
void mqttDisconnectBuzzer();

// Non-blocking delay helper
void yieldDelay(unsigned long ms);

// Chirp Playback Functions
// playChirp() is always blocking (used internally on ESP8266).
// playChirpByName() is non-blocking on ESP32 (posts to FreeRTOS buzzer task queue)
// and blocking on ESP8266.

/**
 * Play a chirp pattern synchronously (blocking on all platforms).
 * Respects buzzer enable setting and sleep mode.
 * @param pattern Pointer to the chirp pattern to play
 * @param repeatCount Number of times to repeat the chirp (1-20, default 1)
 */
void playChirp(const ChirpPattern* pattern, int repeatCount = 1);

/**
 * Play a chirp pattern by name.
 * ESP32: non-blocking — posts to FreeRTOS buzzer task queue and returns immediately.
 * ESP8266: blocking — plays entire pattern before returning.
 * Respects buzzer enable setting and sleep mode.
 * @param chirpName Name of the chirp pattern
 * @param repeatCount Number of times to repeat the chirp (1-20, default 1)
 * @return true if chirp was found (and queued/played), false if not found
 */
bool playChirpByName(const char* chirpName, int repeatCount = 1);

#endif // BUZZER_UTILS_H
