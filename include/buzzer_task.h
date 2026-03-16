#ifndef BUZZER_TASK_H
#define BUZZER_TASK_H

#ifdef ESP32

#include "chirp_library.h"

// Initialize the FreeRTOS buzzer task and queue.
// Must be called from setup() after ledcSetup/ledcAttachPin.
void initBuzzerTask();

// Post a chirp request to the buzzer task queue.
// Returns immediately; sound plays concurrently in a background task.
// Silently discards if the queue is already full (previous request pending).
bool postBuzzerRequest(const ChirpPattern* pattern, int repeats);

// Returns true while the buzzer task is actively playing a chirp.
bool isBuzzerPlaying();

#endif // ESP32

#endif // BUZZER_TASK_H
