#ifdef ESP32

#include "buzzer_task.h"
#include "config.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

struct BuzzerQueueItem {
  const ChirpPattern* pattern;
  int repeats;
};

static QueueHandle_t buzzerQueue = nullptr;
static volatile bool buzzerPlaying = false;

bool isBuzzerPlaying() {
  return buzzerPlaying;
}

static void buzzerTask(void* pvParameters) {
  BuzzerQueueItem item;
  while (true) {
    if (xQueueReceive(buzzerQueue, &item, portMAX_DELAY) == pdTRUE) {
      if (item.pattern == nullptr || item.pattern->noteCount == 0) continue;

      buzzerPlaying = true;

      bool isFastBeep = (strcmp(item.pattern->name, "Fast Beep") == 0);

      if (isFastBeep) {
        for (int rep = 0; rep < item.repeats; rep++) {
          ledcWriteTone(0, 1000);
          vTaskDelay(pdMS_TO_TICKS(10));
          ledcWriteTone(0, 0);
          vTaskDelay(pdMS_TO_TICKS(10));
        }
      } else {
        for (int rep = 0; rep < item.repeats; rep++) {
          for (uint8_t i = 0; i < item.pattern->noteCount; i++) {
            uint16_t freq = item.pattern->notes[i].frequency;
            uint16_t dur  = item.pattern->notes[i].duration;
            if (freq == 0) {
              vTaskDelay(pdMS_TO_TICKS(dur));
            } else {
              ledcWriteTone(0, freq);
              vTaskDelay(pdMS_TO_TICKS(dur));
              ledcWriteTone(0, 0);
            }
            if (i < item.pattern->noteCount - 1) {
              vTaskDelay(pdMS_TO_TICKS(30));
            }
          }
          if (rep < item.repeats - 1) {
            vTaskDelay(pdMS_TO_TICKS(500));
          }
        }
        ledcWriteTone(0, 0);
      }

      buzzerPlaying = false;
    }
  }
}

bool postBuzzerRequest(const ChirpPattern* pattern, int repeats) {
  if (buzzerQueue == nullptr) return false;
  BuzzerQueueItem item = { pattern, repeats };
  return xQueueSend(buzzerQueue, &item, 0) == pdTRUE;
}

void initBuzzerTask() {
  buzzerQueue = xQueueCreate(1, sizeof(BuzzerQueueItem));
  xTaskCreatePinnedToCore(
    buzzerTask,
    "BuzzerTask",
    3072,
    nullptr,
    1,
    nullptr,
    0
  );
}

#endif // ESP32
