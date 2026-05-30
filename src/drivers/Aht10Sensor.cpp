#include "Aht10Sensor.h"
#include "core/Logger.h"
#include <Wire.h>

bool Aht10Sensor::begin(uint8_t i2c_addr) {
    i2c_addr_ = i2c_addr;

    LOGI("AHT10", "Initializing AHT10 sensor at address 0x%02X", i2c_addr_);

    // Wait for sensor to be ready after power-on
    delay(40);

    // Soft reset
    if (!sendCommand(CMD_SOFT_RESET)) {
        LOGE("AHT10", "Soft reset failed");
        return false;
    }
    delay(20);

    // Check if sensor is present
    Wire.beginTransmission(i2c_addr_);
    if (Wire.endTransmission() != 0) {
        LOGE("AHT10", "Sensor not found at address 0x%02X", i2c_addr_);
        return false;
    }

    // Initialize and calibrate
    if (!calibrate()) {
        LOGE("AHT10", "Calibration failed");
        return false;
    }

    initialized_ = true;
    LOGI("AHT10", "Sensor initialized successfully");
    return true;
}

bool Aht10Sensor::calibrate() {
    // Send calibration command
    if (!sendCommand(CMD_INIT, 0x08, 0x00)) {
        return false;
    }

    delay(10);

    // Check calibration status
    uint8_t status;
    if (!readData(&status, 1)) {
        return false;
    }

    if (!(status & STATUS_CALIBRATED)) {
        LOGE("AHT10", "Sensor not calibrated (status: 0x%02X)", status);
        return false;
    }

    LOGI("AHT10", "Sensor calibrated (status: 0x%02X)", status);
    return true;
}

bool Aht10Sensor::read(float &temperature, float &humidity) {
    if (!initialized_) {
        LOGE("AHT10", "Sensor not initialized");
        return false;
    }

    // Rate limiting
    uint32_t now = millis();
    if (now - last_read_ms_ < MIN_READ_INTERVAL_MS) {
        return false;
    }
    last_read_ms_ = now;

    // Trigger measurement
    if (!sendCommand(CMD_TRIGGER, 0x33, 0x00)) {
        LOGE("AHT10", "Failed to trigger measurement");
        return false;
    }

    // Wait for measurement to complete (typical 75ms)
    delay(80);

    // Read 6 bytes of data
    uint8_t data[6];
    if (!readData(data, 6)) {
        LOGE("AHT10", "Failed to read data");
        return false;
    }

    // Check if sensor is busy
    if (data[0] & STATUS_BUSY) {
        LOGW("AHT10", "Sensor busy, retrying...");
        delay(10);
        if (!readData(data, 6)) {
            return false;
        }
    }

    // Extract humidity (20 bits)
    uint32_t humidity_raw = ((uint32_t)data[1] << 12) |
                            ((uint32_t)data[2] << 4) |
                            ((uint32_t)data[3] >> 4);

    // Extract temperature (20 bits)
    uint32_t temperature_raw = (((uint32_t)data[3] & 0x0F) << 16) |
                               ((uint32_t)data[4] << 8) |
                               ((uint32_t)data[5]);

    // Convert to actual values
    humidity = (humidity_raw * 100.0f) / 1048576.0f;
    temperature = ((temperature_raw * 200.0f) / 1048576.0f) - 50.0f;

    LOGD("AHT10", "Read: T=%.1f°C, H=%.1f%% (raw: T=%u, H=%u)",
         temperature, humidity, temperature_raw, humidity_raw);

    return true;
}

bool Aht10Sensor::sendCommand(uint8_t cmd, uint8_t param1, uint8_t param2) {
    Wire.beginTransmission(i2c_addr_);
    Wire.write(cmd);
    if (param1 != 0 || param2 != 0) {
        Wire.write(param1);
        Wire.write(param2);
    }
    return (Wire.endTransmission() == 0);
}

bool Aht10Sensor::readData(uint8_t *data, uint8_t len) {
    Wire.requestFrom(i2c_addr_, len);

    uint8_t count = 0;
    while (Wire.available() && count < len) {
        data[count++] = Wire.read();
    }

    return (count == len);
}
