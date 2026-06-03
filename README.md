# Temperature Monitor Project

A professional ESP32-based temperature and humidity monitoring system with OLED display, following Project Aura's architecture patterns.

## Features

- **Temperature & Humidity Monitoring** - Supports both AHT10 (±0.3°C) and DHT22 (±0.5°C) with automatic detection
- **OLED Display** - SSD1306 128x64 display
- **WiFi Connectivity** - Auto-connect with retry logic
- **MQTT Integration** - Home Assistant auto-discovery
- **Multiple Configuration Methods** - Serial menu, web interface, or code
- **WiFi AP Mode** - Automatic access point for initial setup
- **Web Configuration Portal** - Browser-based settings management
- **Safe Boot** - Automatic crash recovery and config rollback
- **Persistent Storage** - Configuration saved to LittleFS
- **Watchdog Timer** - Automatic recovery from hangs
- **Memory Monitoring** - Heap usage tracking
- **Professional Architecture** - Manager pattern, state machines

## Hardware Requirements

- **ESP32 Development Board** (any variant)
- **Temperature/Humidity Sensor** - Choose one:
  - **AHT10** (I2C, ±0.3°C, recommended) OR
  - **DHT22/DHT11** (GPIO, ±0.5°C, alternative)
- **SSD1306 OLED Display** (128x64, I2C)
- **Jumper Wires**
- **10kΩ Resistor** (only if using DHT22)

## Wiring

### Option 1: AHT10 (Recommended - More Accurate)
**I2C Bus (OLED Display + AHT10 Sensor - Shared)**
- SDA → GPIO 21 (both OLED and AHT10)
- SCL → GPIO 22 (both OLED and AHT10)
- VCC → 3.3V (both devices)
- GND → GND (both devices)

**Note:** Both OLED and AHT10 share the same I2C bus. No pull-up resistor needed!

### Option 2: DHT22 (Alternative)
**I2C Bus (OLED Display)**
- SDA → GPIO 21
- SCL → GPIO 22
- VCC → 3.3V
- GND → GND

**DHT22 Sensor**
- DATA → GPIO 4
- VCC → 3.3V
- GND → GND
- 10kΩ pull-up resistor between DATA and VCC

**Note:** Firmware automatically detects which sensor you have connected!

## Software Requirements

- [PlatformIO](https://platformio.org/)
- VS Code with PlatformIO extension (recommended)

## Building and Flashing

### Option 1: Download Pre-built Binaries (Easiest)

Download the latest firmware binaries from the [GitHub Releases](https://github.com/chanpyaea/temp_monitor_project/releases) page.

Each release includes:
- `temp_monitor_vX.X.X.bin` - Main firmware
- `bootloader.bin` - ESP32 bootloader
- `partitions.bin` - Partition table
- `checksums.txt` - SHA256 verification
- `FLASH_INSTRUCTIONS.md` - Detailed flashing guide

**Flash using esptool.py:**
```bash
esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 460800 write_flash \
  0x1000 bootloader.bin \
  0x8000 partitions.bin \
  0x10000 temp_monitor_vX.X.X.bin
```

**Windows (COM port):**
```bash
esptool.py --chip esp32 --port COM3 --baud 460800 write_flash \
  0x1000 bootloader.bin \
  0x8000 partitions.bin \
  0x10000 temp_monitor_vX.X.X.bin
```

**Flash using ESP Flash Download Tool (Windows GUI):**
1. Download [ESP Flash Download Tool](https://www.espressif.com/en/support/download/other-tools)
2. Select ESP32 chip
3. Add files with addresses:
   - `bootloader.bin` @ `0x1000`
   - `partitions.bin` @ `0x8000`
   - `temp_monitor_vX.X.X.bin` @ `0x10000`
4. Select COM port and click START

### Option 2: Build from Source

```bash
# Clone or navigate to project directory
cd temp_monitor_project

# Build the project
pio run

# Upload to ESP32
pio run -t upload

# Monitor serial output
pio device monitor -b 115200
```

## Configuration

The firmware provides **three ways** to configure settings:

### Method 1: Serial Configuration Menu (Recommended for initial setup)

1. Connect via USB and open serial monitor at 115200 baud
2. Press **'c'** to enter configuration menu
3. Follow the interactive prompts to configure:
   - WiFi credentials (SSID and password)
   - MQTT broker settings (host, port, username, password)
   - Sensor calibration offsets
4. Save and restart

**Example:**
```
Press 'c' to enter configuration menu
========================================
    TEMPERATURE MONITOR CONFIG
========================================
1. WiFi Settings
2. MQTT Settings
3. Sensor Calibration
4. View Status
5. Save & Exit
6. Exit without saving
========================================
Select option (1-6):
```

### Method 2: Web Configuration Interface (Easiest)

**First-time setup (no WiFi configured):**
1. Power on the device
2. Connect to WiFi network: **"TempMonitor-Setup"** (password: **12345678**)
3. Open browser and go to **http://192.168.4.1**
4. Configure WiFi and MQTT settings
5. Save - device will restart and connect to your network

**After WiFi is configured:**
1. Find device IP address from serial monitor or router
2. Open browser and go to **http://[device-ip]**
3. Configure settings via web interface

### Method 3: Manual Code Configuration (Advanced)

Edit `main.cpp` and add configuration code in `setup()`:

```cpp
storage.config().wifi_ssid = "YourSSID";
storage.config().wifi_password = "YourPassword";
storage.config().wifi_enabled = true;
storage.config().mqtt_host = "192.168.1.100";
storage.config().mqtt_port = 1883;
storage.config().mqtt_user = "username";
storage.config().mqtt_password = "password";
storage.config().mqtt_enabled = true;
storage.saveConfig(true);
```

## Project Structure

```
temp_monitor_project/
├── platformio.ini              # Build configuration
├── src/
│   ├── main.cpp                # Entry point
│   ├── config/
│   │   ├── AppConfig.h         # Constants and settings
│   │   └── AppData.h           # Data structures
│   ├── core/                   # Core system
│   │   ├── Logger.cpp/h        # Logging system
│   │   ├── BootPolicy.cpp/h    # Safe boot logic
│   │   ├── Watchdog.cpp/h      # Watchdog timer
│   │   └── MemoryMonitor.cpp/h # Memory tracking
│   ├── modules/                # Feature managers
│   │   ├── StorageManager.cpp/h    # Config persistence
│   │   ├── SensorManager.cpp/h     # Sensor polling
│   │   ├── NetworkManager.cpp/h    # WiFi management
│   │   └── MqttManager.cpp/h       # MQTT client
│   ├── drivers/                # Hardware drivers
│   │   ├── Aht10Sensor.cpp/h   # AHT10 sensor driver
│   │   └── DhtSensor.cpp/h     # DHT sensor driver
│   └── ui/                     # User interface
│       ├── DisplayManager.cpp/h # OLED display
│       ├── SerialConfig.cpp/h   # Serial configuration menu
│       └── WebConfig.cpp/h      # Web configuration interface
├── include/                    # Header files
├── data/                       # Filesystem data
└── test/                       # Unit tests
```

## Architecture Overview

### Manager Pattern

Each subsystem is encapsulated in a manager class:

- **StorageManager** - Persistent configuration in LittleFS
- **SensorManager** - Sensor polling and data collection
- **NetworkManager** - WiFi connection with retry logic
- **MqttManager** - MQTT publishing and Home Assistant integration
- **DisplayManager** - OLED display updates
- **SerialConfig** - Interactive serial configuration menu
- **WebConfig** - Web-based configuration interface with AP mode

### Safe Boot System

The firmware implements a crash detection and recovery system:

1. **Normal Boot** - Load current configuration
2. **Crash Detected** - Increment boot counter
3. **3+ Crashes** - Load last-known-good configuration
4. **Still Failing** - Factory reset to defaults
5. **Stable Boot** (60s) - Save current config as "last good"

### Data Flow

```
DHT Sensor → SensorManager → currentData
                ↓
    ├─→ DisplayManager → OLED
    ├─→ MqttManager → MQTT Broker → Home Assistant
    └─→ StorageManager → LittleFS
```

## Key Features Explained

### 1. Debounced Storage

Configuration changes are batched and written to flash after 5 seconds of no changes, reducing wear on flash memory.

### 2. WiFi State Machine

WiFi connection uses exponential backoff retry logic:
- Attempt 1: retry in 2 seconds
- Attempt 2: retry in 4 seconds
- Attempt 3: retry in 8 seconds
- Max: retry in 60 seconds

### 3. Watchdog Timer

If the main loop hangs for more than 2 minutes, the watchdog forces a reboot and the safe boot system activates.

### 4. Memory Monitoring

Heap usage is logged every minute to detect memory leaks early.

### 5. Home Assistant Integration

MQTT discovery messages are automatically sent, creating entities in Home Assistant:
- `sensor.tempmonitor_temperature`
- `sensor.tempmonitor_humidity`
- `sensor.tempmonitor_wifi_rssi`
- `sensor.tempmonitor_uptime`

## Serial Output Example

```
[1234][I][Main] === Temperature Monitor v1.0.0 ===
[1235][I][Main] Reset reason: 1, Crash: NO
[1236][I][Main] Initializing I2C (SDA=21, SCL=22)
[1240][I][Storage] Initializing LittleFS...
[1245][I][Storage] LittleFS mounted successfully
[1250][I][Storage] Config loaded successfully
[1255][I][Display] Initializing SSD1306 OLED...
[1260][I][Display] Display initialized successfully
[1265][I][Sensor] Initializing DHT sensor on pin 4
[1270][I][Sensor] DHT sensor initialized successfully
[1275][I][Network] Hostname: tempmonitor-A1B2
[1280][I][Network] Connecting to WiFi: MyNetwork
[5000][I][Network] WiFi connected!
[5001][I][Network] IP: 192.168.1.50
[5002][I][MQTT] Connecting to broker...
[5100][I][MQTT] Connected to broker
[5101][I][MQTT] Sending Home Assistant discovery messages
[7000][I][Sensor] Temp: 22.5°C, Humidity: 55.0%
```

## Customization

### Change Sensor Pin

Edit `src/config/AppConfig.h`:
```cpp
constexpr int TEMP_SENSOR_PIN = 4; // Change to your pin
```

### Change I2C Pins

```cpp
constexpr int I2C_SDA_PIN = 21; // Change to your pin
constexpr int I2C_SCL_PIN = 22; // Change to your pin
```

### Adjust Polling Intervals

```cpp
constexpr uint32_t SENSOR_POLL_INTERVAL_MS = 2000;   // 2 seconds
constexpr uint32_t MQTT_PUBLISH_INTERVAL_MS = 30000; // 30 seconds
```

### Temperature Offset Calibration

```cpp
storage.config().temp_offset = -1.5f; // Subtract 1.5°C
storage.config().hum_offset = 2.0f;   // Add 2%
storage.saveConfig(true);
```

## Troubleshooting

### Display Not Working

- Check I2C address (default 0x3C, some use 0x3D)
- Verify wiring (SDA/SCL not swapped)
- Check I2C scanner output

### Sensor Not Reading

- Verify DHT sensor type (DHT11 vs DHT22)
- Check power supply (3.3V or 5V depending on sensor)
- Add 10kΩ pull-up resistor on data line if needed

### WiFi Not Connecting

- Check SSID and password
- Verify 2.4GHz network (ESP32 doesn't support 5GHz)
- Check serial output for error messages

### MQTT Not Publishing

- Verify broker IP and port
- Check username/password if required
- Ensure WiFi is connected first

## Advanced Features

### Adding New Sensors

1. Create driver in `src/drivers/`
2. Add to `SensorManager`
3. Update `SensorData` structure
4. Add MQTT discovery message

### Web Interface (Future)

The architecture supports adding a web server for configuration:
- Create `WebManager` in `src/modules/`
- Add routes for settings, status, OTA
- Serve HTML from LittleFS

### OTA Updates (Future)

Add OTA support:
```cpp
#include <ArduinoOTA.h>
// Initialize in setup()
// Handle in loop()
```

## Learning Resources

This project demonstrates professional firmware patterns from Project Aura:

- **Manager Pattern** - Separation of concerns
- **State Machines** - Non-blocking operations
- **Safe Boot** - Crash recovery
- **Debounced I/O** - Efficient flash writes
- **Watchdog** - Automatic recovery
- **Memory Monitoring** - Leak detection

See the learning guides in `C:\scripts\` for detailed explanations.

## Release Process (For Maintainers)

The project uses GitHub Actions to automatically build and publish firmware binaries.

### Creating a New Release

1. **Update version number** in `platformio.ini`:
   ```ini
   build_flags =
       -DAPP_VERSION=\"X.X.X\"
   ```

2. **Commit and push changes**:
   ```bash
   git add platformio.ini
   git commit -m "Bump version to vX.X.X"
   git push
   ```

3. **Create and push a tag**:
   ```bash
   git tag -a vX.X.X -m "Release vX.X.X - Brief description"
   git push origin vX.X.X
   ```

4. **GitHub Actions automatically**:
   - Builds the firmware for ESP32
   - Generates checksums for all binaries
   - Creates a GitHub release with all artifacts
   - Attaches flashing instructions

### Manual Workflow Trigger

You can also trigger the build manually from the GitHub Actions tab without creating a tag.

## License

MIT License - Free to use and modify

## Credits

Architecture inspired by [Project Aura](https://github.com/21cncstudio/project_aura)

## Support

For issues and questions, check:
- Serial output for error messages
- Memory usage (should stay stable)
- WiFi signal strength
- MQTT broker connectivity
