# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.2.0] - 2026-06-03

### Added
- **Data Logging & History** - 24-hour circular buffer with statistics
  - Stores sensor readings every 5 minutes (288 entries = 24 hours)
  - Automatic calculation of min/max/avg for temperature and humidity
  - Memory-efficient circular buffer implementation
  - Zero external storage required - all in RAM
- **Web History Endpoints** - RESTful API for data access
  - `/history` - JSON endpoint with full history and statistics
  - `/export` - CSV export for spreadsheet analysis
  - Accessible from web configuration portal
- **MQTT History Publishing** - Statistics over MQTT
  - Publishes min/max/avg to `<base_topic>/history`
  - Compatible with Home Assistant for trend monitoring
  - Separate from regular sensor data publishing
- **Display Statistics** - Enhanced OLED display option
  - New `updateWithStats()` method shows min/max/avg on screen
  - Shows sample count for data confidence
  - Compact layout fits all info on 128x64 display

### Changed
- Updated `AppData.h` with `HistoryEntry` and `DataStats` structures
- Added `DataLogger` module to manage history buffer
- Extended `MqttManager` with `publishHistory()` method
- Enhanced `DisplayManager` with statistics display capability
- Web interface now includes "History" button on main menu

### Technical Details
- **Memory Usage**: ~9KB RAM for 288 history entries (24h @ 5min intervals)
- **History Format**: Timestamp + Temperature + Humidity per entry
- **Statistics**: Calculated on-the-fly from circular buffer
- **Export Formats**: JSON (with metadata) and CSV (compatible with Excel/Google Sheets)

## [1.1.1] - 2026-06-01

### Fixed
- **MQTT Discovery Not Working** - Critical fix for Home Assistant auto-discovery
  - Increased MQTT buffer size from 256 to 1024 bytes (discovery messages are ~500-700 bytes)
  - Discovery messages now sent immediately after MQTT connection (was delayed 30 seconds)
  - Added debug logging for discovery topics and payloads
  - Added error logging if discovery publish fails
- **Compilation Errors** - Fixed multiple build issues
  - Renamed `SerialConfig` to `SerialConfigMenu` (name conflict with ESP32 framework enum)
  - Removed AsyncWebServer dependency (wrong library for RP2040 was being pulled)
  - Using built-in WebServer and DNSServer from ESP32 framework
  - Fixed static constexpr linker errors by moving constants to file scope

### Changed
- MQTT buffer size increased to 1024 bytes for reliable discovery message delivery
- Discovery messages now publish immediately on connection instead of waiting for first data publish

## [1.1.0] - 2026-06-01

### Added
- **Serial Configuration Menu** - Interactive USB serial menu for configuring all settings
  - Press 'c' in serial monitor to enter configuration mode
  - Configure WiFi, MQTT, and sensor calibration via serial
  - View current status and save settings
  - 30-second timeout for inactive sessions
- **Web Configuration Interface** - Browser-based configuration portal
  - Automatic AP mode when WiFi not configured
  - Captive portal for easy access (connects to "TempMonitor-Setup")
  - Configure all settings via web browser
  - Real-time status page showing WiFi, MQTT, and system info
  - Responsive design for mobile and desktop
- **Auto AP Mode** - Starts WiFi access point automatically if no WiFi configured
  - SSID: "TempMonitor-Setup"
  - Password: "12345678"
  - Access web interface at http://192.168.4.1

### Changed
- Added ESP Async WebServer library dependency
- Updated main.cpp to integrate serial and web configuration
- Improved user experience with multiple configuration methods

## [1.0.1] - 2026-06-01

### Fixed
- **StorageManager**: Fixed config file not being created on first boot
  - Added automatic config file creation when `loadConfig()` fails
  - `commitLastGood()` now creates config file if it doesn't exist before backup
  - Resolves "no permits for creation" error on fresh installations
- **AHT10 Driver**: Improved sensor initialization reliability
  - Increased power-on delay from 40ms to 100ms
  - Added retry logic (3 attempts) for calibration
  - Skip calibration command if sensor is already calibrated (status bit 3)
  - Added detailed status logging for debugging
  - Better error messages showing I2C error codes

### Changed
- Removed temporary diagnostic tools (I2C scanner, LittleFS formatter)
- Cleaned up `platformio.ini` (removed `format_fs` environment)

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
