#pragma once

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include "config/AppData.h"
#include "modules/StorageManager.h"
#include "modules/NetworkManager.h"

class MqttManager {
public:
    void begin(StorageManager &storage, NetworkManager &network);
    void poll();

    bool connect();
    void disconnect();
    bool publish(const SensorData &data, const SystemStatus &status);
    bool isConnected() const;

    void sendDiscovery();

private:
    bool reconnect();
    void buildPayload(const SensorData &data, const SystemStatus &status, char *buffer, size_t size);
    void publishDiscoveryConfig(const char *component, const char *name,
                               const char *device_class, const char *unit,
                               const char *value_template);

    StorageManager *storage_ = nullptr;
    NetworkManager *network_ = nullptr;
    WiFiClient wifi_client_;
    PubSubClient *mqtt_client_ = nullptr;

    uint32_t last_publish_ms_ = 0;
    uint32_t last_connect_attempt_ms_ = 0;
    uint8_t reconnect_attempts_ = 0;
    bool discovery_sent_ = false;
};
