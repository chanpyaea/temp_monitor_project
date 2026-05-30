#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include "modules/StorageManager.h"

class NetworkManager {
public:
    enum WifiState {
        WIFI_OFF,
        WIFI_CONNECTING,
        WIFI_CONNECTED,
        WIFI_AP_MODE,
        WIFI_ERROR
    };

    void begin(StorageManager &storage);
    void poll();

    bool connect();
    void disconnect();
    void startAP();
    void stopAP();

    bool isConnected() const { return wifi_state_ == WIFI_CONNECTED; }
    WifiState state() const { return wifi_state_; }
    int8_t rssi() const;
    const char* localIP() const;
    const char* hostname() const { return hostname_; }

private:
    void handleConnecting();
    void handleConnected();
    void scheduleRetry();

    StorageManager *storage_ = nullptr;
    WifiState wifi_state_ = WIFI_OFF;
    uint32_t connect_start_ms_ = 0;
    uint32_t retry_at_ms_ = 0;
    uint8_t retry_count_ = 0;
    char hostname_[32];
    char local_ip_[16];
};
