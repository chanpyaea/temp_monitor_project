# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2026-05-30

### Added
- Initial project structure with modular architecture
- Dual temperature/humidity sensor support (AHT10 I2C + DHT22)
- SSD1306 OLED display with boot screen and data view
- WiFi manager with AP mode fallback and exponential backoff retry
- MQTT client with Home Assistant auto-discovery
- LittleFS-based configuration storage with JSON persistence
- Safe boot policy with crash detection and factory reset
- Watchdog timer for system reliability
- Memory monitoring and logging
- Native test environment for unit testing (ArduinoJson v7)

### Changed
- Migrated from ArduinoJson v6 to v7 (`DynamicJsonDocument` → `JsonDocument`)
- Updated all library dependencies to latest versions
- Removed redundant `-DAPP_VERSION` build flag (defined in `AppConfig.h`)

### Fixed
- Platform configuration: `platformio/espressif32 @ ^6.13.0` caused corrupted platform install → changed to `platform = espressif32`
- `min()` type mismatch on ESP32 (`uint32_t` vs `unsigned long`) in `NetworkManager::scheduleRetry()`
- Removed corrupted `espressif32@6.13.0` platform folder from `.platformio/platforms/`

### Dependencies
| Library | Version |
|---------|---------|
| ArduinoJson | 7.4.3 |
| Adafruit SSD1306 | 2.5.17 |
| Adafruit GFX Library | 1.12.6 |
| Adafruit BusIO | 1.17.4 |
| PubSubClient | 2.8.0 |
| DHT sensor library | 1.4.7 |
