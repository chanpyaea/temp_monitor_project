#include "MqttManager.h"
#include "core/Logger.h"
#include "config/AppConfig.h"
#include <ArduinoJson.h>

void MqttManager::begin(StorageManager &storage, NetworkManager &network) {
    storage_ = &storage;
    network_ = &network;

    mqtt_client_ = new PubSubClient(wifi_client_);

    // Increase buffer size for discovery messages (default is 256 bytes)
    mqtt_client_->setBufferSize(1024);

    const auto &cfg = storage_->config();
    if (strlen(cfg.mqtt_host) > 0) {
        mqtt_client_->setServer(cfg.mqtt_host, cfg.mqtt_port);
        LOGI("MQTT", "Server configured: %s:%d", cfg.mqtt_host, cfg.mqtt_port);
    }
}

void MqttManager::poll() {
    if (!mqtt_client_ || !storage_->config().mqtt_enabled) {
        return;
    }

    // Need WiFi connection
    if (!network_->isConnected()) {
        return;
    }

    // Maintain connection
    if (!mqtt_client_->connected()) {
        uint32_t now = millis();
        if (now - last_connect_attempt_ms_ > 5000) {
            reconnect();
            last_connect_attempt_ms_ = now;
        }
    } else {
        mqtt_client_->loop();
    }
}

bool MqttManager::connect() {
    if (!mqtt_client_ || !network_->isConnected()) {
        return false;
    }

    const auto &cfg = storage_->config();

    LOGI("MQTT", "Connecting to broker...");

    String client_id = String(Config::APP_NAME) + "-" + String(ESP.getEfuseMac(), HEX);

    bool connected;
    if (strlen(cfg.mqtt_user) > 0) {
        connected = mqtt_client_->connect(client_id.c_str(),
                                         cfg.mqtt_user,
                                         cfg.mqtt_password);
    } else {
        connected = mqtt_client_->connect(client_id.c_str());
    }

    if (connected) {
        LOGI("MQTT", "Connected to broker");
        reconnect_attempts_ = 0;
        discovery_sent_ = false;

        // Send discovery immediately after connection
        if (cfg.mqtt_discovery) {
            LOGI("MQTT", "Sending Home Assistant discovery messages");
            sendDiscovery();
            discovery_sent_ = true;
        }

        return true;
    } else {
        LOGW("MQTT", "Connection failed, rc=%d", mqtt_client_->state());
        return false;
    }
}

bool MqttManager::reconnect() {
    reconnect_attempts_++;

    if (reconnect_attempts_ > 5) {
        // Give up after 5 attempts, wait longer
        if (millis() - last_connect_attempt_ms_ < 60000) {
            return false;
        }
        reconnect_attempts_ = 0;
    }

    return connect();
}

void MqttManager::disconnect() {
    if (mqtt_client_ && mqtt_client_->connected()) {
        mqtt_client_->disconnect();
        LOGI("MQTT", "Disconnected");
    }
}

bool MqttManager::isConnected() const {
    return mqtt_client_ && mqtt_client_->connected();
}

bool MqttManager::publish(const SensorData &data, const SystemStatus &status) {
    if (!isConnected()) {
        return false;
    }

    // Rate limiting
    uint32_t now = millis();
    if (now - last_publish_ms_ < Config::MQTT_PUBLISH_INTERVAL_MS) {
        return false;
    }
    last_publish_ms_ = now;

    // Build and publish payload
    char payload[512];
    buildPayload(data, status, payload, sizeof(payload));

    const auto &cfg = storage_->config();
    char topic[128];
    snprintf(topic, sizeof(topic), "%s/state", cfg.mqtt_base_topic);

    bool success = mqtt_client_->publish(topic, payload);

    if (success) {
        LOGI("MQTT", "Published to %s", topic);
    } else {
        LOGW("MQTT", "Publish failed");
    }

    return success;
}

void MqttManager::buildPayload(const SensorData &data, const SystemStatus &status,
                               char *buffer, size_t size) {
    JsonDocument doc;
    if (data.has_temperature) {
        doc["temperature"] = serialized(String(data.temperature_c, 1));
    }
    if (data.has_humidity) {
        doc["humidity"] = serialized(String(data.humidity, 1));
    }

    // System status
    doc["uptime"] = status.uptime_seconds;
    doc["wifi_rssi"] = status.wifi_rssi;
    doc["free_heap"] = status.free_heap;

    serializeJson(doc, buffer, size);
}

void MqttManager::sendDiscovery() {
    LOGI("MQTT", "Sending Home Assistant discovery messages");

    // Temperature sensor
    publishDiscoveryConfig("sensor", "temperature", "temperature", "°C",
                          "{{ value_json.temperature }}");

    // Humidity sensor
    publishDiscoveryConfig("sensor", "humidity", "humidity", "%",
                          "{{ value_json.humidity }}");

    // WiFi RSSI sensor
    publishDiscoveryConfig("sensor", "wifi_rssi", "signal_strength", "dBm",
                          "{{ value_json.wifi_rssi }}");

    // Uptime sensor
    publishDiscoveryConfig("sensor", "uptime", "duration", "s",
                          "{{ value_json.uptime }}");
}

void MqttManager::publishDiscoveryConfig(const char *component, const char *name,
                                        const char *device_class, const char *unit,
                                        const char *value_template) {
    const auto &cfg = storage_->config();

    // Build discovery topic
    char topic[256];
    snprintf(topic, sizeof(topic), "homeassistant/%s/%s_%s/config",
             component, cfg.mqtt_device_name, name);

    // Build config payload
    JsonDocument doc;

    char unique_id[64];
    snprintf(unique_id, sizeof(unique_id), "%s_%s", cfg.mqtt_device_name, name);
    doc["unique_id"] = unique_id;

    char friendly_name[64];
    snprintf(friendly_name, sizeof(friendly_name), "%s %s", cfg.mqtt_device_name, name);
    doc["name"] = friendly_name;

    char state_topic[128];
    snprintf(state_topic, sizeof(state_topic), "%s/state", cfg.mqtt_base_topic);
    doc["state_topic"] = state_topic;

    doc["value_template"] = value_template;

    if (device_class) {
        doc["device_class"] = device_class;
    }
    if (unit) {
        doc["unit_of_measurement"] = unit;
    }

    // Device info
    JsonObject device = doc["device"].to<JsonObject>();
    device["identifiers"][0] = cfg.mqtt_device_name;
    device["name"] = cfg.mqtt_device_name;
    device["model"] = Config::APP_NAME;
    device["manufacturer"] = "DIY";
    device["sw_version"] = Config::APP_VERSION;

    char payload[1024];
    serializeJson(doc, payload, sizeof(payload));

    LOGI("MQTT", "Publishing discovery to: %s", topic);
    LOGD("MQTT", "Payload: %s", payload);

    bool success = mqtt_client_->publish(topic, payload, true); // Retained

    if (!success) {
        LOGE("MQTT", "Failed to publish discovery for: %s", name);
    }

    LOGI("MQTT", "Discovery sent: %s", name);
}
