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

// Chirp Playback Functions (blocking)
// These functions play the entire chirp pattern before returning

/**
 * Play a chirp pattern (blocking).
 * Respects buzzer enable setting and sleep mode.
 * @param pattern Pointer to the chirp pattern to play
 * @param repeatCount Number of times to repeat the chirp (1-20, default 1)
 */
void playChirp(const ChirpPattern* pattern, int repeatCount = 1);

/**
 * Play a chirp pattern by name (blocking).
 * Looks up the pattern and plays it.
 * @param chirpName Name of the chirp pattern
 * @param repeatCount Number of times to repeat the chirp (1-20, default 1)
 * @return true if chirp was found and played, false if not found
 */
bool playChirpByName(const char* chirpName, int repeatCount = 1);

#endif // BUZZER_UTILS_H
