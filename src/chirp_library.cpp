#include "chirp_library.h"
#include <string.h>

/**
 * Pre-defined chirp patterns for timer alerts and alarms.
 *
 * Each pattern has:
 * - name: Unique identifier used in configuration
 * - description: Human-readable explanation
 * - noteCount: Number of notes in the pattern
 * - notes[]: Array of {frequency, duration} pairs
 *
 * Frequency guidelines:
 * - Low tones: 200-500 Hz (calm, gentle)
 * - Mid tones: 500-1000 Hz (neutral, clear)
 * - High tones: 1000-2000 Hz (urgent, attention-grabbing)
 *
 * Common musical note frequencies (for reference):
 * C4=262, D4=294, E4=330, F4=349, G4=392, A4=440, B4=494
 * C5=523, D5=587, E5=659, F5=698, G5=784, A5=880, B5=988
 * C6=1047, D6=1175, E6=1319
 */

// Reduced chirp set for ESP8266 memory constraints
const ChirpPattern CHIRP_PATTERNS[] = {
    // 0: Silent - No sound
    {"Silent", "No sound", 0, {}},

    // 1: Fast Beep - Legacy message alert behavior (10ms HIGH/LOW cycles)
    {"Fast Beep", "Fast repeated beeps", 1, {{1000, 10}}},

    // 2: Simple Beep - Use alertBuzzerCount
    {"Simple Beep", "Simple beeps", 0, {}},

    // 3: Gentle Dawn - Soft ascending tones
    {"Gentle Dawn", "Soft ascending", 3, {{400, 300}, {550, 300}, {700, 400}}},

    // 4: Cheerful - Happy bell-like
    {"Cheerful", "Happy chime", 5, {{523, 150}, {659, 150}, {784, 150}, {1047, 200}, {784, 250}}},

    // 5: Urgent - Fast high tones
    {"Urgent", "Fast alert", 8, {{1500, 100}, {1800, 100}, {1500, 100}, {1800, 100}, {1500, 100}, {1800, 100}, {1500, 100}, {1800, 150}}},

    // 6: Beep - Classic electronic
    {"Beep", "Simple beep", 3, {{1000, 150}, {0, 100}, {1000, 150}}},

    // 7: Quick Tap - Single short
    {"Quick Tap", "Single beep", 1, {{1000, 100}}},

    // 8: Double - Two beeps
    {"Double", "Two beeps", 3, {{800, 100}, {0, 80}, {800, 100}}},

    // 9: Triple - Three tones
    {"Triple", "Three tones", 5, {{600, 200}, {0, 100}, {750, 200}, {0, 100}, {900, 250}}},

    // 10: Doorbell - Ding-dong
    {"Doorbell", "Ding-dong", 2, {{659, 400}, {523, 500}}},

    // 11: Alarm - Traditional
    {"Alarm", "Alarm beeps", 10, {{1000, 100}, {0, 50}, {1000, 100}, {0, 50}, {1000, 100}, {0, 200}, {1000, 100}, {0, 50}, {1000, 100}, {0, 50}}},

    // 12: Victory - Triumphant
    {"Victory", "Fanfare", 6, {{392, 150}, {523, 150}, {659, 150}, {784, 300}, {659, 150}, {784, 400}}},

    // 13: Notify - App-style
    {"Notify", "App alert", 2, {{880, 100}, {1175, 150}}},

    // 14: For Elise - Classic melody
    {"For Elise", "Classic opening", 9, {{659, 120}, {622, 120}, {659, 120}, {622, 120}, {659, 120}, {494, 180}, {587, 180}, {523, 180}, {440, 600}}},

    // 15: Mario Bros - Game theme
    {"Mario Bros", "Game opening", 12, {{659, 150}, {659, 150}, {0, 150}, {659, 150}, {0, 150}, {523, 150}, {659, 150}, {0, 150}, {784, 300}, {0, 300}, {392, 300}, {0, 150}}},

    // 16: Imperial March - Star Wars villain theme
    {"Imperial March", "Star Wars theme", 10, {{392, 500}, {392, 500}, {392, 500}, {311, 350}, {466, 150}, {392, 500}, {311, 350}, {466, 150}, {392, 1000}, {0, 100}}},

    // 17: Nokia Ringtone - Classic phone
    {"Nokia Ringtone", "Classic phone", 14, {{659, 125}, {587, 125}, {370, 250}, {415, 250}, {554, 125}, {494, 125}, {294, 250}, {330, 250}, {494, 125}, {440, 125}, {277, 250}, {330, 250}, {440, 500}, {0, 100}}},

    // 18: Tetris Theme - Game music (opening)
    {"Tetris Theme", "Game music", 20, {{659, 200}, {494, 100}, {523, 100}, {587, 200}, {523, 100}, {494, 100}, {440, 200}, {440, 100}, {523, 100}, {659, 200}, {587, 100}, {523, 100}, {494, 300}, {523, 100}, {587, 200}, {659, 200}, {523, 200}, {440, 200}, {440, 400}, {0, 100}}},

    // 19: Zelda Secret - Discovery sound
    {"Zelda Secret", "Discovery", 8, {{784, 100}, {740, 100}, {622, 100}, {440, 100}, {415, 100}, {659, 100}, {831, 100}, {1047, 500}}},

    // 20: Windows XP Startup - Nostalgic
    {"Windows XP", "Startup sound", 6, {{523, 150}, {659, 150}, {784, 150}, {1047, 300}, {784, 150}, {1047, 400}}},

    // 21: iPhone Marimba - Modern alert
    {"iPhone Marimba", "Modern alert", 8, {{1047, 100}, {1175, 100}, {1319, 100}, {1397, 150}, {1319, 100}, {1175, 100}, {1047, 150}, {880, 200}}},

    // 22: Pac-Man Intro - Arcade classic
    {"Pac-Man Intro", "Arcade intro", 8, {{494, 150}, {988, 150}, {740, 150}, {622, 150}, {988, 150}, {740, 150}, {622, 200}, {0, 100}}},

    // 23: Star Trek Beep - Sci-fi beep
    {"Star Trek Beep", "Communicator", 2, {{523, 100}, {784, 150}}},

    // 24: R2-D2 Beep - Robot sound
    {"R2-D2 Beep", "Robot sound", 6, {{1500, 80}, {1800, 60}, {1200, 100}, {1600, 80}, {1400, 90}, {1700, 120}}},

    // 25: Close Encounters - 5-note sequence
    {"Close Encounters", "5-note sequence", 5, {{392, 400}, {440, 400}, {349, 400}, {175, 400}, {262, 800}}},

    // 26: Minecraft Theme - Sweden (C418)
    {"Minecraft Theme", "Sweden (C418)", 7, {{294, 400}, {370, 400}, {440, 800}, {0, 400}, {294, 400}, {370, 400}, {330, 800}}},

    // 27: Pitfall! Yodel - Retro game swing
    {"Pitfall! Yodel", "Retro game swing", 8, {{1047, 100}, {784, 100}, {1047, 100}, {880, 100}, {1319, 100}, {1047, 100}, {784, 150}, {523, 200}}},

    // 28: William Tell - Overture Finale
    {"William Tell", "Overture Finale", 11, {{659, 100}, {659, 100}, {659, 100}, {659, 100}, {523, 200}, {587, 200}, {659, 100}, {659, 100}, {659, 100}, {659, 100}, {523, 100}}},

    // 29: Matrix Alarm - Neo's wake up call
    {"Matrix Alarm", "Classic digital", 8, {{1850, 300}, {0, 200}, {1850, 300}, {0, 200}, {1850, 300}, {0, 200}, {1850, 300}, {0, 200}}},

    // 30: CTU Ring - Cisco IP Phone (24)
    {"24 CTU Ring", "Cisco IP Phone", 12, {{698, 50}, {0, 50}, {2093, 50}, {0, 50}, {698, 50}, {0, 50}, {2093, 50}, {0, 400}, {698, 50}, {0, 50}, {2093, 50}, {0, 50}}}
};

// Calculate count at compile time
const uint8_t CHIRP_COUNT = sizeof(CHIRP_PATTERNS) / sizeof(CHIRP_PATTERNS[0]);

const ChirpPattern* getChirpByName(const char* name) {
    if (name == nullptr) return nullptr;

    for (uint8_t i = 0; i < CHIRP_COUNT; i++) {
        if (strcmp(CHIRP_PATTERNS[i].name, name) == 0) {
            return &CHIRP_PATTERNS[i];
        }
    }
    return nullptr;
}

const ChirpPattern* getChirpByIndex(int index) {
    if (index < 0 || index >= CHIRP_COUNT) {
        return nullptr;
    }
    return &CHIRP_PATTERNS[index];
}

int getChirpCount() {
    return CHIRP_COUNT;
}

unsigned long getChirpDuration(const ChirpPattern* pattern) {
    if (pattern == nullptr) return 0;

    unsigned long total = 0;
    for (uint8_t i = 0; i < pattern->noteCount; i++) {
        total += pattern->notes[i].duration;
        // Add inter-note gap (except after last note)
        if (i < pattern->noteCount - 1) {
            total += 30; // 30ms gap between notes
        }
    }
    return total;
}

int getChirpIndexByName(const char* name) {
    if (name == nullptr) return -1;

    for (uint8_t i = 0; i < CHIRP_COUNT; i++) {
        if (strcmp(CHIRP_PATTERNS[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}
