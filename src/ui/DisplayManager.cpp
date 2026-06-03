#include "DisplayManager.h"
#include "core/Logger.h"
#include "config/AppConfig.h"
#include <Wire.h>

void DisplayManager::begin() {
    LOGI("Display", "Initializing SSD1306 OLED...");

    display_ = new Adafruit_SSD1306(Config::DISPLAY_WIDTH, Config::DISPLAY_HEIGHT, &Wire, -1);

    if (!display_->begin(SSD1306_SWITCHCAPVCC, Config::DISPLAY_I2C_ADDR)) {
        LOGE("Display", "SSD1306 allocation failed");
        display_ok_ = false;
        return;
    }

    display_ok_ = true;
    display_->clearDisplay();
    display_->setTextColor(SSD1306_WHITE);
    display_->display();

    LOGI("Display", "Display initialized successfully");
}

void DisplayManager::showBootScreen() {
    if (!display_ok_) return;

    display_->clearDisplay();
    display_->setTextSize(2);
    display_->setCursor(10, 10);
    display_->println("Temp");
    display_->println("Monitor");
    display_->setTextSize(1);
    display_->setCursor(10, 50);
    display_->print("v");
    display_->print(Config::APP_VERSION);
    display_->display();
}

void DisplayManager::update(const SensorData &data, const SystemStatus &status) {
    if (!display_ok_) return;

    // Rate limiting
    uint32_t now = millis();
    if (now - last_update_ms_ < Config::DISPLAY_UPDATE_INTERVAL_MS) {
        return;
    }
    last_update_ms_ = now;

    display_->clearDisplay();

    // Draw temperature (large)
    drawTemperature(data);

    // Draw humidity
    drawHumidity(data);

    // Draw status icons
    drawStatus(status);

    display_->display();
}

void DisplayManager::drawTemperature(const SensorData &data) {
    display_->setTextSize(3);
    display_->setCursor(5, 5);

    if (data.has_temperature) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%.1f", data.temperature_c);
        display_->print(buf);
        display_->setTextSize(2);
        display_->print("C");
    } else {
        display_->print("--.-");
    }
}

void DisplayManager::drawHumidity(const SensorData &data) {
    display_->setTextSize(2);
    display_->setCursor(5, 35);

    if (data.has_humidity) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%.0f%%", data.humidity);
        display_->print(buf);
    } else {
        display_->print("--%");
    }
}

void DisplayManager::drawStatus(const SystemStatus &status) {
    display_->setTextSize(1);
    display_->setCursor(0, 56);

    // WiFi status
    if (status.wifi_connected) {
        display_->print("W");
        display_->print(status.wifi_rssi);
    } else {
        display_->print("W-");
    }

    display_->print(" ");

    // MQTT status
    if (status.mqtt_connected) {
        display_->print("M+");
    } else {
        display_->print("M-");
    }

    // Uptime (right aligned)
    display_->setCursor(80, 56);
    uint32_t hours = status.uptime_seconds / 3600;
    uint32_t minutes = (status.uptime_seconds % 3600) / 60;
    char buf[16];
    snprintf(buf, sizeof(buf), "%uh%02um", hours, minutes);
    display_->print(buf);
}

void DisplayManager::showError(const char *message) {
    if (!display_ok_) return;

    display_->clearDisplay();
    display_->setTextSize(1);
    display_->setCursor(0, 0);
    display_->println("ERROR:");
    display_->println();
    display_->println(message);
    display_->display();
}

void DisplayManager::clear() {
    if (!display_ok_) return;
    display_->clearDisplay();
    display_->display();
}

void DisplayManager::setBrightness(uint8_t brightness) {
    if (!display_ok_) return;
    // SSD1306 doesn't have brightness control via library
    // Would need to send raw commands
}

void DisplayManager::updateWithStats(const SensorData &data, const SystemStatus &status, const DataStats &stats) {
    if (!display_ok_) return;

    // Rate limiting
    uint32_t now = millis();
    if (now - last_update_ms_ < Config::DISPLAY_UPDATE_INTERVAL_MS) {
        return;
    }
    last_update_ms_ = now;

    display_->clearDisplay();

    // Draw current temperature (smaller to make room for stats)
    display_->setTextSize(2);
    display_->setCursor(5, 0);
    if (data.has_temperature) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%.1fC", data.temperature_c);
        display_->print(buf);
    } else {
        display_->print("--.-C");
    }

    // Draw current humidity
    display_->setTextSize(1);
    display_->setCursor(90, 5);
    if (data.has_humidity) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%.0f%%", data.humidity);
        display_->print(buf);
    }

    // Draw statistics
    drawStats(stats);

    // Draw status icons
    drawStatus(status);

    display_->display();
}

void DisplayManager::drawStats(const DataStats &stats) {
    if (stats.sample_count == 0) {
        display_->setTextSize(1);
        display_->setCursor(5, 25);
        display_->print("No history data");
        return;
    }

    display_->setTextSize(1);

    // Temperature stats
    display_->setCursor(0, 20);
    display_->print("T:");
    display_->setCursor(15, 20);
    char buf[32];
    snprintf(buf, sizeof(buf), "%.1f/%.1f/%.1f", stats.temp_min, stats.temp_avg, stats.temp_max);
    display_->print(buf);

    // Humidity stats
    display_->setCursor(0, 30);
    display_->print("H:");
    display_->setCursor(15, 30);
    snprintf(buf, sizeof(buf), "%.0f/%.0f/%.0f", stats.hum_min, stats.hum_avg, stats.hum_max);
    display_->print(buf);

    // Sample count
    display_->setCursor(0, 45);
    snprintf(buf, sizeof(buf), "Samples: %d", stats.sample_count);
    display_->print(buf);
}
