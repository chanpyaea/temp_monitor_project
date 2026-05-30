#include "Watchdog.h"
#include "core/Logger.h"
#include <esp_task_wdt.h>

void Watchdog::setup(uint32_t timeout_ms) {
    esp_task_wdt_init(timeout_ms / 1000, true); // Enable panic on timeout
    esp_task_wdt_add(NULL); // Add current task
    LOGI("Watchdog", "Enabled with %u ms timeout", timeout_ms);
}

void Watchdog::kick() {
    esp_task_wdt_reset();
}
