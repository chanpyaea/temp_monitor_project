#include "MemoryMonitor.h"
#include "core/Logger.h"

void MemoryMonitor::begin(uint32_t log_interval_ms) {
    log_interval_ms_ = log_interval_ms;
    LOGI("Memory", "Monitor started, interval: %u ms", log_interval_ms);
}

void MemoryMonitor::poll(uint32_t now_ms) {
    if (now_ms - last_log_ms_ >= log_interval_ms_) {
        logNow();
        last_log_ms_ = now_ms;
    }
}

void MemoryMonitor::logNow(const char *label) {
    uint32_t free_heap = ESP.getFreeHeap();
    uint32_t min_free_heap = ESP.getMinFreeHeap();
    uint32_t largest_block = ESP.getMaxAllocHeap();

    LOGI("Memory", "%s: free=%u, min_free=%u, largest=%u",
         label ? label : "periodic",
         free_heap, min_free_heap, largest_block);
}
