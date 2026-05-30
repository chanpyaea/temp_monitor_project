#pragma once

#include <Arduino.h>
#include "config/AppConfig.h"

class StorageManager {
public:
    enum class BootAction {
        Normal,
        SafeRollback,
        FactoryReset
    };

    void begin(BootAction action = BootAction::Normal);
    const Config::StoredConfig &config() const { return config_; }
    Config::StoredConfig &config() { return config_; }

    void requestSave();
    void poll(uint32_t now_ms);
    bool saveConfig(bool force = false);
    bool commitLastGood();
    bool restoreLastGood();
    void clearAll();

    bool isMounted() const { return mounted_; }
    bool isConfigLoaded() const { return config_loaded_; }

    // File paths
    static constexpr const char *kConfigPath = "/config.json";
    static constexpr const char *kLastGoodPath = "/config_last_good.json";

private:
    bool loadConfig();
    bool saveConfigInternal();
    void markDirty();

    Config::StoredConfig config_;
    bool dirty_ = false;
    uint32_t last_save_ms_ = 0;
    uint32_t debounce_ms_ = Config::STORAGE_SAVE_DEBOUNCE_MS;
    bool mounted_ = false;
    bool config_loaded_ = false;
};
