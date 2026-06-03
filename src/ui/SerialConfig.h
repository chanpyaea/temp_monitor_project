#pragma once

#include <Arduino.h>
#include "modules/StorageManager.h"
#include "modules/OtaManager.h"
#include "modules/DataLogger.h"

class SerialConfigMenu {
public:
    void begin(StorageManager &storage);
    void poll();
    bool isActive() const { return active_; }

    void setOtaManager(OtaManager *ota) { ota_manager_ = ota; }
    void setDataLogger(DataLogger *logger) { data_logger_ = logger; }

private:
    void showMainMenu();
    void showWiFiMenu();
    void showMQTTMenu();
    void showSensorMenu();
    void showStatusMenu();
    void showOTAMenu();
    void handleInput();
    void readLine(char *buffer, size_t max_len);
    void clearInputBuffer();

    StorageManager *storage_ = nullptr;
    OtaManager *ota_manager_ = nullptr;
    DataLogger *data_logger_ = nullptr;
    bool active_ = false;
    uint32_t last_input_ms_ = 0;
    static constexpr uint32_t INPUT_TIMEOUT_MS = 30000; // 30 second timeout
};
