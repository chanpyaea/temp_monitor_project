#pragma once

#include <Arduino.h>

// Simple DHT sensor driver (DHT22/DHT11)
class DhtSensor {
public:
    bool begin(uint8_t pin);
    bool read(float &temperature, float &humidity);
    bool isPresent() const { return initialized_; }

private:
    uint8_t pin_ = 0;
    bool initialized_ = false;
    uint32_t last_read_ms_ = 0;
    static constexpr uint32_t MIN_READ_INTERVAL_MS = 2000;
};
