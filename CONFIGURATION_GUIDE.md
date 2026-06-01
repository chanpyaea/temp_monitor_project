# Configuration Features Summary

## What's New in v1.1.0

### 1. Serial Configuration Menu
- **File:** `src/ui/SerialConfig.cpp/h`
- **How to use:** Press 'c' in serial monitor (115200 baud)
- **Features:**
  - Interactive menu system
  - Configure WiFi (SSID, password, enable/disable)
  - Configure MQTT (host, port, username, password, topics)
  - Sensor calibration (temperature/humidity offsets)
  - View current status
  - Save configuration to flash
  - 30-second timeout for inactive sessions

### 2. Web Configuration Interface
- **File:** `src/ui/WebConfig.cpp/h`
- **How to use:** 
  - First boot: Connect to "TempMonitor-Setup" WiFi (password: 12345678)
  - Access: http://192.168.4.1 (AP mode) or http://[device-ip] (normal mode)
- **Features:**
  - Automatic AP mode when no WiFi configured
  - Captive portal (redirects all requests to config page)
  - Responsive web interface
  - Configure all settings via browser
  - Real-time status page
  - Restart device from web interface

### 3. Integration
- **Modified:** `src/main.cpp`
- **Changes:**
  - Added SerialConfig and WebConfig managers
  - Auto-start AP mode if WiFi not configured
  - Poll both config interfaces in main loop
  - Display helpful messages on boot

## How to Build and Upload

```bash
cd "C:\Users\ACER\Documents\PlatformIO\Projects\temp_monitor_project"
platformio run -t upload
platformio device monitor -b 115200
```

## First-Time Setup Workflow

**Option A: Web Interface (Easiest)**
1. Upload firmware
2. Device starts in AP mode (no WiFi configured)
3. Connect phone/laptop to "TempMonitor-Setup" WiFi
4. Browser opens automatically or go to http://192.168.4.1
5. Enter WiFi and MQTT settings
6. Save - device restarts and connects

**Option B: Serial Menu**
1. Upload firmware
2. Open serial monitor at 115200 baud
3. Press 'c' to enter config menu
4. Follow prompts to configure
5. Save and exit

## Configuration Fields

### WiFi
- SSID (network name)
- Password
- Enable/Disable

### MQTT
- Broker Host (IP or hostname)
- Broker Port (default: 1883)
- Username (optional, leave empty if not needed)
- Password
- Base Topic (default: "tempmonitor")
- Device Name (default: "TempMonitor")
- Enable/Disable MQTT
- Home Assistant Auto-Discovery (default: enabled)

### Sensor Calibration
- Temperature Offset (°C)
- Humidity Offset (%)
- Temperature Units (Celsius/Fahrenheit)

## Dependencies Added
- ESP Async WebServer (for web interface)

## Files Created
- `src/ui/SerialConfig.h` - Serial menu header
- `src/ui/SerialConfig.cpp` - Serial menu implementation
- `src/ui/WebConfig.h` - Web interface header
- `src/ui/WebConfig.cpp` - Web interface implementation

## Files Modified
- `src/main.cpp` - Integrated new config managers
- `platformio.ini` - Added ESP Async WebServer dependency
- `CHANGELOG.md` - Documented v1.1.0 changes
- `README.md` - Updated configuration instructions

## Next Steps
1. Build and upload the firmware
2. Test serial configuration menu
3. Test web interface in AP mode
4. Configure WiFi and MQTT
5. Verify Home Assistant auto-discovery
