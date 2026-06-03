#include "SerialConfig.h"
#include "core/Logger.h"
#include "config/AppConfig.h"

void SerialConfigMenu::begin(StorageManager &storage) {
    storage_ = &storage;
    LOGI("SerialConfig", "Serial configuration available");
    LOGI("SerialConfig", "Press 'c' to enter configuration menu");
}

void SerialConfigMenu::poll() {
    if (!storage_) return;

    // Check for 'c' key to enter config mode
    if (Serial.available() > 0) {
        char c = Serial.read();
        if (c == 'c' || c == 'C') {
            active_ = true;
            last_input_ms_ = millis();
            clearInputBuffer();
            showMainMenu();
        }
    }

    // Handle active config session
    if (active_) {
        if (millis() - last_input_ms_ > INPUT_TIMEOUT_MS) {
            Serial.println("\n[Timeout] Exiting configuration menu");
            active_ = false;
            return;
        }

        if (Serial.available() > 0) {
            handleInput();
        }
    }
}

void SerialConfigMenu::showMainMenu() {
    Serial.println("\n========================================");
    Serial.println("    TEMPERATURE MONITOR CONFIG");
    Serial.println("========================================");
    Serial.println("1. WiFi Settings");
    Serial.println("2. MQTT Settings");
    Serial.println("3. Sensor Calibration");
    Serial.println("4. View Status");
    Serial.println("5. OTA Update");
    Serial.println("6. Save & Exit");
    Serial.println("7. Exit without saving");
    Serial.println("========================================");
    Serial.print("Select option (1-7): ");
}

void SerialConfigMenu::showWiFiMenu() {
    const auto &cfg = storage_->config();

    Serial.println("\n--- WiFi Settings ---");
    Serial.printf("Current SSID: %s\n", strlen(cfg.wifi_ssid) > 0 ? cfg.wifi_ssid : "(not set)");
    Serial.printf("WiFi Enabled: %s\n", cfg.wifi_enabled ? "Yes" : "No");
    Serial.println("\n1. Set SSID");
    Serial.println("2. Set Password");
    Serial.println("3. Enable WiFi");
    Serial.println("4. Disable WiFi");
    Serial.println("5. Back to main menu");
    Serial.print("Select option (1-5): ");
}

void SerialConfigMenu::showMQTTMenu() {
    const auto &cfg = storage_->config();

    Serial.println("\n--- MQTT Settings ---");
    Serial.printf("Broker: %s:%d\n", strlen(cfg.mqtt_host) > 0 ? cfg.mqtt_host : "(not set)", cfg.mqtt_port);
    Serial.printf("Username: %s\n", strlen(cfg.mqtt_user) > 0 ? cfg.mqtt_user : "(not set)");
    Serial.printf("Base Topic: %s\n", cfg.mqtt_base_topic);
    Serial.printf("Device Name: %s\n", cfg.mqtt_device_name);
    Serial.printf("MQTT Enabled: %s\n", cfg.mqtt_enabled ? "Yes" : "No");
    Serial.printf("Auto-Discovery: %s\n", cfg.mqtt_discovery ? "Yes" : "No");
    Serial.println("\n1. Set Broker Host");
    Serial.println("2. Set Broker Port");
    Serial.println("3. Set Username");
    Serial.println("4. Set Password");
    Serial.println("5. Set Base Topic");
    Serial.println("6. Set Device Name");
    Serial.println("7. Enable MQTT");
    Serial.println("8. Disable MQTT");
    Serial.println("9. Toggle Auto-Discovery");
    Serial.println("0. Back to main menu");
    Serial.print("Select option (0-9): ");
}

void SerialConfigMenu::showSensorMenu() {
    const auto &cfg = storage_->config();

    Serial.println("\n--- Sensor Calibration ---");
    Serial.printf("Temperature Offset: %.2f°C\n", cfg.temp_offset);
    Serial.printf("Humidity Offset: %.2f%%\n", cfg.hum_offset);
    Serial.printf("Temperature Units: %s\n", cfg.temp_units_celsius ? "Celsius" : "Fahrenheit");
    Serial.println("\n1. Set Temperature Offset");
    Serial.println("2. Set Humidity Offset");
    Serial.println("3. Toggle Temperature Units");
    Serial.println("4. Back to main menu");
    Serial.print("Select option (1-4): ");
}

void SerialConfigMenu::showStatusMenu() {
    const auto &cfg = storage_->config();

    Serial.println("\n========================================");
    Serial.println("         CURRENT CONFIGURATION");
    Serial.println("========================================");
    Serial.println("\n[WiFi]");
    Serial.printf("  SSID: %s\n", strlen(cfg.wifi_ssid) > 0 ? cfg.wifi_ssid : "(not set)");
    Serial.printf("  Enabled: %s\n", cfg.wifi_enabled ? "Yes" : "No");

    Serial.println("\n[MQTT]");
    Serial.printf("  Broker: %s:%d\n", strlen(cfg.mqtt_host) > 0 ? cfg.mqtt_host : "(not set)", cfg.mqtt_port);
    Serial.printf("  Username: %s\n", strlen(cfg.mqtt_user) > 0 ? cfg.mqtt_user : "(not set)");
    Serial.printf("  Base Topic: %s\n", cfg.mqtt_base_topic);
    Serial.printf("  Device Name: %s\n", cfg.mqtt_device_name);
    Serial.printf("  Enabled: %s\n", cfg.mqtt_enabled ? "Yes" : "No");
    Serial.printf("  Auto-Discovery: %s\n", cfg.mqtt_discovery ? "Yes" : "No");

    Serial.println("\n[Sensor]");
    Serial.printf("  Temp Offset: %.2f°C\n", cfg.temp_offset);
    Serial.printf("  Humidity Offset: %.2f%%\n", cfg.hum_offset);
    Serial.printf("  Units: %s\n", cfg.temp_units_celsius ? "Celsius" : "Fahrenheit");

    Serial.println("\n[OTA]");
    Serial.printf("  GitHub Repo: %s\n", cfg.ota_github_repo);
    Serial.printf("  OTA Enabled: %s\n", cfg.ota_enabled ? "Yes" : "No");
    Serial.printf("  Auto-check: %s\n", cfg.ota_auto_check ? "Yes" : "No");

    Serial.println("\n[System]");
    Serial.printf("  Version: %s\n", Config::APP_VERSION);
    Serial.printf("  Free Heap: %u bytes\n", ESP.getFreeHeap());
    Serial.printf("  Uptime: %lu seconds\n", millis() / 1000);

    Serial.println("========================================");
    Serial.println("\nPress any key to continue...");
}

void SerialConfigMenu::showOTAMenu() {
    const auto &cfg = storage_->config();

    Serial.println("\n--- OTA Update Settings ---");
    Serial.printf("Current Version: %s\n", Config::APP_VERSION);
    Serial.printf("GitHub Repo: %s\n", cfg.ota_github_repo);
    Serial.printf("OTA Enabled: %s\n", cfg.ota_enabled ? "Yes" : "No");
    Serial.printf("Auto-check on Boot: %s\n", cfg.ota_auto_check ? "Yes" : "No");

    if (ota_manager_) {
        if (ota_manager_->isUpdateAvailable()) {
            Serial.printf("\n*** Update Available: %s ***\n", ota_manager_->latestVersion());
        }
    }

    Serial.println("\n1. Check for Updates");
    Serial.println("2. Install Update (if available)");
    Serial.println("3. Set GitHub Repo");
    Serial.println("4. Toggle OTA Enable");
    Serial.println("5. Toggle Auto-check");
    Serial.println("6. Back to main menu");
    Serial.print("Select option (1-6): ");
}

void SerialConfigMenu::handleInput() {
    static char menu_state = 0; // 0=main, 1=wifi, 2=mqtt, 3=sensor, 4=ota
    static char input_buffer[128];

    last_input_ms_ = millis();

    char c = Serial.read();

    // Handle menu navigation
    if (menu_state == 0) { // Main menu
        Serial.println(c);
        switch (c) {
            case '1':
                menu_state = 1;
                showWiFiMenu();
                break;
            case '2':
                menu_state = 2;
                showMQTTMenu();
                break;
            case '3':
                menu_state = 3;
                showSensorMenu();
                break;
            case '4':
                showStatusMenu();
                clearInputBuffer();
                showMainMenu();
                break;
            case '5':
                menu_state = 4;
                showOTAMenu();
                break;
            case '6':
                Serial.println("\nSaving configuration...");
                storage_->saveConfig(true);
                Serial.println("Configuration saved!");
                Serial.println("Exiting configuration menu");
                active_ = false;
                break;
            case '7':
                Serial.println("\nExiting without saving");
                active_ = false;
                break;
            default:
                Serial.println("\nInvalid option");
                showMainMenu();
                break;
        }
    }
    else if (menu_state == 1) { // WiFi menu
        Serial.println(c);
        switch (c) {
            case '1':
                Serial.print("\nEnter WiFi SSID: ");
                readLine(input_buffer, sizeof(input_buffer));
                strncpy(storage_->config().wifi_ssid, input_buffer, sizeof(storage_->config().wifi_ssid));
                Serial.println("SSID updated");
                showWiFiMenu();
                break;
            case '2':
                Serial.print("\nEnter WiFi Password: ");
                readLine(input_buffer, sizeof(input_buffer));
                strncpy(storage_->config().wifi_password, input_buffer, sizeof(storage_->config().wifi_password));
                Serial.println("Password updated");
                showWiFiMenu();
                break;
            case '3':
                storage_->config().wifi_enabled = true;
                Serial.println("\nWiFi enabled");
                showWiFiMenu();
                break;
            case '4':
                storage_->config().wifi_enabled = false;
                Serial.println("\nWiFi disabled");
                showWiFiMenu();
                break;
            case '5':
                menu_state = 0;
                showMainMenu();
                break;
            default:
                Serial.println("\nInvalid option");
                showWiFiMenu();
                break;
        }
    }
    else if (menu_state == 2) { // MQTT menu
        Serial.println(c);
        switch (c) {
            case '1':
                Serial.print("\nEnter MQTT Broker Host: ");
                readLine(input_buffer, sizeof(input_buffer));
                strncpy(storage_->config().mqtt_host, input_buffer, sizeof(storage_->config().mqtt_host));
                Serial.println("Broker host updated");
                showMQTTMenu();
                break;
            case '2':
                Serial.print("\nEnter MQTT Broker Port: ");
                readLine(input_buffer, sizeof(input_buffer));
                storage_->config().mqtt_port = atoi(input_buffer);
                Serial.println("Broker port updated");
                showMQTTMenu();
                break;
            case '3':
                Serial.print("\nEnter MQTT Username (leave empty for none): ");
                readLine(input_buffer, sizeof(input_buffer));
                strncpy(storage_->config().mqtt_user, input_buffer, sizeof(storage_->config().mqtt_user));
                Serial.println("Username updated");
                showMQTTMenu();
                break;
            case '4':
                Serial.print("\nEnter MQTT Password: ");
                readLine(input_buffer, sizeof(input_buffer));
                strncpy(storage_->config().mqtt_password, input_buffer, sizeof(storage_->config().mqtt_password));
                Serial.println("Password updated");
                showMQTTMenu();
                break;
            case '5':
                Serial.print("\nEnter Base Topic: ");
                readLine(input_buffer, sizeof(input_buffer));
                strncpy(storage_->config().mqtt_base_topic, input_buffer, sizeof(storage_->config().mqtt_base_topic));
                Serial.println("Base topic updated");
                showMQTTMenu();
                break;
            case '6':
                Serial.print("\nEnter Device Name: ");
                readLine(input_buffer, sizeof(input_buffer));
                strncpy(storage_->config().mqtt_device_name, input_buffer, sizeof(storage_->config().mqtt_device_name));
                Serial.println("Device name updated");
                showMQTTMenu();
                break;
            case '7':
                storage_->config().mqtt_enabled = true;
                Serial.println("\nMQTT enabled");
                showMQTTMenu();
                break;
            case '8':
                storage_->config().mqtt_enabled = false;
                Serial.println("\nMQTT disabled");
                showMQTTMenu();
                break;
            case '9':
                storage_->config().mqtt_discovery = !storage_->config().mqtt_discovery;
                Serial.printf("\nAuto-Discovery %s\n", storage_->config().mqtt_discovery ? "enabled" : "disabled");
                showMQTTMenu();
                break;
            case '0':
                menu_state = 0;
                showMainMenu();
                break;
            default:
                Serial.println("\nInvalid option");
                showMQTTMenu();
                break;
        }
    }
    else if (menu_state == 3) { // Sensor menu
        Serial.println(c);
        switch (c) {
            case '1':
                Serial.print("\nEnter Temperature Offset (°C): ");
                readLine(input_buffer, sizeof(input_buffer));
                storage_->config().temp_offset = atof(input_buffer);
                Serial.println("Temperature offset updated");
                showSensorMenu();
                break;
            case '2':
                Serial.print("\nEnter Humidity Offset (%): ");
                readLine(input_buffer, sizeof(input_buffer));
                storage_->config().hum_offset = atof(input_buffer);
                Serial.println("Humidity offset updated");
                showSensorMenu();
                break;
            case '3':
                storage_->config().temp_units_celsius = !storage_->config().temp_units_celsius;
                Serial.printf("\nUnits: %s\n", storage_->config().temp_units_celsius ? "Celsius" : "Fahrenheit");
                showSensorMenu();
                break;
            case '4':
                menu_state = 0;
                showMainMenu();
                break;
            default:
                Serial.println("\nInvalid option");
                showSensorMenu();
                break;
        }
    }
    else if (menu_state == 4) { // OTA menu
        Serial.println(c);
        switch (c) {
            case '1':
                Serial.println("\nChecking for updates...");
                if (ota_manager_ && ota_manager_->checkForUpdate()) {
                    Serial.printf("Update available: %s\n", ota_manager_->latestVersion());
                } else if (ota_manager_) {
                    Serial.printf("No update available (current: %s)\n", Config::APP_VERSION);
                    if (strlen(ota_manager_->errorMessage()) > 0) {
                        Serial.printf("Error: %s\n", ota_manager_->errorMessage());
                    }
                }
                showOTAMenu();
                break;
            case '2':
                if (ota_manager_ && ota_manager_->isUpdateAvailable()) {
                    Serial.printf("\nInstalling update: %s\n", ota_manager_->latestVersion());
                    Serial.println("Please wait, do not power off...");
                    ota_manager_->startUpdate();
                } else {
                    Serial.println("\nNo update available to install");
                    showOTAMenu();
                }
                break;
            case '3':
                Serial.print("\nEnter GitHub Repo (owner/repo): ");
                readLine(input_buffer, sizeof(input_buffer));
                strncpy(storage_->config().ota_github_repo, input_buffer, sizeof(storage_->config().ota_github_repo));
                Serial.println("GitHub repo updated");
                showOTAMenu();
                break;
            case '4':
                storage_->config().ota_enabled = !storage_->config().ota_enabled;
                Serial.printf("\nOTA Updates: %s\n", storage_->config().ota_enabled ? "Enabled" : "Disabled");
                showOTAMenu();
                break;
            case '5':
                storage_->config().ota_auto_check = !storage_->config().ota_auto_check;
                Serial.printf("\nAuto-check on boot: %s\n", storage_->config().ota_auto_check ? "Enabled" : "Disabled");
                showOTAMenu();
                break;
            case '6':
                menu_state = 0;
                showMainMenu();
                break;
            default:
                Serial.println("\nInvalid option");
                showOTAMenu();
                break;
        }
    }
}

void SerialConfigMenu::readLine(char *buffer, size_t max_len) {
    size_t pos = 0;
    buffer[0] = '\0';

    while (pos < max_len - 1) {
        while (!Serial.available()) {
            delay(10);
        }

        char c = Serial.read();

        if (c == '\n' || c == '\r') {
            buffer[pos] = '\0';
            Serial.println();
            return;
        }
        else if (c == 8 || c == 127) { // Backspace
            if (pos > 0) {
                pos--;
                Serial.print("\b \b");
            }
        }
        else if (c >= 32 && c <= 126) { // Printable characters
            buffer[pos++] = c;
            Serial.print(c);
        }
    }

    buffer[pos] = '\0';
    Serial.println();
}

void SerialConfigMenu::clearInputBuffer() {
    while (Serial.available()) {
        Serial.read();
    }
}
