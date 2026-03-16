#ifdef ESP32

#include "http_task.h"
#include "globals.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static void httpTask(void* pvParameters) {
  while (true) {
    serverHttp.handleClient();
    vTaskDelay(pdMS_TO_TICKS(1));
  }
}

void initHttpTask() {
  xTaskCreatePinnedToCore(
    httpTask,
    "HttpTask",
    10240,  // 10KB stack — accommodates JSON serialisation and HTML String building
    nullptr,
    2,      // Priority 2 — responsive to user requests; display loop is priority 1 on core 1
    nullptr,
    0       // Core 0, separate from the display loop on core 1
  );
}

#endif // ESP32
