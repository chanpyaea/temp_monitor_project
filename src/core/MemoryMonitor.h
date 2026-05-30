#pragma once

#include <Arduino.h>

class MemoryMonitor {
public:
    void begin(uint32_t log_interval_ms);
    void poll(uint32_t now_ms);
    void logNow(const char *label = nullptr);

private:
    uint32_t log_interval_ms_ = 60000;
    uint32_t last_log_ms_ = 0;
};
