#include "WebConfig.h"
#include "core/Logger.h"
#include "config/AppConfig.h"
#include <WiFi.h>

// Define constants
static const char* AP_SSID = "TempMonitor-Setup";
static const char* AP_PASS = "12345678";
static const uint16_t WEB_SERVER_PORT = 80;
static const uint16_t DNS_PORT = 53;

void WebConfig::begin(StorageManager &storage, NetworkManager &network) {
    storage_ = &storage;
    network_ = &network;

    server_ = new WebServer(WEB_SERVER_PORT);
    dns_server_ = new DNSServer();

    setupRoutes();

    LOGI("WebConfig", "Web configuration initialized");
}

void WebConfig::poll() {
    if (server_) {
        server_->handleClient();
    }
    if (dns_server_ && ap_mode_) {
        dns_server_->processNextRequest();
    }
}

void WebConfig::startAPMode() {
    if (ap_mode_) return;

    LOGI("WebConfig", "Starting AP mode: %s", AP_SSID);

    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASS);

    IPAddress IP = WiFi.softAPIP();
    LOGI("WebConfig", "AP IP address: %s", IP.toString().c_str());

    // Start DNS server for captive portal
    dns_server_->start(DNS_PORT, "*", IP);

    server_->begin();
    ap_mode_ = true;

    LOGI("WebConfig", "Web server started on http://%s", IP.toString().c_str());
    LOGI("WebConfig", "Connect to WiFi: %s (password: %s)", AP_SSID, AP_PASS);
}

void WebConfig::stopAPMode() {
    if (!ap_mode_) return;

    LOGI("WebConfig", "Stopping AP mode");

    if (dns_server_) {
        dns_server_->stop();
    }
    if (server_) {
        server_->stop();
    }

    WiFi.softAPdisconnect(true);
    ap_mode_ = false;
}

void WebConfig::setupRoutes() {
    server_->on("/", [this]() { handleRoot(); });
    server_->on("/config", [this]() { handleConfig(); });
    server_->on("/save", HTTP_POST, [this]() { handleSave(); });
    server_->on("/status", [this]() { handleStatus(); });
    server_->on("/restart", [this]() { handleRestart(); });
    server_->onNotFound([this]() { handleNotFound(); });
}

void WebConfig::handleRoot() {
    String html = getHTMLHeader();
    html += "<h1>Temperature Monitor</h1>";
    html += "<p>Welcome to the Temperature Monitor configuration portal.</p>";
    html += "<div class='menu'>";
    html += "<a href='/config' class='btn'>Configuration</a>";
    html += "<a href='/status' class='btn'>Status</a>";
    html += "<a href='/restart' class='btn btn-danger'>Restart</a>";
    html += "</div>";
    html += getHTMLFooter();

    server_->send(200, "text/html", html);
}

void WebConfig::handleConfig() {
    if (server_->method() == HTTP_GET) {
        server_->send(200, "text/html", getConfigPage());
    }
}

void WebConfig::handleSave() {
    if (server_->method() != HTTP_POST) {
        server_->send(405, "text/plain", "Method Not Allowed");
        return;
    }

    // WiFi settings
    if (server_->hasArg("wifi_ssid")) {
        strncpy(storage_->config().wifi_ssid, server_->arg("wifi_ssid").c_str(),
                sizeof(storage_->config().wifi_ssid));
    }
    if (server_->hasArg("wifi_password")) {
        strncpy(storage_->config().wifi_password, server_->arg("wifi_password").c_str(),
                sizeof(storage_->config().wifi_password));
    }
    storage_->config().wifi_enabled = server_->hasArg("wifi_enabled");

    // MQTT settings
    if (server_->hasArg("mqtt_host")) {
        strncpy(storage_->config().mqtt_host, server_->arg("mqtt_host").c_str(),
                sizeof(storage_->config().mqtt_host));
    }
    if (server_->hasArg("mqtt_port")) {
        storage_->config().mqtt_port = server_->arg("mqtt_port").toInt();
    }
    if (server_->hasArg("mqtt_user")) {
        strncpy(storage_->config().mqtt_user, server_->arg("mqtt_user").c_str(),
                sizeof(storage_->config().mqtt_user));
    }
    if (server_->hasArg("mqtt_password")) {
        strncpy(storage_->config().mqtt_password, server_->arg("mqtt_password").c_str(),
                sizeof(storage_->config().mqtt_password));
    }
    if (server_->hasArg("mqtt_base_topic")) {
        strncpy(storage_->config().mqtt_base_topic, server_->arg("mqtt_base_topic").c_str(),
                sizeof(storage_->config().mqtt_base_topic));
    }
    if (server_->hasArg("mqtt_device_name")) {
        strncpy(storage_->config().mqtt_device_name, server_->arg("mqtt_device_name").c_str(),
                sizeof(storage_->config().mqtt_device_name));
    }
    storage_->config().mqtt_enabled = server_->hasArg("mqtt_enabled");
    storage_->config().mqtt_discovery = server_->hasArg("mqtt_discovery");

    // Sensor settings
    if (server_->hasArg("temp_offset")) {
        storage_->config().temp_offset = server_->arg("temp_offset").toFloat();
    }
    if (server_->hasArg("hum_offset")) {
        storage_->config().hum_offset = server_->arg("hum_offset").toFloat();
    }

    // Save configuration
    storage_->saveConfig(true);

    LOGI("WebConfig", "Configuration saved via web interface");

    String html = getHTMLHeader();
    html += "<h1>Configuration Saved</h1>";
    html += "<p>Your settings have been saved successfully.</p>";
    html += "<p>The device will restart in 3 seconds...</p>";
    html += "<script>setTimeout(function(){ window.location.href='/'; }, 3000);</script>";
    html += getHTMLFooter();

    server_->send(200, "text/html", html);

    // Restart after 3 seconds
    delay(3000);
    ESP.restart();
}

void WebConfig::handleStatus() {
    server_->send(200, "text/html", getStatusPage());
}

void WebConfig::handleRestart() {
    String html = getHTMLHeader();
    html += "<h1>Restarting...</h1>";
    html += "<p>The device is restarting. Please wait...</p>";
    html += getHTMLFooter();

    server_->send(200, "text/html", html);

    delay(1000);
    ESP.restart();
}

void WebConfig::handleNotFound() {
    // Redirect to root for captive portal
    if (ap_mode_) {
        server_->sendHeader("Location", "/", true);
        server_->send(302, "text/plain", "");
    } else {
        server_->send(404, "text/plain", "Not Found");
    }
}

String WebConfig::getHTMLHeader() {
    String html = "<!DOCTYPE html><html><head>";
    html += "<meta charset='UTF-8'>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    html += "<title>Temperature Monitor</title>";
    html += "<style>";
    html += "body { font-family: Arial, sans-serif; margin: 0; padding: 20px; background: #f0f0f0; }";
    html += ".container { max-width: 600px; margin: 0 auto; background: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }";
    html += "h1 { color: #333; margin-top: 0; }";
    html += "h2 { color: #666; border-bottom: 2px solid #007bff; padding-bottom: 10px; }";
    html += ".form-group { margin-bottom: 15px; }";
    html += "label { display: block; margin-bottom: 5px; font-weight: bold; color: #555; }";
    html += "input[type='text'], input[type='password'], input[type='number'] { width: 100%; padding: 8px; border: 1px solid #ddd; border-radius: 4px; box-sizing: border-box; }";
    html += "input[type='checkbox'] { margin-right: 5px; }";
    html += ".checkbox-label { display: inline; font-weight: normal; }";
    html += ".btn { display: inline-block; padding: 10px 20px; margin: 5px; background: #007bff; color: white; text-decoration: none; border-radius: 4px; border: none; cursor: pointer; }";
    html += ".btn:hover { background: #0056b3; }";
    html += ".btn-success { background: #28a745; }";
    html += ".btn-success:hover { background: #218838; }";
    html += ".btn-danger { background: #dc3545; }";
    html += ".btn-danger:hover { background: #c82333; }";
    html += ".menu { margin: 20px 0; }";
    html += ".status-item { padding: 10px; margin: 5px 0; background: #f8f9fa; border-left: 4px solid #007bff; }";
    html += ".status-label { font-weight: bold; color: #555; }";
    html += ".status-value { color: #333; }";
    html += "</style>";
    html += "</head><body><div class='container'>";
    return html;
}

String WebConfig::getHTMLFooter() {
    String html = "<hr><p style='text-align: center; color: #999; font-size: 12px;'>";
    html += "Temperature Monitor v" + String(Config::APP_VERSION);
    html += "</p></div></body></html>";
    return html;
}

String WebConfig::getConfigPage() {
    const auto &cfg = storage_->config();

    String html = getHTMLHeader();
    html += "<h1>Configuration</h1>";
    html += "<form method='POST' action='/save'>";

    // WiFi Settings
    html += "<h2>WiFi Settings</h2>";
    html += "<div class='form-group'>";
    html += "<label>SSID:</label>";
    html += "<input type='text' name='wifi_ssid' value='" + String(cfg.wifi_ssid) + "' required>";
    html += "</div>";
    html += "<div class='form-group'>";
    html += "<label>Password:</label>";
    html += "<input type='password' name='wifi_password' value='" + String(cfg.wifi_password) + "'>";
    html += "</div>";
    html += "<div class='form-group'>";
    html += "<input type='checkbox' name='wifi_enabled' id='wifi_enabled' " + String(cfg.wifi_enabled ? "checked" : "") + ">";
    html += "<label for='wifi_enabled' class='checkbox-label'>Enable WiFi</label>";
    html += "</div>";

    // MQTT Settings
    html += "<h2>MQTT Settings</h2>";
    html += "<div class='form-group'>";
    html += "<label>Broker Host:</label>";
    html += "<input type='text' name='mqtt_host' value='" + String(cfg.mqtt_host) + "' placeholder='192.168.1.100'>";
    html += "</div>";
    html += "<div class='form-group'>";
    html += "<label>Broker Port:</label>";
    html += "<input type='number' name='mqtt_port' value='" + String(cfg.mqtt_port) + "' min='1' max='65535'>";
    html += "</div>";
    html += "<div class='form-group'>";
    html += "<label>Username (optional):</label>";
    html += "<input type='text' name='mqtt_user' value='" + String(cfg.mqtt_user) + "'>";
    html += "</div>";
    html += "<div class='form-group'>";
    html += "<label>Password:</label>";
    html += "<input type='password' name='mqtt_password' value='" + String(cfg.mqtt_password) + "'>";
    html += "</div>";
    html += "<div class='form-group'>";
    html += "<label>Base Topic:</label>";
    html += "<input type='text' name='mqtt_base_topic' value='" + String(cfg.mqtt_base_topic) + "'>";
    html += "</div>";
    html += "<div class='form-group'>";
    html += "<label>Device Name:</label>";
    html += "<input type='text' name='mqtt_device_name' value='" + String(cfg.mqtt_device_name) + "'>";
    html += "</div>";
    html += "<div class='form-group'>";
    html += "<input type='checkbox' name='mqtt_enabled' id='mqtt_enabled' " + String(cfg.mqtt_enabled ? "checked" : "") + ">";
    html += "<label for='mqtt_enabled' class='checkbox-label'>Enable MQTT</label>";
    html += "</div>";
    html += "<div class='form-group'>";
    html += "<input type='checkbox' name='mqtt_discovery' id='mqtt_discovery' " + String(cfg.mqtt_discovery ? "checked" : "") + ">";
    html += "<label for='mqtt_discovery' class='checkbox-label'>Home Assistant Auto-Discovery</label>";
    html += "</div>";

    // Sensor Calibration
    html += "<h2>Sensor Calibration</h2>";
    html += "<div class='form-group'>";
    html += "<label>Temperature Offset (°C):</label>";
    html += "<input type='number' name='temp_offset' value='" + String(cfg.temp_offset, 2) + "' step='0.1'>";
    html += "</div>";
    html += "<div class='form-group'>";
    html += "<label>Humidity Offset (%):</label>";
    html += "<input type='number' name='hum_offset' value='" + String(cfg.hum_offset, 2) + "' step='0.1'>";
    html += "</div>";

    html += "<div class='form-group'>";
    html += "<button type='submit' class='btn btn-success'>Save Configuration</button>";
    html += "<a href='/' class='btn'>Cancel</a>";
    html += "</div>";

    html += "</form>";
    html += getHTMLFooter();

    return html;
}

String WebConfig::getStatusPage() {
    const auto &cfg = storage_->config();

    String html = getHTMLHeader();
    html += "<h1>System Status</h1>";

    html += "<h2>WiFi</h2>";
    html += "<div class='status-item'>";
    html += "<span class='status-label'>Status:</span> ";
    html += "<span class='status-value'>" + String(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected") + "</span>";
    html += "</div>";
    if (WiFi.status() == WL_CONNECTED) {
        html += "<div class='status-item'>";
        html += "<span class='status-label'>SSID:</span> ";
        html += "<span class='status-value'>" + WiFi.SSID() + "</span>";
        html += "</div>";
        html += "<div class='status-item'>";
        html += "<span class='status-label'>IP Address:</span> ";
        html += "<span class='status-value'>" + WiFi.localIP().toString() + "</span>";
        html += "</div>";
        html += "<div class='status-item'>";
        html += "<span class='status-label'>RSSI:</span> ";
        html += "<span class='status-value'>" + String(WiFi.RSSI()) + " dBm</span>";
        html += "</div>";
    }

    html += "<h2>MQTT</h2>";
    html += "<div class='status-item'>";
    html += "<span class='status-label'>Enabled:</span> ";
    html += "<span class='status-value'>" + String(cfg.mqtt_enabled ? "Yes" : "No") + "</span>";
    html += "</div>";
    html += "<div class='status-item'>";
    html += "<span class='status-label'>Broker:</span> ";
    html += "<span class='status-value'>" + String(cfg.mqtt_host) + ":" + String(cfg.mqtt_port) + "</span>";
    html += "</div>";

    html += "<h2>System</h2>";
    html += "<div class='status-item'>";
    html += "<span class='status-label'>Firmware:</span> ";
    html += "<span class='status-value'>" + String(Config::APP_VERSION) + "</span>";
    html += "</div>";
    html += "<div class='status-item'>";
    html += "<span class='status-label'>Free Heap:</span> ";
    html += "<span class='status-value'>" + String(ESP.getFreeHeap()) + " bytes</span>";
    html += "</div>";
    html += "<div class='status-item'>";
    html += "<span class='status-label'>Uptime:</span> ";
    html += "<span class='status-value'>" + String(millis() / 1000) + " seconds</span>";
    html += "</div>";

    html += "<div class='menu'>";
    html += "<a href='/' class='btn'>Back</a>";
    html += "<a href='/status' class='btn'>Refresh</a>";
    html += "</div>";

    html += getHTMLFooter();

    return html;
}
