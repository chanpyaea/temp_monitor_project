#include "OtaManager.h"
#include "core/Logger.h"
#include "config/AppConfig.h"
#include <WiFiClientSecure.h>
#include <HTTPUpdate.h>
#include <ArduinoJson.h>

OtaManager* OtaManager::instance_ = nullptr;

void OtaManager::begin(StorageManager &storage, NetworkManager &network) {
    storage_ = &storage;
    network_ = &network;
    instance_ = this;

    latest_version_[0] = '\0';
    download_url_[0] = '\0';
    error_msg_[0] = '\0';

    LOGI("OTA", "OTA Manager initialized");
    LOGI("OTA", "Current version: %s", Config::APP_VERSION);
    LOGI("OTA", "GitHub repo: %s", storage_->config().ota_github_repo);
}

void OtaManager::poll() {
    if (!storage_ || !network_) return;

    const auto &cfg = storage_->config();

    // Auto-check on boot (once)
    if (cfg.ota_enabled && cfg.ota_auto_check && !auto_checked_) {
        if (network_->isConnected()) {
            uint32_t now = millis();
            // Wait 30 seconds after boot before checking
            if (now > 30000) {
                LOGI("OTA", "Auto-checking for updates on boot");
                checkForUpdate();
                auto_checked_ = true;
            }
        }
    }
}

bool OtaManager::checkForUpdate() {
    if (!storage_ || !network_) {
        snprintf(error_msg_, sizeof(error_msg_), "Manager not initialized");
        setState(UpdateState::ERROR);
        return false;
    }

    if (!storage_->config().ota_enabled) {
        snprintf(error_msg_, sizeof(error_msg_), "OTA updates disabled");
        setState(UpdateState::ERROR);
        return false;
    }

    if (!network_->isConnected()) {
        snprintf(error_msg_, sizeof(error_msg_), "WiFi not connected");
        setState(UpdateState::ERROR);
        return false;
    }

    setState(UpdateState::CHECKING);
    LOGI("OTA", "Checking for updates from GitHub");

    bool result = fetchLatestRelease();
    last_check_ms_ = millis();

    return result;
}

bool OtaManager::fetchLatestRelease() {
    WiFiClientSecure client;
    client.setInsecure(); // Skip certificate verification (for simplicity)

    const char* host = "api.github.com";
    const int port = 443;

    LOGI("OTA", "Connecting to %s:%d", host, port);

    if (!client.connect(host, port)) {
        snprintf(error_msg_, sizeof(error_msg_), "Failed to connect to GitHub API");
        LOGE("OTA", "%s", error_msg_);
        setState(UpdateState::ERROR);
        return false;
    }

    // Build API URL
    char url[256];
    snprintf(url, sizeof(url), "/repos/%s/releases/latest", storage_->config().ota_github_repo);

    LOGI("OTA", "GET %s", url);

    // Send HTTP request
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "User-Agent: ESP32-TempMonitor\r\n" +
                 "Accept: application/json\r\n" +
                 "Connection: close\r\n\r\n");

    // Wait for response
    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 10000) {
            snprintf(error_msg_, sizeof(error_msg_), "Request timeout");
            LOGE("OTA", "%s", error_msg_);
            client.stop();
            setState(UpdateState::ERROR);
            return false;
        }
    }

    // Skip headers
    bool headers_done = false;
    while (client.available()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") {
            headers_done = true;
            break;
        }
    }

    if (!headers_done) {
        snprintf(error_msg_, sizeof(error_msg_), "Invalid HTTP response");
        LOGE("OTA", "%s", error_msg_);
        client.stop();
        setState(UpdateState::ERROR);
        return false;
    }

    // Read JSON body
    String json_body;
    while (client.available()) {
        json_body += client.readString();
    }
    client.stop();

    // Parse JSON
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, json_body);

    if (error) {
        snprintf(error_msg_, sizeof(error_msg_), "JSON parse error: %s", error.c_str());
        LOGE("OTA", "%s", error_msg_);
        setState(UpdateState::ERROR);
        return false;
    }

    // Extract version and download URL
    const char* tag_name = doc["tag_name"];
    if (!tag_name) {
        snprintf(error_msg_, sizeof(error_msg_), "No tag_name in response");
        LOGE("OTA", "%s", error_msg_);
        setState(UpdateState::ERROR);
        return false;
    }

    strncpy(latest_version_, tag_name, sizeof(latest_version_) - 1);
    latest_version_[sizeof(latest_version_) - 1] = '\0';

    LOGI("OTA", "Latest version: %s", latest_version_);

    // Find .bin asset
    JsonArray assets = doc["assets"];
    bool found_binary = false;

    for (JsonObject asset : assets) {
        const char* asset_name = asset["name"];
        const char* browser_download_url = asset["browser_download_url"];

        if (asset_name && browser_download_url) {
            // Look for .bin file
            if (strstr(asset_name, ".bin") != nullptr) {
                strncpy(download_url_, browser_download_url, sizeof(download_url_) - 1);
                download_url_[sizeof(download_url_) - 1] = '\0';
                found_binary = true;
                LOGI("OTA", "Found binary: %s", asset_name);
                LOGI("OTA", "Download URL: %s", download_url_);
                break;
            }
        }
    }

    if (!found_binary) {
        snprintf(error_msg_, sizeof(error_msg_), "No .bin file in release");
        LOGE("OTA", "%s", error_msg_);
        setState(UpdateState::ERROR);
        return false;
    }

    // Compare versions
    if (compareVersions(Config::APP_VERSION, latest_version_)) {
        LOGI("OTA", "Update available: %s -> %s", Config::APP_VERSION, latest_version_);
        setState(UpdateState::UPDATE_AVAILABLE);
        return true;
    } else {
        LOGI("OTA", "Already up to date");
        snprintf(error_msg_, sizeof(error_msg_), "Already on latest version");
        setState(UpdateState::IDLE);
        return false;
    }
}

bool OtaManager::compareVersions(const char* current, const char* latest) {
    // Simple version comparison (assumes format: v1.2.3 or 1.2.3)
    // Returns true if latest > current

    int curr_major = 0, curr_minor = 0, curr_patch = 0;
    int lat_major = 0, lat_minor = 0, lat_patch = 0;

    // Parse current version
    const char* curr_ptr = current;
    if (curr_ptr[0] == 'v' || curr_ptr[0] == 'V') curr_ptr++;
    sscanf(curr_ptr, "%d.%d.%d", &curr_major, &curr_minor, &curr_patch);

    // Parse latest version
    const char* lat_ptr = latest;
    if (lat_ptr[0] == 'v' || lat_ptr[0] == 'V') lat_ptr++;
    sscanf(lat_ptr, "%d.%d.%d", &lat_major, &lat_minor, &lat_patch);

    LOGD("OTA", "Version comparison: %d.%d.%d vs %d.%d.%d",
         curr_major, curr_minor, curr_patch,
         lat_major, lat_minor, lat_patch);

    // Compare
    if (lat_major > curr_major) return true;
    if (lat_major < curr_major) return false;
    if (lat_minor > curr_minor) return true;
    if (lat_minor < curr_minor) return false;
    if (lat_patch > curr_patch) return true;

    return false;
}

bool OtaManager::startUpdate() {
    if (state_ != UpdateState::UPDATE_AVAILABLE) {
        snprintf(error_msg_, sizeof(error_msg_), "No update available");
        LOGE("OTA", "%s", error_msg_);
        return false;
    }

    if (!network_->isConnected()) {
        snprintf(error_msg_, sizeof(error_msg_), "WiFi not connected");
        LOGE("OTA", "%s", error_msg_);
        setState(UpdateState::ERROR);
        return false;
    }

    return downloadAndInstall(download_url_);
}

bool OtaManager::downloadAndInstall(const char* url) {
    setState(UpdateState::DOWNLOADING);
    LOGI("OTA", "Starting OTA update from: %s", url);

    // Configure HTTPUpdate
    httpUpdate.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    httpUpdate.onProgress(OtaManager::onProgress);

    // Start update
    WiFiClient client;
    t_httpUpdate_return ret = httpUpdate.update(client, url);

    switch (ret) {
        case HTTP_UPDATE_FAILED:
            snprintf(error_msg_, sizeof(error_msg_), "Update failed: %s",
                     httpUpdate.getLastErrorString().c_str());
            LOGE("OTA", "%s", error_msg_);
            setState(UpdateState::ERROR);
            return false;

        case HTTP_UPDATE_NO_UPDATES:
            snprintf(error_msg_, sizeof(error_msg_), "No update needed");
            LOGI("OTA", "%s", error_msg_);
            setState(UpdateState::IDLE);
            return false;

        case HTTP_UPDATE_OK:
            LOGI("OTA", "Update successful! Rebooting...");
            setState(UpdateState::SUCCESS);
            delay(1000);
            ESP.restart();
            return true;
    }

    return false;
}

void OtaManager::cancelUpdate() {
    if (state_ == UpdateState::CHECKING || state_ == UpdateState::DOWNLOADING) {
        LOGI("OTA", "Update cancelled");
        setState(UpdateState::IDLE);
    }
}

void OtaManager::setState(UpdateState new_state) {
    if (state_ != new_state) {
        state_ = new_state;
        LOGD("OTA", "State changed to: %d", (int)new_state);
    }
}

void OtaManager::onProgress(size_t current, size_t total) {
    if (instance_ && total > 0) {
        instance_->progress_ = (current * 100) / total;
        if (instance_->progress_ % 10 == 0) {
            LOGI("OTA", "Download progress: %d%%", instance_->progress_);
        }
    }
}
