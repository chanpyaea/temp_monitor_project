#pragma once

#include <Arduino.h>
#include "config/AppData.h"

class SensorManager {
public:
    enum SensorType {
        SENSOR_NONE = 0,
        SENSOR_AHT10,
        SENSOR_DHT22
    };

    struct PollResult {
        bool data_changed = false;
        bool error_occurred = false;
    };

    void begin(float temp_offset, float hum_offset);
    PollResult poll(SensorData &data);

    void setOffsets(float temp_offset, float hum_offset);
    bool isOk() const { return sensor_ok_; }
    uint32_t lastReadMs() const { return last_read_ms_; }
    SensorType sensorType() const { return sensor_type_; }
    const char* sensorName() const;

private:
    bool readSensor(float &temp, float &hum);

    float temp_offset_ = 0.0f;
    float hum_offset_ = 0.0f;
    bool sensor_ok_ = false;
    uint32_t last_read_ms_ = 0;
    uint32_t last_poll_ms_ = 0;
    float last_temp_ = 0.0f;
    float last_hum_ = 0.0f;
    SensorType sensor_type_ = SENSOR_NONE;
};
