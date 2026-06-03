#pragma once

#include <Arduino.h>
#include <WebServer.h>
#include <DNSServer.h>
#include "modules/StorageManager.h"
#include "modules/NetworkManager.h"
#include "modules/OtaManager.h"
#include "modules/DataLogger.h"

class WebConfig {
public:
    void begin(StorageManager &storage, NetworkManager &network);
    void poll();
    void startAPMode();
    void stopAPMode();
    bool isAPMode() const { return ap_mode_; }

    void setOtaManager(OtaManager *ota) { ota_manager_ = ota; }
    void setDataLogger(DataLogger *logger) { data_logger_ = logger; }

private:
    void setupRoutes();
    void handleRoot();
    void handleConfig();
    void handleSave();
    void handleStatus();
    void handleRestart();
    void handleNotFound();
    void handleOTA();
    void handleOTACheck();
    void handleOTAInstall();
    void handleHistory();
    void handleExport();

    String getHTMLHeader();
    String getHTMLFooter();
    String getConfigPage();
    String getStatusPage();
    String getOTAPage();

    StorageManager *storage_ = nullptr;
    NetworkManager *network_ = nullptr;
    OtaManager *ota_manager_ = nullptr;
    DataLogger *data_logger_ = nullptr;
    WebServer *server_ = nullptr;
    DNSServer *dns_server_ = nullptr;
    bool ap_mode_ = false;
};
