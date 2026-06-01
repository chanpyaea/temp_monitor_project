#include "Aht10Sensor.h"
#include "core/Logger.h"
#include <Wire.h>

bool Aht10Sensor::begin(uint8_t i2c_addr) {
    i2c_addr_ = i2c_addr;

    LOGI("AHT10", "Initializing AHT10 sensor at address 0x%02X", i2c_addr_);

    // Check if sensor is present first
    Wire.beginTransmission(i2c_addr_);
    if (Wire.endTransmission() != 0) {
        LOGE("AHT10", "Sensor not found at address 0x%02X", i2c_addr_);
        return false;
    }

    // Wait longer for sensor to be ready after power-on (datasheet: 20-40ms)
    delay(100);

    // Soft reset
    if (!sendCommand(CMD_SOFT_RESET)) {
        LOGE("AHT10", "Soft reset failed");
        return false;
    }
    delay(40);  // Increased delay after reset

    // Read status to verify sensor is responding
    uint8_t status;
    if (!readData(&status, 1)) {
        LOGE("AHT10", "Failed to read status after reset");
        return false;
    }
    LOGI("AHT10", "Status after reset: 0x%02X", status);

    // Initialize and calibrate (retry up to 3 times)
    for (int attempt = 0; attempt < 3; attempt++) {
        if (calibrate()) {
            initialized_ = true;
            LOGI("AHT10", "Sensor initialized successfully");
            return true;
        }
        LOGW("AHT10", "Calibration attempt %d failed, retrying...", attempt + 1);
        delay(50);
    }

    LOGE("AHT10", "Calibration failed after 3 attempts");
    return false;
}

bool Aht10Sensor::calibrate() {
    // Read current status first
    uint8_t status;
    if (!readData(&status, 1)) {
        LOGE("AHT10", "Failed to read initial status");
        return false;
    }

    LOGI("AHT10", "Initial status: 0x%02X (busy=%d, cal=%d)",
         status, (status & STATUS_BUSY) ? 1 : 0, (status & STATUS_CALIBRATED) ? 1 : 0);

    // If already calibrated, skip calibration command
    if (status & STATUS_CALIBRATED) {
        LOGI("AHT10", "Sensor already calibrated, skipping init command");
        return true;
    }

    // Send calibration command (0xE1 0x08 0x00)
    Wire.beginTransmission(i2c_addr_);
    Wire.write(CMD_INIT);
    Wire.write(0x08);
    Wire.write(0x00);
    uint8_t result = Wire.endTransmission();

    if (result != 0) {
        LOGE("AHT10", "Failed to send calibration command (error: %d)", result);
        return false;
    }

    // Wait for calibration to complete (datasheet: 10ms typical)
    delay(50);

    // Check calibration status (read status byte)
    if (!readData(&status, 1)) {
        LOGE("AHT10", "Failed to read calibration status");
        return false;
    }

    LOGI("AHT10", "Calibration status: 0x%02X (busy=%d, cal=%d)",
         status, (status & STATUS_BUSY) ? 1 : 0, (status & STATUS_CALIBRATED) ? 1 : 0);

    // Check if calibrated bit is set (bit 3)
    if (!(status & STATUS_CALIBRATED)) {
        LOGE("AHT10", "Sensor not calibrated (status: 0x%02X)", status);
        return false;
    }

    LOGI("AHT10", "Sensor calibrated successfully");
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
