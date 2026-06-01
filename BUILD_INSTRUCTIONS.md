# Build and Upload Instructions

## What Was Fixed

### Issue 1: Name Conflict
**Error:** `SerialConfig` conflicted with ESP32 Arduino framework enum
**Fix:** Renamed to `SerialConfigMenu`
**Commit:** a2504b0

### Issue 2: Wrong Library Dependency
**Error:** AsyncWebServer pulled `AsyncTCP_RP2040W` (Raspberry Pi library) instead of ESP32 version
**Fix:** Removed external dependency, using built-in `WebServer` and `DNSServer` from ESP32 framework
**Commit:** af1a66f

## Build Commands

```bash
cd "C:\Users\ACER\Documents\PlatformIO\Projects\temp_monitor_project"

# Clean build (recommended after library changes)
platformio run -t clean

# Build firmware
platformio run

# Upload to ESP32
platformio run -t upload

# Monitor serial output
platformio device monitor -b 115200
```

## Expected Output After Upload

```
[Main] === Temperature Monitor v1.0.0 ===
[Main] Reset reason: 1, Crash: NO
[Storage] Initializing LittleFS...
[Storage] LittleFS mounted successfully
[Storage] Creating initial config file with defaults
[Storage] Config saved successfully
[Display] Display initialized successfully
[Sensor] Auto-detecting temperature sensor...
[AHT10] Initializing AHT10 sensor at address 0x38
[SerialConfig] Serial configuration available
[SerialConfig] Press 'c' to enter configuration menu
[Main] No WiFi configured, starting AP mode for configuration
[WebConfig] Starting AP mode: TempMonitor-Setup
[WebConfig] AP IP address: 192.168.4.1
[WebConfig] Web server started on http://192.168.4.1
[WebConfig] Connect to WiFi: TempMonitor-Setup (password: 12345678)
[Main] Initialization complete
```

## Configuration Methods

### Method 1: Web Interface (Easiest)
1. Connect to WiFi: **TempMonitor-Setup** (password: **12345678**)
2. Browser opens automatically or go to **http://192.168.4.1**
3. Configure WiFi and MQTT settings
4. Save - device restarts

### Method 2: Serial Menu
1. Open serial monitor at 115200 baud
2. Press **'c'** to enter configuration menu
3. Follow prompts
4. Save and exit

## Home Assistant Integration

After configuring MQTT:
- Sensors appear automatically in Home Assistant
- No manual configuration needed
- Entities created:
  - `sensor.tempmonitor_temperature`
  - `sensor.tempmonitor_humidity`
  - `sensor.tempmonitor_wifi_rssi`
  - `sensor.tempmonitor_uptime`

## Troubleshooting

**If build still fails:**
```bash
# Clean everything
platformio run -t clean
rm -rf .pio/libdeps/temp_monitor

# Rebuild
platformio run
```

**If AHT10 not detected:**
- Check I2C wiring (SDA=21, SCL=22)
- Verify 3.3V power
- Run I2C scanner (see logs for detected address)

**If config file errors:**
- Already fixed in v1.0.1
- Config file created automatically on first boot

## Git Status

```
af1a66f - Fix: Remove AsyncWebServer dependency, use built-in WebServer
a2504b0 - Fix: Rename SerialConfig to SerialConfigMenu to avoid name conflict
26c0663 - Add serial and web configuration interfaces (v1.1.0)
7af4513 - Fix config file creation and improve AHT10 initialization
```

## Ready to Build!

All compilation errors are fixed. Run the build commands above.
