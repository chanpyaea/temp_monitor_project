#pragma once

#include <stdint.h>

// Sensor data structure
struct SensorData {
    float temperature_c;
    float humidity;
    bool has_temperature;
    bool has_humidity;
    uint32_t last_read_ms;

    SensorData() :
        temperature_c(0.0f),
        humidity(0.0f),
        has_temperature(false),
        has_humidity(false),
        last_read_ms(0)
    {}
};

// System status
struct SystemStatus {
    uint32_t uptime_seconds;
    uint32_t free_heap;
    uint32_t min_free_heap;
    bool wifi_connected;
    bool mqtt_connected;
    int8_t wifi_rssi;

    SystemStatus() :
        uptime_seconds(0),
        free_heap(0),
        min_free_heap(0),
        wifi_connected(false),
        mqtt_connected(false),
        wifi_rssi(0)
    {}
};

// History entry for data logging
struct HistoryEntry {
    uint32_t timestamp;
    float temperature_c;
    float humidity;
    bool valid;

    HistoryEntry() :
        timestamp(0),
        temperature_c(0.0f),
        humidity(0.0f),
        valid(false)
    {}

    HistoryEntry(uint32_t ts, float temp, float hum) :
        timestamp(ts),
        temperature_c(temp),
        humidity(hum),
        valid(true)
    {}
};

// Data statistics for history
struct DataStats {
    float temp_min;
    float temp_max;
    float temp_avg;
    float hum_min;
    float hum_max;
    float hum_avg;
    uint16_t sample_count;

    DataStats() :
        temp_min(999.0f),
        temp_max(-999.0f),
        temp_avg(0.0f),
        hum_min(999.0f),
        hum_max(-999.0f),
        hum_avg(0.0f),
        sample_count(0)
    {}
};
