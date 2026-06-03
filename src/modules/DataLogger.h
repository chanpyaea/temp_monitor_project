#pragma once

#include <Arduino.h>
#include "config/AppData.h"

class DataLogger {
public:
    static constexpr uint16_t HISTORY_SIZE = 288;  // 24 hours at 5-minute intervals
    static constexpr uint32_t LOG_INTERVAL_MS = 300000;  // 5 minutes

    void begin();
    void poll(uint32_t now, const SensorData &data);

    // Query statistics
    DataStats getStats() const { return stats_; }

    // Query history
    uint16_t getHistoryCount() const { return count_; }
    const HistoryEntry* getHistory() const { return history_; }
    uint16_t getHistoryHead() const { return head_; }

    // Export functions
    void exportJSON(String &output) const;
    void exportCSV(String &output) const;

    // Clear history
    void clear();

private:
    void addEntry(const SensorData &data, uint32_t timestamp);
    void calculateStats();

    HistoryEntry history_[HISTORY_SIZE];
    DataStats stats_;
    uint16_t head_;
    uint16_t count_;
    uint32_t last_log_ms_;
};
