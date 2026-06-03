#include "DataLogger.h"
#include "core/Logger.h"
#include <ArduinoJson.h>

void DataLogger::begin() {
    head_ = 0;
    count_ = 0;
    last_log_ms_ = 0;

    for (uint16_t i = 0; i < HISTORY_SIZE; i++) {
        history_[i].valid = false;
    }

    stats_ = DataStats();

    LOGI("DataLogger", "Initialized - capacity: %d entries (24h @ 5min intervals)", HISTORY_SIZE);
}

void DataLogger::poll(uint32_t now, const SensorData &data) {
    if (!data.has_temperature && !data.has_humidity) {
        return;
    }

    // Check if it's time to log
    if (last_log_ms_ == 0 || (now - last_log_ms_ >= LOG_INTERVAL_MS)) {
        addEntry(data, now / 1000);
        last_log_ms_ = now;
        LOGD("DataLogger", "Logged entry %d/%d - T:%.1f°C H:%.1f%%",
             count_, HISTORY_SIZE, data.temperature_c, data.humidity);
    }
}

void DataLogger::addEntry(const SensorData &data, uint32_t timestamp) {
    // Add entry to circular buffer
    history_[head_] = HistoryEntry(timestamp, data.temperature_c, data.humidity);

    head_ = (head_ + 1) % HISTORY_SIZE;
    if (count_ < HISTORY_SIZE) {
        count_++;
    }

    // Recalculate statistics
    calculateStats();
}

void DataLogger::calculateStats() {
    if (count_ == 0) {
        stats_ = DataStats();
        return;
    }

    float temp_sum = 0.0f;
    float hum_sum = 0.0f;
    uint16_t valid_count = 0;

    stats_.temp_min = 999.0f;
    stats_.temp_max = -999.0f;
    stats_.hum_min = 999.0f;
    stats_.hum_max = -999.0f;

    // Iterate through valid entries
    for (uint16_t i = 0; i < count_; i++) {
        const HistoryEntry &entry = history_[i];
        if (entry.valid) {
            // Temperature stats
            if (entry.temperature_c < stats_.temp_min) {
                stats_.temp_min = entry.temperature_c;
            }
            if (entry.temperature_c > stats_.temp_max) {
                stats_.temp_max = entry.temperature_c;
            }
            temp_sum += entry.temperature_c;

            // Humidity stats
            if (entry.humidity < stats_.hum_min) {
                stats_.hum_min = entry.humidity;
            }
            if (entry.humidity > stats_.hum_max) {
                stats_.hum_max = entry.humidity;
            }
            hum_sum += entry.humidity;

            valid_count++;
        }
    }

    if (valid_count > 0) {
        stats_.temp_avg = temp_sum / valid_count;
        stats_.hum_avg = hum_sum / valid_count;
        stats_.sample_count = valid_count;
    } else {
        stats_ = DataStats();
    }
}

void DataLogger::exportJSON(String &output) const {
    JsonDocument doc;

    doc["count"] = count_;
    doc["capacity"] = HISTORY_SIZE;
    doc["interval_sec"] = LOG_INTERVAL_MS / 1000;

    // Add statistics
    JsonObject stats = doc["statistics"].to<JsonObject>();
    stats["temp_min"] = stats_.temp_min;
    stats["temp_max"] = stats_.temp_max;
    stats["temp_avg"] = stats_.temp_avg;
    stats["hum_min"] = stats_.hum_min;
    stats["hum_max"] = stats_.hum_max;
    stats["hum_avg"] = stats_.hum_avg;
    stats["sample_count"] = stats_.sample_count;

    // Add history entries
    JsonArray entries = doc["history"].to<JsonArray>();

    // Start from oldest entry (if buffer is full)
    uint16_t start_idx = (count_ == HISTORY_SIZE) ? head_ : 0;

    for (uint16_t i = 0; i < count_; i++) {
        uint16_t idx = (start_idx + i) % HISTORY_SIZE;
        const HistoryEntry &entry = history_[idx];

        if (entry.valid) {
            JsonObject obj = entries.add<JsonObject>();
            obj["ts"] = entry.timestamp;
            obj["temp"] = entry.temperature_c;
            obj["hum"] = entry.humidity;
        }
    }

    serializeJson(doc, output);
}

void DataLogger::exportCSV(String &output) const {
    // CSV Header
    output = "Timestamp,Temperature_C,Humidity_Percent\n";

    // Start from oldest entry
    uint16_t start_idx = (count_ == HISTORY_SIZE) ? head_ : 0;

    for (uint16_t i = 0; i < count_; i++) {
        uint16_t idx = (start_idx + i) % HISTORY_SIZE;
        const HistoryEntry &entry = history_[idx];

        if (entry.valid) {
            char line[64];
            snprintf(line, sizeof(line), "%lu,%.2f,%.2f\n",
                     (unsigned long)entry.timestamp,
                     entry.temperature_c,
                     entry.humidity);
            output += line;
        }
    }
}

void DataLogger::clear() {
    head_ = 0;
    count_ = 0;
    last_log_ms_ = 0;

    for (uint16_t i = 0; i < HISTORY_SIZE; i++) {
        history_[i].valid = false;
    }

    stats_ = DataStats();

    LOGI("DataLogger", "History cleared");
}
