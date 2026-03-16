#include "buzzer_utils.h"
#include "globals.h"
#include "functions.h"
#include "web_server.h"
#include "chirp_library.h"
#ifdef ESP32
#include "buzzer_task.h"
#endif

void wifiModeBuzzer() {
  if (strcmp(generalConfig.buzzerEnable, "off") == 0) return;
  #ifndef DISABLE_SLEEP_MODE_FEATURE
  if (isSleepModeActive()) return;
  #endif
  PRINTS("\nBUZZ");
  #ifdef ESP32
  ledcWriteTone(0, 1000);  // 1kHz tone
  delay(1000);
  ledcWriteTone(0, 0);
  #else
  digitalWrite(BUZZER_PIN, HIGH);
  delay(1000);
  digitalWrite(BUZZER_PIN, LOW);
  #endif
}

void startupBuzzer() {
  if (strcmp(generalConfig.buzzerEnable, "off") == 0) return;
  #ifndef DISABLE_SLEEP_MODE_FEATURE
  if (isSleepModeActive()) return;
  #endif
  PRINTS("\nBUZZ");
  // Simple short beep to avoid blocking startup
  #ifdef ESP32
  ledcWriteTone(0, 1000);  // 1kHz tone
  delay(100);
  ledcWriteTone(0, 0);
  #else
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
  #endif
}

void mqttConnectBuzzer() {
  if (strcmp(generalConfig.buzzerEnable, "off") == 0) return;
  #ifndef DISABLE_SLEEP_MODE_FEATURE
  if (isSleepModeActive()) return;
  #endif
  PRINTS("\nBUZZ");
  for (int i = 0; i < 10; i++) {
    #ifdef ESP32
    ledcWriteTone(0, 1000);
    delay(30);
    ledcWriteTone(0, 0);
    #else
    digitalWrite(BUZZER_PIN, HIGH);
    delay(30);
    digitalWrite(BUZZER_PIN, LOW);
    #endif
    delay(30);
  }
}

void mqttDisconnectBuzzer() {
  if (strcmp(generalConfig.buzzerEnable, "off") == 0) return;
  #ifndef DISABLE_SLEEP_MODE_FEATURE
  if (isSleepModeActive()) return;
  #endif
  PRINTS("\nBUZZ");
  for (int i = 0; i < 2; i++) {
    #ifdef ESP32
    ledcWriteTone(0, 1000);
    delay(500);
    ledcWriteTone(0, 0);
    #else
    digitalWrite(BUZZER_PIN, HIGH);
    delay(500);
    digitalWrite(BUZZER_PIN, LOW);
    #endif
    delay(500);
  }
}

// Non-blocking delay that keeps web server responsive
void yieldDelay(unsigned long ms) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    handleHttpServer(); // no-op on ESP32 (HTTP task owns handleClient there)
    delay(1); // Allow WiFi stack to process
    yield();
  }
}

void playChirp(const ChirpPattern* pattern, int repeatCount) {
  // Check if buzzer is globally disabled
  if (strcmp(generalConfig.buzzerEnable, "off") == 0) return;

  // Check sleep mode
  #ifndef DISABLE_SLEEP_MODE_FEATURE
  if (isSleepModeActive()) return;
  #endif

  // Null check or empty pattern
  if (pattern == nullptr || pattern->noteCount == 0) return;

  // Validate repeat count (1-20)
  if (repeatCount < 1) repeatCount = 1;
  if (repeatCount > 20) repeatCount = 20;

  PRINTS("\nPlaying chirp: ");
  Serial.print(pattern->name);
  PRINT(" (repeat: ", repeatCount);
  PRINTS(")\n");

  // Special case: "Fast Beep" uses legacy rapid beep behavior (10ms HIGH/LOW cycles)
  // This matches the original message alert buzzer pattern
  bool isFastBeep = (strcmp(pattern->name, "Fast Beep") == 0);

  if (isFastBeep) {
    // Legacy behavior: rapid 10ms HIGH/LOW cycles with no gap between repetitions
    PRINTS("[DEBUG] playChirp: Fast Beep detected, using LEDC/digitalWrite\n");
    for (int rep = 0; rep < repeatCount; rep++) {
      #ifdef ESP32
      PRINTS("[DEBUG] ESP32: ledcWriteTone(0, 1000)\n");
      ledcWriteTone(0, 1000);
      delay(10);
      ledcWriteTone(0, 0);
      #else
      PRINTS("[DEBUG] ESP8266: digitalWrite HIGH\n");
      digitalWrite(BUZZER_PIN, HIGH);
      delay(10);
      digitalWrite(BUZZER_PIN, LOW);
      #endif
      delay(10);
    }
    PRINTS("[DEBUG] playChirp: Fast Beep completed\n");
    return;
  }

  // Repeat the entire chirp pattern
  for (int rep = 0; rep < repeatCount; rep++) {
    // Play each note in the pattern
    for (uint8_t i = 0; i < pattern->noteCount; i++) {
      uint16_t freq = pattern->notes[i].frequency;
      uint16_t dur = pattern->notes[i].duration;

      if (freq == 0) {
        // Silence/pause - just delay
        yieldDelay(dur);
      } else {
        // Play tone using tone() if available, otherwise use simple on/off
        #if defined(ESP32)
          // ESP32: Use LEDC for tone generation
          PRINT("[DEBUG] ESP32: ledcWriteTone freq=", freq);
          PRINT(" dur=", dur);
          PRINTS("\n");
          ledcWriteTone(0, freq);
          yieldDelay(dur);
          ledcWriteTone(0, 0);
        #elif defined(ESP8266)
          // ESP8266: Use tone() function
          PRINT("[DEBUG] ESP8266: tone freq=", freq);
          PRINT(" dur=", dur);
          PRINTS("\n");
          tone(BUZZER_PIN, freq, dur);
          yieldDelay(dur);
          noTone(BUZZER_PIN);
        #else
          // Fallback: Simple on/off (no frequency control)
          PRINTS("[DEBUG] Fallback: digitalWrite HIGH\n");
          digitalWrite(BUZZER_PIN, HIGH);
          yieldDelay(dur);
          digitalWrite(BUZZER_PIN, LOW);
        #endif
      }

      // Small gap between notes for clarity (except after last note)
      if (i < pattern->noteCount - 1) {
        yieldDelay(30);
      }
    }

    // Longer delay between repetitions (except after last repetition)
    // This makes the sound ending distinctive
    if (rep < repeatCount - 1) {
      yieldDelay(500); // 500ms gap between chirp repetitions
    }
  }

  // Ensure buzzer is off at the end
  #if defined(ESP32)
    ledcWriteTone(0, 0);
  #else
    noTone(BUZZER_PIN);
    digitalWrite(BUZZER_PIN, LOW);
  #endif
}

bool playChirpByName(const char* chirpName, int repeatCount) {
  if (chirpName == nullptr || chirpName[0] == '\0') {
    return false;
  }

  const ChirpPattern* pattern = getChirpByName(chirpName);
  if (pattern == nullptr) {
    PRINTS("\nChirp not found: ");
    Serial.print(chirpName);
    return false;
  }

#ifdef ESP32
  // Non-blocking path: check guards here since we bypass playChirp()
  if (strcmp(generalConfig.buzzerEnable, "off") == 0) return false;
  #ifndef DISABLE_SLEEP_MODE_FEATURE
  if (isSleepModeActive()) return false;
  #endif
  if (repeatCount < 1) repeatCount = 1;
  if (repeatCount > 20) repeatCount = 20;
  PRINTS("\nPosting chirp to buzzer task: ");
  Serial.print(chirpName);
  return postBuzzerRequest(pattern, repeatCount);
#else
  playChirp(pattern, repeatCount);
  return true;
#endif
}
