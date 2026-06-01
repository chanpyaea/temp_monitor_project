#pragma once

#include <Arduino.h>
#include <WebServer.h>
#include <DNSServer.h>
#include "modules/StorageManager.h"
#include "modules/NetworkManager.h"

class WebConfig {
public:
    void begin(StorageManager &storage, NetworkManager &network);
    void poll();
    void startAPMode();
    void stopAPMode();
    bool isAPMode() const { return ap_mode_; }

private:
    void setupRoutes();
    void handleRoot();
    void handleConfig();
    void handleSave();
    void handleStatus();
    void handleRestart();
    void handleNotFound();

    String getHTMLHeader();
    String getHTMLFooter();
    String getConfigPage();
    String getStatusPage();

    StorageManager *storage_ = nullptr;
    NetworkManager *network_ = nullptr;
    WebServer *server_ = nullptr;
    DNSServer *dns_server_ = nullptr;
    bool ap_mode_ = false;

    static constexpr uint16_t WEB_SERVER_PORT = 80;
    static constexpr uint16_t DNS_PORT = 53;
};

// Define static constants
constexpr uint16_t WebConfig::WEB_SERVER_PORT;
constexpr uint16_t WebConfig::DNS_PORT;
