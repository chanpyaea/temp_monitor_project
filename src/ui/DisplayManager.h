#pragma once

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include "config/AppData.h"

class DisplayManager {
public:
    void begin();
    void update(const SensorData &data, const SystemStatus &status);
    void showBootScreen();
    void showError(const char *message);
    void clear();
    void setBrightness(uint8_t brightness);
    bool isOk() const { return display_ok_; }

private:
    void drawTemperature(const SensorData &data);
    void drawHumidity(const SensorData &data);
    void drawStatus(const SystemStatus &status);

    Adafruit_SSD1306 *display_ = nullptr;
    bool display_ok_ = false;
    uint32_t last_update_ms_ = 0;
};
