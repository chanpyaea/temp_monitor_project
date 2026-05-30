#include <Arduino.h>
#include <Wire.h>
#include <esp_system.h>

#include "config/AppConfig.h"
#include "config/AppData.h"
#include "core/Logger.h"
#include "core/BootPolicy.h"
#include "core/Watchdog.h"
#include "core/MemoryMonitor.h"

#include "modules/StorageManager.h"
#include "modules/SensorManager.h"
#include "modules/NetworkManager.h"
#include "modules/MqttManager.h"

#include "ui/DisplayManager.h"

// Global managers (static allocation)
StorageManager storage;
SensorManager sensorManager;
NetworkManager networkManager;
MqttManager mqttManager;
DisplayManager displayManager;
MemoryMonitor memoryMonitor;

// Global data
SensorData currentData;
SystemStatus systemStatus;

// Boot state
uint32_t boot_start_ms = 0;
uint32_t boot_count = 0;
uint32_t safe_boot_stage = 0;
bool boot_stable = false;

// Helper function to check reset reason
bool isCrashReset(esp_reset_reason_t reason) {
    return (reason == ESP_RST_PANIC ||
            reason == ESP_RST_INT_WDT ||
            reason == ESP_RST_TASK_WDT ||
            reason == ESP_RST_WDT);
}

void setup() {
    delay(1000);
    Serial.begin(115200);
    delay(500);

    // Initialize logging
    Logger::begin(Serial, static_cast<Logger::Level>(Config::LOG_LEVEL));
    LOGI("Main", "=== Temperature Monitor v%s ===", Config::APP_VERSION);

    // Check reset reason and apply boot policy
    esp_reset_reason_t reset_reason = esp_reset_reason();
    bool crash_reset = isCrashReset(reset_reason);

    LOGI("Main", "Reset reason: %d, Crash: %s", reset_reason, crash_reset ? "YES" : "NO");

    StorageManager::BootAction boot_action =
        BootPolicy::apply(crash_reset, boot_count, safe_boot_stage, Config::SAFE_BOOT_MAX_REBOOTS);

    boot_start_ms = millis();

    // Initialize I2C
    LOGI("Main", "Initializing I2C (SDA=%d, SCL=%d)", Config::I2C_SDA_PIN, Config::I2C_SCL_PIN);
    Wire.begin(Config::I2C_SDA_PIN, Config::I2C_SCL_PIN);
    Wire.setClock(Config::I2C_FREQ);

    // Initialize storage
    storage.begin(boot_action);

    // Initialize display
    displayManager.begin();
    displayManager.showBootScreen();
    delay(2000);

    // Initialize sensor manager (auto-detects AHT10 or DHT22)
    sensorManager.begin(storage.config().temp_offset, storage.config().hum_offset);

    // Initialize network manager
    networkManager.begin(storage);

    // Initialize MQTT manager
    mqttManager.begin(storage, networkManager);

    // Initialize memory monitor
    memoryMonitor.begin(Config::MEM_LOG_INTERVAL_MS);
    memoryMonitor.logNow("boot");

    // Setup watchdog
    Watchdog::setup(Config::WATCHDOG_TIMEOUT_MS);

    LOGI("Main", "Initialization complete");
    LOGI("Main", "Free heap: %u bytes", ESP.getFreeHeap());
}

void loop() {
    uint32_t now = millis();

    // Update system status
    systemStatus.uptime_seconds = now / 1000;
    systemStatus.free_heap = ESP.getFreeHeap();
    systemStatus.min_free_heap = ESP.getMinFreeHeap();
    systemStatus.wifi_connected = networkManager.isConnected();
    systemStatus.mqtt_connected = mqttManager.isConnected();
    systemStatus.wifi_rssi = networkManager.rssi();

    // Poll sensor manager
    SensorManager::PollResult sensor_result = sensorManager.poll(currentData);

    if (sensor_result.data_changed) {
        LOGI("Main", "Sensor data updated");
    }

    // Poll network manager
    networkManager.poll();

    // Poll MQTT manager
    mqttManager.poll();

    // Publish to MQTT if connected
    if (mqttManager.isConnected()) {
        mqttManager.publish(currentData, systemStatus);
    }

    // Update display
    displayManager.update(currentData, systemStatus);

    // Poll storage (for debounced saves)
    storage.poll(now);

    // Check if boot is stable
    BootPolicy::markStable(now, boot_start_ms, Config::SAFE_BOOT_STABLE_MS,
                          boot_stable, boot_count, safe_boot_stage);

    // If boot just became stable, save last known good config
    static bool last_boot_stable = false;
    if (boot_stable && !last_boot_stable) {
        LOGI("Main", "Boot stable, saving last known good config");
        storage.commitLastGood();
    }
    last_boot_stable = boot_stable;

    // Memory monitoring
    memoryMonitor.poll(now);

    // Reset watchdog
    Watchdog::kick();

    // Small delay to yield to other tasks
    delay(10);
}
