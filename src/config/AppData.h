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
