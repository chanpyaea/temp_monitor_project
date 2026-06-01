#pragma once

#include <stdint.h>

namespace Config {
    // Application info
    constexpr const char* APP_NAME = "TempMonitor";
    constexpr const char* APP_VERSION = "1.1.1";

    // Timing constants
    constexpr uint32_t SENSOR_POLL_INTERVAL_MS = 2000;      // Read sensor every 2 seconds
    constexpr uint32_t DISPLAY_UPDATE_INTERVAL_MS = 1000;   // Update display every 1 second
    constexpr uint32_t MQTT_PUBLISH_INTERVAL_MS = 30000;    // Publish every 30 seconds
    constexpr uint32_t WIFI_CONNECT_TIMEOUT_MS = 30000;     // 30 second WiFi timeout
    constexpr uint32_t STORAGE_SAVE_DEBOUNCE_MS = 5000;     // Wait 5s before saving config

    // Safe boot configuration
    constexpr uint32_t SAFE_BOOT_STABLE_MS = 60000;         // 60 seconds = stable boot
    constexpr uint8_t SAFE_BOOT_MAX_REBOOTS = 3;            // 3 crashes = safe boot

    // Watchdog
    constexpr uint32_t WATCHDOG_TIMEOUT_MS = 120000;        // 2 minute watchdog

    // Memory monitoring
    constexpr uint32_t MEM_LOG_INTERVAL_MS = 60000;         // Log memory every minute

    // I2C pins (adjust for your ESP32 board)
    constexpr int I2C_SDA_PIN = 21;
    constexpr int I2C_SCL_PIN = 22;
    constexpr uint32_t I2C_FREQ = 400000;                   // 400kHz

    // Temperature sensors (supports both!)
    constexpr uint8_t AHT10_I2C_ADDR = 0x38;                // AHT10 I2C address
    constexpr int DHT_SENSOR_PIN = 4;                       // DHT22/DHT11 GPIO pin

    // Sensor auto-detection priority
    // 1. Try AHT10 first (I2C, more accurate)
    // 2. Fall back to DHT22 if AHT10 not found

    // Display (SSD1306 OLED)
    constexpr int DISPLAY_WIDTH = 128;
    constexpr int DISPLAY_HEIGHT = 64;
    constexpr uint8_t DISPLAY_I2C_ADDR = 0x3C;

    // WiFi
    constexpr const char* DEFAULT_AP_SSID = "TempMonitor-Setup";
    constexpr const char* DEFAULT_AP_PASS = "12345678";
    constexpr const char* DEFAULT_HOSTNAME = "tempmonitor";

    // MQTT
    constexpr uint16_t DEFAULT_MQTT_PORT = 1883;
    constexpr const char* DEFAULT_MQTT_BASE_TOPIC = "tempmonitor";

    // Logging
    constexpr int LOG_LEVEL = 3; // 0=None, 1=Error, 2=Warn, 3=Info, 4=Debug
    constexpr bool LOG_SERIAL_OUTPUT = true;

    // Temperature settings
    constexpr float TEMP_OFFSET_DEFAULT = 0.0f;
    constexpr float HUM_OFFSET_DEFAULT = 0.0f;
    constexpr float TEMP_MIN = -40.0f;
    constexpr float TEMP_MAX = 125.0f;
    constexpr float HUM_MIN = 0.0f;
    constexpr float HUM_MAX = 100.0f;

    // Configuration structure
    struct StoredConfig {
        // WiFi settings
        char wifi_ssid[64];
        char wifi_password[64];
        bool wifi_enabled;

        // MQTT settings
        char mqtt_host[128];
        uint16_t mqtt_port;
        char mqtt_user[64];
        char mqtt_password[64];
        char mqtt_base_topic[64];
        char mqtt_device_name[32];
        bool mqtt_enabled;
        bool mqtt_discovery;

        // Sensor settings
        float temp_offset;
        float hum_offset;
        bool temp_units_celsius;

        // Display settings
        bool display_enabled;
        uint8_t display_brightness;

        // Constructor with defaults
        StoredConfig() :
            wifi_enabled(false),
            mqtt_port(DEFAULT_MQTT_PORT),
            mqtt_enabled(false),
            mqtt_discovery(true),
            temp_offset(TEMP_OFFSET_DEFAULT),
            hum_offset(HUM_OFFSET_DEFAULT),
            temp_units_celsius(true),
            display_enabled(true),
            display_brightness(128)
        {
            wifi_ssid[0] = '\0';
            wifi_password[0] = '\0';
            mqtt_host[0] = '\0';
            mqtt_user[0] = '\0';
            mqtt_password[0] = '\0';
            strncpy(mqtt_base_topic, DEFAULT_MQTT_BASE_TOPIC, sizeof(mqtt_base_topic));
            strncpy(mqtt_device_name, APP_NAME, sizeof(mqtt_device_name));
        }
    };
}
