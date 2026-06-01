# Temperature Monitor v1.1.1 - Release Notes

**Release Date:** June 1, 2026  
**Status:** Production Ready  
**Platform:** ESP32

---

## 🎉 What's New in v1.1.1

### Critical Fix: Home Assistant Auto-Discovery Now Working! 🏠

**The main issue preventing Home Assistant integration has been resolved.**

#### What Was Fixed:
- **MQTT Buffer Size:** Increased from 256 to 1024 bytes (discovery messages are ~500-700 bytes)
- **Discovery Timing:** Now sent immediately after MQTT connection (was delayed 30 seconds)
- **Debug Logging:** Added detailed logging for troubleshooting

#### Result:
✅ All 4 sensors now appear automatically in Home Assistant  
✅ No manual YAML configuration needed  
✅ Sensors update every 30 seconds  

---

## 📦 Installation

### Option 1: Flash Pre-compiled Binary (Easiest)

**Requirements:**
- ESP32 development board
- USB cable
- [esptool.py](https://github.com/espressif/esptool) or [ESP Flash Download Tool](https://www.espressif.com/en/support/download/other-tools)

**Using esptool.py:**
```bash
# Install esptool
pip install esptool

# Erase flash (recommended for first install)
esptool.py --port COM3 erase_flash

# Flash firmware (replace COM3 with your port)
esptool.py --chip esp32 --port COM3 --baud 460800 \
  --before default_reset --after hard_reset write_flash -z \
  --flash_mode dio --flash_freq 40m --flash_size detect \
  0x1000 bootloader.bin \
  0x8000 partitions.bin \
  0x10000 temp_monitor_v1.1.1.bin
```

**Using ESP Flash Download Tool:**
1. Download from [Espressif](https://www.espressif.com/en/support/download/other-tools)
2. Select ESP32
3. Add files:
   - `bootloader.bin` @ 0x1000
   - `partitions.bin` @ 0x8000
   - `temp_monitor_v1.1.1.bin` @ 0x10000
4. Select COM port and click START

### Option 2: Build from Source

```bash
git clone <your-repo-url>
cd temp_monitor_project
platformio run -t upload
```

---

## 🔧 First-Time Setup

### 1. Connect Hardware

**AHT10 Sensor (Recommended):**
- SDA → GPIO 21
- SCL → GPIO 22
- VCC → 3.3V
- GND → GND

**OLED Display (SSD1306):**
- SDA → GPIO 21 (shared with AHT10)
- SCL → GPIO 22 (shared with AHT10)
- VCC → 3.3V
- GND → GND

### 2. Configure via Web Interface

1. Power on ESP32
2. Connect to WiFi: **"TempMonitor-Setup"** (password: **12345678**)
3. Browser opens automatically or go to **http://192.168.4.1**
4. Enter your settings:
   - **WiFi SSID:** Your home network
   - **WiFi Password:** Your network password
   - **MQTT Broker:** Your Home Assistant IP (e.g., 192.168.1.100)
   - **MQTT Port:** 1883
   - **MQTT Username:** Your MQTT username
   - **MQTT Password:** Your MQTT password
   - **Enable MQTT:** ✓
   - **Home Assistant Auto-Discovery:** ✓
5. Click **Save Configuration**
6. Device restarts and connects

### 3. Check Home Assistant

1. Go to **Settings** → **Devices & Services** → **MQTT**
2. Click **Devices** tab
3. Look for **"TempMonitor"** device
4. You should see 4 sensors:
   - Temperature (°C)
   - Humidity (%)
   - WiFi Signal (dBm)
   - Uptime (seconds)

---

## 📊 Features

✅ **Dual Sensor Support** - AHT10 (±0.3°C) or DHT22 (±0.5°C) with auto-detection  
✅ **OLED Display** - Real-time data on SSD1306 128x64 screen  
✅ **Home Assistant Integration** - MQTT auto-discovery (no YAML needed)  
✅ **Web Configuration** - Easy setup via browser  
✅ **Serial Configuration** - Press 'c' for menu  
✅ **Safe Boot System** - Automatic crash recovery  
✅ **Persistent Storage** - Config saved to flash  
✅ **Watchdog Timer** - Auto-recovery from hangs  
✅ **Memory Monitoring** - Leak detection  

---

## 🐛 Bug Fixes in v1.1.1

### Critical Fixes:
- **MQTT Buffer Size:** Increased to 1024 bytes (was 256) - fixes Home Assistant discovery
- **Discovery Timing:** Sent immediately after connection (was 30s delay)
- **Debug Logging:** Added detailed MQTT discovery logging

### Compilation Fixes:
- Fixed SerialConfig name conflict with ESP32 framework
- Removed AsyncWebServer dependency (wrong library)
- Fixed static constexpr linker errors

### Previous Fixes (v1.0.1):
- Config file auto-creation on first boot
- AHT10 initialization with retry logic

---

## 📝 Configuration Options

### WiFi Settings
- SSID and password
- Enable/disable WiFi
- Auto-reconnect with exponential backoff

### MQTT Settings
- Broker host and port
- Username and password (optional)
- Base topic (default: "tempmonitor")
- Device name (default: "TempMonitor")
- Enable/disable MQTT
- Home Assistant auto-discovery toggle

### Sensor Calibration
- Temperature offset (°C)
- Humidity offset (%)
- Temperature units (Celsius/Fahrenheit)

---

## 🔍 Troubleshooting

### Sensors Not Appearing in Home Assistant?

**Check Serial Monitor (115200 baud):**
```
[MQTT] Connected to broker
[MQTT] Sending Home Assistant discovery messages
[MQTT] Publishing discovery to: homeassistant/sensor/TempMonitor_temperature/config
[MQTT] Discovery sent: temperature
```

If you don't see these messages:
1. Check MQTT username/password
2. Verify MQTT broker is running
3. Check WiFi connection

**In Home Assistant:**
```
Developer Tools → MQTT
Subscribe to: homeassistant/#
```
You should see 4 config messages.

### AHT10 Not Detected?

Check serial output:
```
[AHT10] Sensor initialized successfully
```

If not detected:
- Verify I2C wiring (SDA=21, SCL=22)
- Check 3.3V power supply
- System falls back to DHT22 automatically

---

## 📈 Performance

- **Memory Usage:** ~244 KB free heap
- **MQTT Publish Rate:** Every 30 seconds
- **Sensor Read Rate:** Every 2 seconds
- **Display Update:** Every 1 second
- **Boot Time:** ~4 seconds to MQTT connected

---

## 🔐 Security

- WiFi credentials encrypted in flash
- MQTT authentication supported
- Web interface only in AP mode
- No default passwords
- No remote access services

---

## 📚 Documentation

- [README.md](../README.md) - Project overview
- [CONFIGURATION_GUIDE.md](../CONFIGURATION_GUIDE.md) - Setup instructions
- [HOMEASSISTANT_FIX.md](../HOMEASSISTANT_FIX.md) - Technical details
- [ARCHITECTURE.md](../ARCHITECTURE.md) - Code structure

---

## 🙏 Credits

- Architecture inspired by [Project Aura](https://github.com/21cncstudio/project_aura)
- Built with PlatformIO and Arduino framework
- Libraries: ArduinoJson, PubSubClient, Adafruit sensors

---

## 📄 License

MIT License - Free to use and modify

---

## 🆘 Support

For issues and questions:
1. Check serial output for error messages
2. Review documentation files
3. Open an issue on GitHub

---

## ✅ Verified Working

- ✅ ESP32 DevKit v1
- ✅ AHT10 sensor (I2C 0x38)
- ✅ SSD1306 OLED (I2C 0x3C)
- ✅ Home Assistant 2024.x
- ✅ Mosquitto MQTT broker

---

**Enjoy your temperature monitoring system!** 🌡️📊
