# Temperature Monitor v1.2.0 - Release Notes

**Release Date:** June 3, 2026  
**Version:** 1.2.0  
**Status:** Production Ready

---

## 🎉 What's New: Data Logging & History

This release adds comprehensive data logging capabilities, allowing you to track temperature and humidity trends over time!

### ✨ Key Features

#### 📊 24-Hour History Buffer
- **Circular Buffer:** Stores 288 data points (24 hours at 5-minute intervals)
- **Memory Efficient:** Only ~9KB RAM usage
- **Automatic Statistics:** Real-time min/max/avg calculations
- **No External Storage:** All data stored in RAM

#### 🌐 Web API Endpoints
- **`/history`** - JSON endpoint with full history and statistics
  ```json
  {
    "count": 288,
    "capacity": 288,
    "interval_sec": 300,
    "statistics": {
      "temp_min": 22.3,
      "temp_max": 28.7,
      "temp_avg": 25.1,
      "hum_min": 45.2,
      "hum_max": 68.4,
      "hum_avg": 56.8,
      "sample_count": 288
    },
    "history": [...]
  }
  ```
- **`/export`** - CSV export for Excel/Google Sheets
  ```csv
  Timestamp,Temperature_C,Humidity_Percent
  1717423200,25.1,56.8
  1717423500,25.2,56.5
  ...
  ```

#### 📡 MQTT History Publishing
- Publishes statistics to `<base_topic>/history`
- Compatible with Home Assistant for trend tracking
- Separate from regular sensor data (doesn't affect polling rate)

#### 🖥️ Enhanced Display
- New statistics view showing min/max/avg on OLED
- Shows sample count for data confidence
- Compact layout fits all data on 128x64 display

---

## 📦 Installation

### Option 1: Flash Pre-compiled Binary (Recommended)

**Requirements:**
- ESP32 development board
- USB cable
- [esptool](https://github.com/espressif/esptool) or [ESP Flash Tool](https://www.espressif.com/en/support/download/other-tools)

**Steps:**
1. Download all `.bin` files from this release
2. Flash using esptool:
   ```bash
   esptool.py --chip esp32 --port COM3 --baud 921600 \
     --before default_reset --after hard_reset write_flash -z \
     --flash_mode dio --flash_freq 40m --flash_size detect \
     0x1000 bootloader.bin \
     0x8000 partitions.bin \
     0x10000 temp_monitor_v1.2.0.bin
   ```
3. See [FLASH_INSTRUCTIONS.md](FLASH_INSTRUCTIONS.md) for detailed guide

### Option 2: Build from Source

**Requirements:**
- [PlatformIO](https://platformio.org/)
- Git

**Steps:**
```bash
git clone https://github.com/chanpyaea/temp_monitor_project.git
cd temp_monitor_project
git checkout v1.2.0
platformio run -t upload
```

---

## 🔧 Configuration

### First-Time Setup
1. Power on ESP32
2. Connect to WiFi: **"TempMonitor-Setup"** (password: **12345678**)
3. Browser opens to http://192.168.4.1
4. Configure WiFi and MQTT settings
5. Save and restart

### Accessing History Data

**Via Web Browser:**
- Open http://[device-ip]/history for JSON
- Open http://[device-ip]/export for CSV download

**Via MQTT:**
- Subscribe to `tempmonitor/history` topic
- Statistics published every 30 seconds (same as sensor data)

**On Display:**
- Call `displayManager.updateWithStats()` to show statistics
- Shows: Current + Min/Avg/Max + Sample count

---

## 📊 Features Overview

### Data Logging
- ✅ 288 data points (24 hours @ 5-minute intervals)
- ✅ Circular buffer (oldest data auto-replaced)
- ✅ Real-time min/max/avg statistics
- ✅ JSON and CSV export formats
- ✅ MQTT publishing of statistics

### Sensors
- ✅ Dual sensor support (AHT10/DHT22 auto-detect)
- ✅ Temperature accuracy: ±0.3°C
- ✅ Humidity accuracy: ±2%
- ✅ Calibration offsets

### Display
- ✅ SSD1306 OLED (128x64)
- ✅ Real-time sensor readings
- ✅ Statistics display (new!)
- ✅ WiFi/MQTT status indicators
- ✅ Uptime counter

### Connectivity
- ✅ WiFi with auto-reconnect
- ✅ MQTT client with Home Assistant auto-discovery
- ✅ Web configuration interface
- ✅ Serial configuration menu
- ✅ RESTful API endpoints (new!)

### Reliability
- ✅ Safe boot with crash recovery
- ✅ Watchdog timer
- ✅ Memory monitoring
- ✅ Persistent configuration
- ✅ Factory reset support

---

## 🔄 Upgrading from v1.1.1

### What Changes
- New DataLogger module (~9KB RAM usage)
- Two new web endpoints: `/history` and `/export`
- New MQTT topic: `<base_topic>/history`
- Version number: 1.1.1 → 1.2.0

### Migration Steps
1. **Backup your configuration** (optional - config is preserved)
2. Flash v1.2.0 firmware
3. Device will reboot and start logging automatically
4. No configuration changes needed

### Compatibility
- ✅ Existing WiFi/MQTT settings preserved
- ✅ Home Assistant integration unchanged
- ✅ All v1.1.1 features still work
- ✅ Display layout unchanged (unless you use new stats view)

---

## 📈 Usage Examples

### View History in Browser
```bash
# Get JSON history
curl http://192.168.1.100/history

# Download CSV export
curl http://192.168.1.100/export -o history.csv
```

### Monitor with Home Assistant
```yaml
# configuration.yaml
sensor:
  - platform: mqtt
    name: "Temperature Min (24h)"
    state_topic: "tempmonitor/history"
    value_template: "{{ value_json.temp_min }}"
    unit_of_measurement: "°C"
  
  - platform: mqtt
    name: "Temperature Max (24h)"
    state_topic: "tempmonitor/history"
    value_template: "{{ value_json.temp_max }}"
    unit_of_measurement: "°C"
```

### Display Statistics in Code
```cpp
// In main.cpp loop()
DataStats stats = dataLogger.getStats();
displayManager.updateWithStats(currentData, systemStatus, stats);
```

---

## 🐛 Known Issues

None at this time. Please report issues on GitHub!

---

## 🔐 Checksums (SHA256)

Generated at build time - see `checksums.txt` in release assets.

---

## 📚 Documentation

- [CHANGELOG.md](../../CHANGELOG.md) - Complete version history
- [CONFIGURATION_GUIDE.md](../../CONFIGURATION_GUIDE.md) - Setup instructions
- [BUILD_INSTRUCTIONS.md](../../BUILD_INSTRUCTIONS.md) - Build from source
- [ROADMAP.md](../../ROADMAP.md) - Future features

---

## 🆘 Support

**Issues:** https://github.com/chanpyaea/temp_monitor_project/issues  
**Discussions:** https://github.com/chanpyaea/temp_monitor_project/discussions

When reporting issues, include:
- Serial monitor output (115200 baud)
- Hardware configuration (sensor type, display)
- Network details (WiFi, MQTT broker)
- Steps to reproduce

---

## 🙏 Credits

- **Author:** @chanpyaea
- **Contributors:** Community feedback and testing
- **Libraries:** Adafruit, ArduinoJson, PubSubClient

---

## 📜 License

This project is open source. See LICENSE file for details.

---

**Repository:** https://github.com/chanpyaea/temp_monitor_project  
**Release:** v1.2.0  
**Date:** June 3, 2026  
**Status:** ✅ Production Ready
