#ifndef DISABLE_TIMER_FEATURE
#include "timer.h"
#include "globals.h"
#include "functions.h"
#include "config_manager.h"
#include "buzzer_utils.h"
#include "chirp_library.h"

// Start or resume timer
void startTimer() {
  // Allow start if not running, paused, OR if currently in completion state (restart)
  if (!timerRunning || timerPaused || currentTimerState == TIMER_COMPLETION_ALERT || currentTimerState == TIMER_COMPLETION_DONE) {
    if (timerPaused && currentTimerState == TIMER_PAUSED && timerElapsedSeconds > 0) {
      // Resume from pause - only if there's actual elapsed time (not after reset)
      unsigned long pauseDuration = (millis() - timerPausedTime) / 1000;
      timerStartTime += pauseDuration * 1000;
      timerPaused = false;
      currentTimerState = TIMER_RUNNING;
    } else {
      // Fresh start (or restart from completion or after reset)
      timerStartTime = millis();
      timerElapsedSeconds = 0;
      timerAlertTriggered = false;
      currentTimerState = TIMER_RUNNING;
    }
    timerRunning = true;
    timerPaused = false;

    PRINTS("Timer started\n");
  }
}

// Pause timer
void pauseTimer() {
  if (timerRunning && !timerPaused && currentTimerState == TIMER_RUNNING) {
    timerPaused = true;
    currentTimerState = TIMER_PAUSED;
    timerPausedTime = millis();
    PRINTS("Timer paused\n");
  }
}

// Stop and reset timer
void stopTimer() {
  timerRunning = false;
  timerPaused = false;
  currentTimerState = TIMER_STOPPED;
  timerElapsedSeconds = 0;
  timerAlertTriggered = false;
  PRINTS("Timer stopped\n");
}

// Reset timer to initial state (Paused at 0)
void resetTimer() {
  timerRunning = true;  // Keep running so display mode stays active
  timerPaused = true;   // But paused
  currentTimerState = TIMER_PAUSED;
  timerPausedTime = millis();
  timerElapsedSeconds = 0;
  timerAlertTriggered = false;
  timerTargetSeconds = atoi(timerConfig.durationSeconds);
  PRINTS("Timer reset (paused)\n");
}

// Set timer duration
void setTimerDuration(int hours, int minutes, int seconds) {
  timerTargetSeconds = (hours * 3600) + (minutes * 60) + seconds;
  snprintf(timerConfig.durationSeconds, sizeof(timerConfig.durationSeconds), "%lu", timerTargetSeconds);
  saveTimerConfiguration(timerConfigFile, timerConfig);
  PRINT("Timer duration set to: ", timerTargetSeconds);
  PRINTS(" seconds\n");
}

// Get current timer display string
void getTimerDisplay(char* buffer, size_t size) {
  unsigned long currentSeconds;
  
  if (strcmp(timerConfig.mode, "countdown") == 0) {
    if (timerElapsedSeconds >= timerTargetSeconds) currentSeconds = 0;
    else currentSeconds = timerTargetSeconds - timerElapsedSeconds;
  } else {
    currentSeconds = timerElapsedSeconds;
  }
  
  int hours = currentSeconds / 3600;
  int minutes = (currentSeconds % 3600) / 60;
  int seconds = currentSeconds % 60;
  
  #if MAX_DEVICES == 4
    // Smart formatting for 4 modules to fit 32px
    if (hours > 0) {
       // > 1 hour: Show HHhMM (e.g. "01h30")
       // visual cue: "h" means hours.
       // Static "h" to prevent jumping (widths of 'h' and ' ' differ)
       snprintf(buffer, size, "%02dh%02d", hours, minutes);
    } else {
       // < 1 hour: Show MMmSS (e.g. "58m01")
       // Compact to fit 32px
       snprintf(buffer, size, "%02dm%02d", minutes, seconds);
    }
  #else
    // 8-module display: show HH:MM:SS
    snprintf(buffer, size, "%02d:%02d:%02d", hours, minutes, seconds);
  #endif
}

// Update timer state and display
void updateTimer() {
  // Safety check: specific fixes for user reported issues
  
  // 1. If timer is disabled, force stop and do nothing (fixes "DONE" showing when disabled)
  if (!timerEnabled) {
      if (currentTimerState != TIMER_STOPPED) {
          stopTimer();
      }
      return;
  }

  // Allow update in alert states even if "running" flag logic is complex
  // Only return early if completely stopped or paused (and not in completion sequence)
  if (currentTimerState == TIMER_STOPPED || currentTimerState == TIMER_PAUSED) {
    return;
  }
  
  unsigned long currentMillis = millis();
  
  // Normal running state
  if (currentTimerState == TIMER_RUNNING) {
      // Update elapsed time (once per second)
      if (currentMillis - lastTimerUpdate >= 1000) {
        lastTimerUpdate = currentMillis;
        timerElapsedSeconds = (currentMillis - timerStartTime) / 1000;
        
        // Check for completion in countdown mode
        if (strcmp(timerConfig.mode, "countdown") == 0) {
          // Trigger completion when we've reached or exceeded the target
          // This ensures we show 00:00 before triggering the alert
          if (timerElapsedSeconds >= timerTargetSeconds && !timerAlertTriggered) {
            // Force display to show 00:00
            timerElapsedSeconds = timerTargetSeconds;
            handleTimerCompletion();
          }
        }
      }
  }

  // Handle completion alert (Chirp playback)
  if (currentTimerState == TIMER_COMPLETION_ALERT) {
      static bool chirpStarted = false;

      #ifndef DISABLE_SLEEP_MODE_FEATURE
      bool buzzerEnabled = (strcmp(timerConfig.alertBuzzer, "on") == 0 && strcmp(generalConfig.buzzerEnable, "on") == 0 && !isSleepModeActive());
      #else
      bool buzzerEnabled = (strcmp(timerConfig.alertBuzzer, "on") == 0 && strcmp(generalConfig.buzzerEnable, "on") == 0);
      #endif

      if (!buzzerEnabled) {
          // Skip chirp if disabled
          chirpStarted = false;
          currentTimerState = TIMER_COMPLETION_DONE;
          return;
      }

      if (!chirpStarted) {
          chirpStarted = true;

          // Display "DONE" before playing chirp (since chirp is blocking)
          P.print("DONE");
          yieldDelay(100); // Brief pause to ensure display updates

          // Get repeat count from config
          int repeatCount = atoi(timerConfig.alertBuzzerCount);
          if (repeatCount < 1) repeatCount = 1;
          if (repeatCount > 20) repeatCount = 20;

          // Play the selected chirp pattern (blocking) with repeat count
          if (timerConfig.alertChirp[0] != '\0' && strcmp(timerConfig.alertChirp, "Simple Beep") != 0) {
              PRINTS("Playing timer alert chirp: ");
              Serial.println(timerConfig.alertChirp);
              playChirpByName(timerConfig.alertChirp, repeatCount);
          } else {
              // Legacy fallback: simple beeps based on alertBuzzerCount
              PRINTS("\n[DEBUG] Timer: Playing Simple Beep (using LEDC/digitalWrite)");
              PRINT("Beep count: ", repeatCount);
              PRINTS("\n");
              for (int i = 0; i < repeatCount; i++) {
                  #ifdef ESP32
                  ledcWriteTone(0, 1000);  // 1kHz tone
                  yieldDelay(100);
                  ledcWriteTone(0, 0);
                  #else
                  digitalWrite(BUZZER_PIN, HIGH);
                  yieldDelay(100);
                  digitalWrite(BUZZER_PIN, LOW);
                  #endif
                  yieldDelay(100);
              }
              PRINTS("[DEBUG] Timer Simple Beep completed\n");
          }

          // Move to done state after chirp completes
          chirpStarted = false;
          currentTimerState = TIMER_COMPLETION_DONE;
      }
  }
  
  // Handle non-blocking "DONE" flash
  if (currentTimerState == TIMER_COMPLETION_DONE) {
      static unsigned long lastFlashToggle = 0;
      static int flashCount = 0;
      static bool flashOn = false;
      
      if (currentMillis - lastFlashToggle >= 500) { // 500ms interval
          lastFlashToggle = currentMillis;
          
          if (flashCount < 6) { // 3 blinks (on/off pairs)
              flashOn = !flashOn;
              if (flashOn) P.print("DONE");
              else P.print("    ");
              flashCount++;
          } else {
              // Finished completion sequence
              flashCount = 0;
              
              // Auto-repeat or Stop
              if (strcmp(timerConfig.autoRepeat, "on") == 0) {
                  // Manual reset for fresh start loop
                  timerStartTime = millis();
                  timerElapsedSeconds = 0;
                  timerAlertTriggered = false;
                  currentTimerState = TIMER_RUNNING;
                  PRINTS("Timer auto-restarted\n");
              } else {
                  stopTimer();
              }
          }
      }
  }

}

// Display timer on LED matrix
void displayTimer(bool withAnimation) {
  #ifndef DISABLE_SLEEP_MODE_FEATURE
  if (isSleepModeActive() && !sleepModeMuteOnly) {
    P.displayClear();
    return;
  }
  #endif
  // Static buffer is CRITICAL because P.displayText stores the pointer, not a copy.
  // If we use a local variable, it goes out of scope and displays garbage ("Lay?").
  static char displayBuffer[32] = "";
  static uint8_t lastBrightness = 255;
  
  char currentString[32];
  getTimerDisplay(currentString, sizeof(currentString));

  // Check for changes
  bool contentChanged = (strcmp(displayBuffer, currentString) != 0);
  bool brightnessChanged = (timerBrightness != lastBrightness);
  
  // Update persistent state if changed
  if (contentChanged) {
      strcpy(displayBuffer, currentString);
  }
  if (brightnessChanged) {
      lastBrightness = timerBrightness;
  }
  
  // If nothing changed and not forced, return
  // Note: withAnimation forces a refresh/reset of the display effect
  if (!withAnimation && !contentChanged && !brightnessChanged) {
      return;
  }
  
  // suppress display update if we are in completion logic (unless forced animation override)
  if (!withAnimation && (currentTimerState == TIMER_COMPLETION_ALERT || currentTimerState == TIMER_COMPLETION_DONE)) {
      return;
  }

  // Use standard font
  P.setFont(nullptr);
  
  // Apply timer brightness
  P.setIntensity(getEffectiveBrightness(timerBrightness));
  
  // Show text using the STATIC buffer
  P.displayText(displayBuffer, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
}

// Handle timer completion
// Handle timer completion
void handleTimerCompletion() {
  timerAlertTriggered = true;
  currentTimerState = TIMER_COMPLETION_ALERT;
  
  PRINTS("Timer completed! Starting alert sequence...\n");
}

// Get timer status as JSON string
String getTimerStatusJson() {
  String json = "{";
  json += "\"enabled\":\"" + String(timerConfig.enabled) + "\",";
  json += "\"mode\":\"" + String(timerConfig.mode) + "\",";
  json += "\"durationSeconds\":" + String(timerConfig.durationSeconds) + ",";
  json += "\"brightness\":" + String(timerConfig.brightness) + ",";
  json += "\"alertBuzzer\":\"" + String(timerConfig.alertBuzzer) + "\",";
  json += "\"alertBuzzerCount\":" + String(timerConfig.alertBuzzerCount) + ",";
  json += "\"alertChirp\":\"" + String(timerConfig.alertChirp) + "\",";
  json += "\"autoRepeat\":\"" + String(timerConfig.autoRepeat) + "\",";
  json += "\"running\":" + String(timerRunning ? "true" : "false") + ",";
  json += "\"paused\":" + String(timerPaused ? "true" : "false") + ",";
  json += "\"elapsedSeconds\":" + String(timerElapsedSeconds) + ",";
  json += "\"targetSeconds\":" + String(timerTargetSeconds) + ",";
  json += "\"state\":" + String(currentTimerState) + ",";
  json += "\"alertTriggered\":" + String(timerAlertTriggered ? "true" : "false");
  json += "}";
  return json;
}
#endif // DISABLE_TIMER_FEATURE
