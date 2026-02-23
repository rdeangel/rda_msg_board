#ifndef CHIRP_LIBRARY_H
#define CHIRP_LIBRARY_H

#include <Arduino.h>

// Maximum notes per chirp pattern
#define MAX_CHIRP_NOTES 20

/**
 * Represents a single note in a chirp pattern.
 * frequency: Tone frequency in Hz (0 = silence/pause)
 * duration: Note duration in milliseconds
 */
struct ChirpNote {
    uint16_t frequency;  // Hz (0 for silence)
    uint16_t duration;   // ms
};

/**
 * Represents a complete chirp pattern with metadata.
 * Used for timer alerts and future alarm functionality.
 */
struct ChirpPattern {
    const char* name;           // Unique identifier for selection
    const char* description;    // Human-readable description
    uint8_t noteCount;          // Number of notes in pattern
    ChirpNote notes[MAX_CHIRP_NOTES];
};

// Total number of available chirp patterns
extern const uint8_t CHIRP_COUNT;

// Array of all pre-defined chirp patterns
extern const ChirpPattern CHIRP_PATTERNS[];

/**
 * Get a chirp pattern by its name.
 * @param name The name of the chirp to find
 * @return Pointer to the ChirpPattern, or nullptr if not found
 */
const ChirpPattern* getChirpByName(const char* name);

/**
 * Get a chirp pattern by its index.
 * @param index Index in the CHIRP_PATTERNS array (0-based)
 * @return Pointer to the ChirpPattern, or nullptr if index out of range
 */
const ChirpPattern* getChirpByIndex(int index);

/**
 * Get the total number of available chirp patterns.
 * @return Number of patterns in the library
 */
int getChirpCount();

/**
 * Calculate the total duration of a chirp pattern in milliseconds.
 * @param pattern Pointer to the chirp pattern
 * @return Total duration in ms, or 0 if pattern is null
 */
unsigned long getChirpDuration(const ChirpPattern* pattern);

/**
 * Get the index of a chirp pattern by name.
 * @param name The name of the chirp to find
 * @return Index in CHIRP_PATTERNS, or -1 if not found
 */
int getChirpIndexByName(const char* name);

#endif // CHIRP_LIBRARY_H
