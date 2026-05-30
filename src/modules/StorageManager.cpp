#include "StorageManager.h"
#include "core/Logger.h"
#include <LittleFS.h>
#include <ArduinoJson.h>

void StorageManager::begin(BootAction action) {
    LOGI("Storage", "Initializing LittleFS...");

    if (!LittleFS.begin(true)) {
        LOGE("Storage", "Failed to mount LittleFS");
        mounted_ = false;
        return;
    }

    mounted_ = true;
    LOGI("Storage", "LittleFS mounted successfully");

    // Handle boot action
    switch (action) {
        case BootAction::SafeRollback:
            LOGW("Storage", "Safe boot: restoring last known good config");
            if (!restoreLastGood()) {
                LOGW("Storage", "Failed to restore last good, using defaults");
            }
            break;

        case BootAction::FactoryReset:
            LOGW("Storage", "Factory reset: clearing all config");
            clearAll();
            break;

        case BootAction::Normal:
        default:
            if (!loadConfig()) {
                LOGW("Storage", "Failed to load config, using defaults");
            }
            break;
    }
}

bool StorageManager::loadConfig() {
    if (!mounted_) {
        LOGE("Storage", "Cannot load config: filesystem not mounted");
        return false;
    }

    File file = LittleFS.open(kConfigPath, "r");
    if (!file) {
        LOGW("Storage", "Config file not found, using defaults");
        config_loaded_ = false;
        return false;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        LOGE("Storage", "Failed to parse config: %s", error.c_str());
        return false;
    }

    // Load WiFi settings
    strlcpy(config_.wifi_ssid, doc["wifi_ssid"] | "", sizeof(config_.wifi_ssid));
    strlcpy(config_.wifi_password, doc["wifi_password"] | "", sizeof(config_.wifi_password));
    config_.wifi_enabled = doc["wifi_enabled"] | false;

    // Load MQTT settings
    strlcpy(config_.mqtt_host, doc["mqtt_host"] | "", sizeof(config_.mqtt_host));
    config_.mqtt_port = doc["mqtt_port"] | Config::DEFAULT_MQTT_PORT;
    strlcpy(config_.mqtt_user, doc["mqtt_user"] | "", sizeof(config_.mqtt_user));
    strlcpy(config_.mqtt_password, doc["mqtt_password"] | "", sizeof(config_.mqtt_password));
    strlcpy(config_.mqtt_base_topic, doc["mqtt_base_topic"] | Config::DEFAULT_MQTT_BASE_TOPIC, sizeof(config_.mqtt_base_topic));
    strlcpy(config_.mqtt_device_name, doc["mqtt_device_name"] | Config::APP_NAME, sizeof(config_.mqtt_device_name));
    config_.mqtt_enabled = doc["mqtt_enabled"] | false;
    config_.mqtt_discovery = doc["mqtt_discovery"] | true;

    // Load sensor settings
    config_.temp_offset = doc["temp_offset"] | Config::TEMP_OFFSET_DEFAULT;
    config_.hum_offset = doc["hum_offset"] | Config::HUM_OFFSET_DEFAULT;
    config_.temp_units_celsius = doc["temp_units_celsius"] | true;

    // Load display settings
    config_.display_enabled = doc["display_enabled"] | true;
    config_.display_brightness = doc["display_brightness"] | 128;

    LOGI("Storage", "Config loaded successfully");
    config_loaded_ = true;
    return true;
}

bool StorageManager::saveConfigInternal() {
    if (!mounted_) {
        LOGE("Storage", "Cannot save config: filesystem not mounted");
        return false;
    }

    JsonDocument doc;
    doc["wifi_ssid"] = config_.wifi_ssid;
    doc["wifi_password"] = config_.wifi_password;
    doc["wifi_enabled"] = config_.wifi_enabled;

    // MQTT settings
    doc["mqtt_host"] = config_.mqtt_host;
    doc["mqtt_port"] = config_.mqtt_port;
    doc["mqtt_user"] = config_.mqtt_user;
    doc["mqtt_password"] = config_.mqtt_password;
    doc["mqtt_base_topic"] = config_.mqtt_base_topic;
    doc["mqtt_device_name"] = config_.mqtt_device_name;
    doc["mqtt_enabled"] = config_.mqtt_enabled;
    doc["mqtt_discovery"] = config_.mqtt_discovery;

    // Sensor settings
    doc["temp_offset"] = config_.temp_offset;
    doc["hum_offset"] = config_.hum_offset;
    doc["temp_units_celsius"] = config_.temp_units_celsius;

    // Display settings
    doc["display_enabled"] = config_.display_enabled;
    doc["display_brightness"] = config_.display_brightness;

    File file = LittleFS.open(kConfigPath, "w");
    if (!file) {
        LOGE("Storage", "Failed to open config file for writing");
        return false;
    }

    if (serializeJson(doc, file) == 0) {
        LOGE("Storage", "Failed to write config");
        file.close();
        return false;
    }

    file.close();
    LOGI("Storage", "Config saved successfully");
    return true;
}

void StorageManager::requestSave() {
    markDirty();
}

void StorageManager::markDirty() {
    dirty_ = true;
    last_save_ms_ = millis();
}

void StorageManager::poll(uint32_t now_ms) {
    if (!dirty_) return;

    // Debounce: wait for no changes for debounce_ms
    if (now_ms - last_save_ms_ < debounce_ms_) return;

    saveConfigInternal();
    dirty_ = false;
}

bool StorageManager::saveConfig(bool force) {
    if (force) {
        return saveConfigInternal();
    } else {
        requestSave();
        return true;
    }
}

bool StorageManager::commitLastGood() {
    if (!mounted_) return false;

    // Copy current config to last good
    if (LittleFS.exists(kConfigPath)) {
        File src = LittleFS.open(kConfigPath, "r");
        File dst = LittleFS.open(kLastGoodPath, "w");

        if (src && dst) {
            uint8_t buf[128];
            while (src.available()) {
                size_t len = src.read(buf, sizeof(buf));
                dst.write(buf, len);
            }
            src.close();
            dst.close();
            LOGI("Storage", "Last known good config saved");
            return true;
        }
    }

    return false;
}

bool StorageManager::restoreLastGood() {
    if (!mounted_) return false;

    if (LittleFS.exists(kLastGoodPath)) {
        // Copy last good to current
        File src = LittleFS.open(kLastGoodPath, "r");
        File dst = LittleFS.open(kConfigPath, "w");

        if (src && dst) {
            uint8_t buf[128];
            while (src.available()) {
                size_t len = src.read(buf, sizeof(buf));
                dst.write(buf, len);
            }
            src.close();
            dst.close();

            // Reload config
            return loadConfig();
        }
    }

    return false;
}

void StorageManager::clearAll() {
    if (!mounted_) return;

    if (LittleFS.exists(kConfigPath)) {
        LittleFS.remove(kConfigPath);
    }
    if (LittleFS.exists(kLastGoodPath)) {
        LittleFS.remove(kLastGoodPath);
    }

    // Reset to defaults
    config_ = Config::StoredConfig();
    config_loaded_ = false;

    LOGI("Storage", "All config cleared");
}
