#pragma once

#include <Arduino.h>
#include "modules/StorageManager.h"
#include "modules/NetworkManager.h"

class OtaManager {
public:
    enum class UpdateState {
        IDLE,
        CHECKING,
        UPDATE_AVAILABLE,
        DOWNLOADING,
        INSTALLING,
        SUCCESS,
        ERROR
    };

    void begin(StorageManager &storage, NetworkManager &network);
    void poll();

    // Check for updates from GitHub
    bool checkForUpdate();

    // Trigger update installation
    bool startUpdate();

    // Cancel ongoing update
    void cancelUpdate();

    // Query state
    UpdateState state() const { return state_; }
    const char* latestVersion() const { return latest_version_; }
    const char* errorMessage() const { return error_msg_; }
    bool isUpdateAvailable() const { return state_ == UpdateState::UPDATE_AVAILABLE; }
    int getProgress() const { return progress_; }

private:
    bool fetchLatestRelease();
    bool downloadAndInstall(const char* url);
    bool compareVersions(const char* current, const char* latest);
    void setState(UpdateState new_state);
    static void onProgress(size_t current, size_t total);

    StorageManager* storage_ = nullptr;
    NetworkManager* network_ = nullptr;

    UpdateState state_ = UpdateState::IDLE;
    char latest_version_[16];
    char download_url_[256];
    char error_msg_[128];
    uint32_t last_check_ms_ = 0;
    bool auto_checked_ = false;
    int progress_ = 0;

    static OtaManager* instance_;
};
