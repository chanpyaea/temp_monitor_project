#include "NetworkManager.h"
#include "core/Logger.h"
#include "config/AppConfig.h"

void NetworkManager::begin(StorageManager &storage) {
    storage_ = &storage;

    // Generate hostname
    uint64_t chipid = ESP.getEfuseMac();
    snprintf(hostname_, sizeof(hostname_), "%s-%04X",
             Config::DEFAULT_HOSTNAME, (uint16_t)(chipid & 0xFFFF));

    WiFi.setHostname(hostname_);
    WiFi.mode(WIFI_STA);

    LOGI("Network", "Hostname: %s", hostname_);

    // Auto-connect if enabled
    if (storage_->config().wifi_enabled && strlen(storage_->config().wifi_ssid) > 0) {
        connect();
    }
}

void NetworkManager::poll() {
    uint32_t now = millis();

    switch (wifi_state_) {
        case WIFI_CONNECTING:
            handleConnecting();
            break;

        case WIFI_CONNECTED:
            handleConnected();
            break;

        case WIFI_ERROR:
            if (now >= retry_at_ms_) {
                LOGI("Network", "Retrying connection...");
                connect();
            }
            break;

        default:
            break;
    }
}

bool NetworkManager::connect() {
    if (!storage_) return false;

    const auto &cfg = storage_->config();

    if (strlen(cfg.wifi_ssid) == 0) {
        LOGW("Network", "No WiFi credentials configured");
        return false;
    }

    LOGI("Network", "Connecting to WiFi: %s", cfg.wifi_ssid);

    WiFi.begin(cfg.wifi_ssid, cfg.wifi_password);
    wifi_state_ = WIFI_CONNECTING;
    connect_start_ms_ = millis();
    retry_count_ = 0;

    return true;
}

void NetworkManager::handleConnecting() {
    wl_status_t status = WiFi.status();

    if (status == WL_CONNECTED) {
        wifi_state_ = WIFI_CONNECTED;
        retry_count_ = 0;

        strlcpy(local_ip_, WiFi.localIP().toString().c_str(), sizeof(local_ip_));

        LOGI("Network", "WiFi connected!");
        LOGI("Network", "IP: %s", local_ip_);
        LOGI("Network", "RSSI: %d dBm", WiFi.RSSI());
        return;
    }

    // Check timeout
    if (millis() - connect_start_ms_ > Config::WIFI_CONNECT_TIMEOUT_MS) {
        LOGW("Network", "WiFi connection timeout");
        scheduleRetry();
    }
}

void NetworkManager::handleConnected() {
    if (WiFi.status() != WL_CONNECTED) {
        LOGW("Network", "WiFi disconnected");
        scheduleRetry();
    }
}

void NetworkManager::scheduleRetry() {
    wifi_state_ = WIFI_ERROR;
    retry_count_++;

    // Exponential backoff: 2s, 4s, 8s, 16s, max 60s
    uint32_t delay_ms = 1000 * (1 << retry_count_);
    delay_ms = min(delay_ms, (uint32_t)60000UL);

    retry_at_ms_ = millis() + delay_ms;

    LOGW("Network", "Retry #%d in %u ms", retry_count_, delay_ms);
}

void NetworkManager::disconnect() {
    WiFi.disconnect(true);
    wifi_state_ = WIFI_OFF;
    LOGI("Network", "WiFi disconnected");
}

void NetworkManager::startAP() {
    LOGI("Network", "Starting AP mode: %s", Config::DEFAULT_AP_SSID);

    WiFi.mode(WIFI_AP);
    WiFi.softAP(Config::DEFAULT_AP_SSID, Config::DEFAULT_AP_PASS);

    wifi_state_ = WIFI_AP_MODE;

    LOGI("Network", "AP started, IP: %s", WiFi.softAPIP().toString().c_str());
}

void NetworkManager::stopAP() {
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_STA);
    wifi_state_ = WIFI_OFF;
    LOGI("Network", "AP stopped");
}

int8_t NetworkManager::rssi() const {
    if (wifi_state_ == WIFI_CONNECTED) {
        return WiFi.RSSI();
    }
    return 0;
}

const char* NetworkManager::localIP() const {
    return local_ip_;
}
