#pragma once

#include <Arduino.h>
#include "modules/StorageManager.h"

class SerialConfig {
public:
    void begin(StorageManager &storage);
    void poll();
    bool isActive() const { return active_; }

private:
    void showMainMenu();
    void showWiFiMenu();
    void showMQTTMenu();
    void showSensorMenu();
    void showStatusMenu();
    void handleInput();
    void readLine(char *buffer, size_t max_len);
    void clearInputBuffer();

    StorageManager *storage_ = nullptr;
    bool active_ = false;
    uint32_t last_input_ms_ = 0;
    static constexpr uint32_t INPUT_TIMEOUT_MS = 30000; // 30 second timeout
};
