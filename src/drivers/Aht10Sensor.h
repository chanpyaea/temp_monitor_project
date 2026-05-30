#pragma once

#include <Arduino.h>

// AHT10 I2C Temperature and Humidity Sensor
class Aht10Sensor {
public:
    bool begin(uint8_t i2c_addr = 0x38);
    bool read(float &temperature, float &humidity);
    bool isPresent() const { return initialized_; }

private:
    bool sendCommand(uint8_t cmd, uint8_t param1 = 0, uint8_t param2 = 0);
    bool readData(uint8_t *data, uint8_t len);
    bool calibrate();

    uint8_t i2c_addr_ = 0x38;
    bool initialized_ = false;
    uint32_t last_read_ms_ = 0;
    static constexpr uint32_t MIN_READ_INTERVAL_MS = 1000; // AHT10 needs 1s between reads

    // AHT10 Commands
    static constexpr uint8_t CMD_INIT = 0xE1;
    static constexpr uint8_t CMD_TRIGGER = 0xAC;
    static constexpr uint8_t CMD_SOFT_RESET = 0xBA;
    static constexpr uint8_t STATUS_BUSY = 0x80;
    static constexpr uint8_t STATUS_CALIBRATED = 0x08;
};
