# Getting Started Guide

## Step-by-Step Tutorial for Building and Running the Temperature Monitor

---

## Prerequisites

### Hardware
- [ ] ESP32 development board (any variant)
- [ ] DHT22 or DHT11 temperature/humidity sensor
- [ ] SSD1306 OLED display (128x64, I2C)
- [ ] Breadboard
- [ ] Jumper wires
- [ ] USB cable for programming

### Software
- [ ] [VS Code](https://code.visualstudio.com/) installed
- [ ] [PlatformIO extension](https://platformio.org/install/ide?install=vscode) installed
- [ ] USB drivers for your ESP32 board

---

## Step 1: Hardware Setup

### Wiring Diagram

```
ESP32                    OLED Display (SSD1306)
┌─────────┐             ┌──────────┐
│         │             │          │
│  GPIO21 ├─────────────┤ SDA      │
│  GPIO22 ├─────────────┤ SCL      │
│    3.3V ├─────────────┤ VCC      │
│     GND ├─────────────┤ GND      │
│         │             └──────────┘
│         │
│         │             DHT22 Sensor
│         │             ┌──────────┐
│   GPIO4 ├─────────────┤ DATA     │
│    3.3V ├─────────────┤ VCC      │
│     GND ├─────────────┤ GND      │
└─────────┘             └──────────┘
```

### Connection Table

| ESP32 Pin | Component | Component Pin |
|-----------|-----------|---------------|
| GPIO 21   | OLED      | SDA           |
| GPIO 22   | OLED      | SCL           |
| 3.3V      | OLED      | VCC           |
| GND       | OLED      | GND           |
| GPIO 4    | DHT22     | DATA          |
| 3.3V      | DHT22     | VCC           |
| GND       | DHT22     | GND           |

**Note:** Some DHT22 modules have a built-in pull-up resistor. If yours doesn't, add a 10kΩ resistor between DATA and VCC.

---

## Step 2: Software Setup

### Install PlatformIO

1. Open VS Code
2. Go to Extensions (Ctrl+Shift+X)
3. Search for "PlatformIO IDE"
4. Click Install
5. Restart VS Code

### Open the Project

1. Open VS Code
2. File → Open Folder
3. Navigate to `C:\scripts\temp_monitor_project`
4. Click "Select Folder"

You should see this structure:
```
temp_monitor_project/
├── platformio.ini
├── src/
├── include/
└── README.md
```

---

## Step 3: Build the Project

### First Build

1. Open PlatformIO sidebar (click the alien icon on the left)
2. Expand "temp_monitor" environment
3. Click "Build"

**What happens:**
- PlatformIO downloads ESP32 platform (~500 MB)
- Downloads required libraries
- Compiles the code
- Creates firmware binary

**Expected output:**
```
Building .pio/build/temp_monitor/firmware.bin
RAM:   [=         ]  12.3% (used 40312 bytes from 327680 bytes)
Flash: [====      ]  35.2% (used 461234 bytes from 1310720 bytes)
========================= [SUCCESS] Took 45.23 seconds =========================
```

### If Build Fails

**Error:** `Platform 'espressif32' not found`
**Fix:** 
```bash
pio platform install espressif32
```

**Error:** `Library not found`
**Fix:**
```bash
pio lib install
```

---

## Step 4: Upload to ESP32

### Connect ESP32

1. Plug ESP32 into USB port
2. Wait for drivers to install (Windows may take a minute)

### Find COM Port

**Windows:**
- Device Manager → Ports (COM & LPT)
- Look for "USB-SERIAL CH340" or "CP210x"
- Note the COM port (e.g., COM3)

**Mac/Linux:**
```bash
ls /dev/tty.*
# Look for /dev/ttyUSB0 or /dev/cu.usbserial-*
```

### Upload Firmware

1. In PlatformIO sidebar
2. Click "Upload"
3. Wait for upload to complete

**Expected output:**
```
Connecting........_____.
Writing at 0x00010000... (10 %)
Writing at 0x00020000... (20 %)
...
Writing at 0x00070000... (100 %)
Wrote 461234 bytes (298765 compressed) at 0x00010000 in 26.5 seconds
Hard resetting via RTS pin...
========================= [SUCCESS] Took 32.15 seconds =========================
```

### If Upload Fails

**Error:** `Failed to connect to ESP32`
**Fix:**
1. Hold BOOT button on ESP32
2. Click Upload
3. Release BOOT when "Connecting..." appears

**Error:** `Serial port not found`
**Fix:**
1. Check USB cable (must be data cable, not charge-only)
2. Install CH340 or CP210x drivers
3. Try different USB port

---

## Step 5: Monitor Serial Output

### Open Serial Monitor

1. In PlatformIO sidebar
2. Click "Monitor"
3. Or press Ctrl+Alt+M

### Expected Output

```
[1234][I][Main] === Temperature Monitor v1.0.0 ===
[1235][I][Main] Reset reason: 1, Crash: NO
[1236][I][Main] Initializing I2C (SDA=21, SCL=22)
[1240][I][Storage] Initializing LittleFS...
[1245][I][Storage] LittleFS mounted successfully
[1250][I][Storage] Config file not found, using defaults
[1255][I][Display] Initializing SSD1306 OLED...
[1260][I][Display] Display initialized successfully
[1265][I][Sensor] Initializing DHT sensor on pin 4
[1270][I][Sensor] DHT sensor initialized successfully
[1275][I][Network] Hostname: tempmonitor-A1B2
[1280][I][Main] Initialization complete
[1281][I][Main] Free heap: 287456 bytes
[3000][I][Sensor] Temp: 22.5°C, Humidity: 55.0%
[5000][I][Sensor] Temp: 22.6°C, Humidity: 54.8%
```

### Check Display

Your OLED should show:
```
┌────────────────────┐
│  22.5°C            │
│                    │
│  55%               │
│                    │
│ W- M- 0h00m        │
└────────────────────┘
```

**W-** = WiFi not connected (expected, not configured yet)
**M-** = MQTT not connected (expected)

---

## Step 6: Configure WiFi

### Method 1: Via Serial Monitor

In the serial monitor, you can't directly input commands yet (we haven't implemented a CLI). For now, we'll configure via code.

### Method 2: Edit Code (Temporary)

Add this to `main.cpp` in `setup()` after `storage.begin()`:

```cpp
// Temporary WiFi configuration
if (!storage.config().wifi_enabled) {
    LOGI("Main", "Configuring WiFi...");
    strlcpy(storage.config().wifi_ssid, "YourSSID", sizeof(storage.config().wifi_ssid));
    strlcpy(storage.config().wifi_password, "YourPassword", sizeof(storage.config().wifi_password));
    storage.config().wifi_enabled = true;
    storage.saveConfig(true);
    LOGI("Main", "WiFi configured, rebooting...");
    delay(1000);
    ESP.restart();
}
```

**Replace:**
- `YourSSID` with your WiFi network name
- `YourPassword` with your WiFi password

### Upload and Test

1. Save the file
2. Click Upload
3. Open Serial Monitor

**Expected output:**
```
[1280][I][Main] Configuring WiFi...
[1281][I][Storage] Config saved successfully
[1282][I][Main] WiFi configured, rebooting...

[After reboot]
[1275][I][Network] Hostname: tempmonitor-A1B2
[1280][I][Network] Connecting to WiFi: YourSSID
[5000][I][Network] WiFi connected!
[5001][I][Network] IP: 192.168.1.50
[5002][I][Network] RSSI: -45 dBm
```

**Display should now show:**
```
┌────────────────────┐
│  22.5°C            │
│                    │
│  55%               │
│                    │
│ W-45 M- 0h01m      │
└────────────────────┘
```

**W-45** = WiFi connected, signal strength -45 dBm

---

## Step 7: Configure MQTT (Optional)

### Prerequisites

You need an MQTT broker. Options:
- **Mosquitto** (local): `sudo apt install mosquitto`
- **Home Assistant** (built-in MQTT broker)
- **Cloud**: HiveMQ, CloudMQTT

### Configure MQTT

Add this to `main.cpp` in `setup()` after WiFi config:

```cpp
// MQTT configuration
if (!storage.config().mqtt_enabled) {
    LOGI("Main", "Configuring MQTT...");
    strlcpy(storage.config().mqtt_host, "192.168.1.100", sizeof(storage.config().mqtt_host));
    storage.config().mqtt_port = 1883;
    strlcpy(storage.config().mqtt_user, "username", sizeof(storage.config().mqtt_user));
    strlcpy(storage.config().mqtt_password, "password", sizeof(storage.config().mqtt_password));
    storage.config().mqtt_enabled = true;
    storage.saveConfig(true);
    LOGI("Main", "MQTT configured");
}
```

**Replace:**
- `192.168.1.100` with your MQTT broker IP
- `username` and `password` (or leave empty for anonymous)

### Upload and Test

**Expected output:**
```
[5100][I][MQTT] Connecting to broker...
[5150][I][MQTT] Connected to broker
[5151][I][MQTT] Sending Home Assistant discovery messages
[5152][I][MQTT] Discovery sent: temperature
[5153][I][MQTT] Discovery sent: humidity
[5154][I][MQTT] Discovery sent: wifi_rssi
[5155][I][MQTT] Discovery sent: uptime
[35000][I][MQTT] Published to tempmonitor/state
```

**Display should show:**
```
┌────────────────────┐
│  22.5°C            │
│                    │
│  55%               │
│                    │
│ W-45 M+ 0h02m      │
└────────────────────┘
```

**M+** = MQTT connected

---

## Step 8: Home Assistant Integration

### Prerequisites

- Home Assistant installed
- MQTT integration configured

### Auto-Discovery

The device automatically sends discovery messages. In Home Assistant:

1. Go to **Settings** → **Devices & Services**
2. Click **MQTT** integration
3. You should see **TempMonitor** device
4. Click on it to see entities:
   - `sensor.tempmonitor_temperature`
   - `sensor.tempmonitor_humidity`
   - `sensor.tempmonitor_wifi_rssi`
   - `sensor.tempmonitor_uptime`

### Add to Dashboard

1. Go to your dashboard
2. Click **Edit Dashboard**
3. Click **Add Card**
4. Choose **Entities Card**
5. Add the temperature and humidity sensors
6. Save

---

## Step 9: Testing

### Test Sensor

1. Breathe on the DHT sensor
2. Watch temperature and humidity change
3. Check serial output for updates
4. Verify display updates
5. Check MQTT messages in Home Assistant

### Test Safe Boot

1. Add infinite loop to `loop()`:
```cpp
void loop() {
    while(true) {} // Hang forever
    // ... rest of code
}
```

2. Upload
3. Device will hang
4. Watchdog will reboot after 2 minutes
5. After 3 reboots, safe boot activates
6. Device loads last-known-good config

**Expected output:**
```
[1235][W][Boot] Crash detected! Boot count: 1
[1235][I][Boot] Normal boot, count: 1

[After 2 minutes, watchdog reboots]

[1235][W][Boot] Crash detected! Boot count: 2
[1235][I][Boot] Normal boot, count: 2

[After 2 minutes, watchdog reboots]

[1235][W][Boot] Crash detected! Boot count: 3
[1235][E][Boot] Too many crashes (3), entering safe boot
[1236][W][Storage] Safe boot: restoring last known good config
```

**Remove the infinite loop and upload again to fix!**

---

## Step 10: Customization

### Change Sensor Pin

Edit `src/config/AppConfig.h`:
```cpp
constexpr int TEMP_SENSOR_PIN = 5; // Changed from 4 to 5
```

### Change Display Update Rate

```cpp
constexpr uint32_t DISPLAY_UPDATE_INTERVAL_MS = 500; // Update every 0.5s
```

### Add Temperature Offset

In `main.cpp` after `storage.begin()`:
```cpp
storage.config().temp_offset = -1.5f; // Subtract 1.5°C
storage.config().hum_offset = 2.0f;   // Add 2%
storage.saveConfig(true);
```

### Change MQTT Topic

```cpp
strlcpy(storage.config().mqtt_base_topic, "home/sensors/temp1", 
        sizeof(storage.config().mqtt_base_topic));
```

---

## Troubleshooting

### Display Shows Nothing

**Check:**
1. I2C address (try 0x3D if 0x3C doesn't work)
2. Wiring (SDA/SCL not swapped)
3. Power (3.3V or 5V depending on module)

**Test I2C:**
Add to `setup()`:
```cpp
Wire.begin(21, 22);
byte error, address;
for(address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
        Serial.printf("I2C device found at 0x%02X\n", address);
    }
}
```

### Sensor Always Shows Same Value

**Cause:** Using simulated data (default implementation)

**Fix:** Install DHT library and update driver:
```cpp
// In DhtSensor.cpp
#include <DHT.h>
DHT dht(pin_, DHT22);

bool DhtSensor::begin(uint8_t pin) {
    pin_ = pin;
    dht.begin();
    return true;
}

bool DhtSensor::read(float &temperature, float &humidity) {
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();
    return !isnan(temperature) && !isnan(humidity);
}
```

### WiFi Won't Connect

**Check:**
1. SSID and password correct
2. 2.4GHz network (ESP32 doesn't support 5GHz)
3. Router not blocking device
4. Signal strength (move closer to router)

### MQTT Not Publishing

**Check:**
1. Broker IP correct
2. Port correct (default 1883)
3. Username/password if required
4. Firewall not blocking

**Test with mosquitto_sub:**
```bash
mosquitto_sub -h 192.168.1.100 -t "tempmonitor/#" -v
```

### Device Keeps Rebooting

**Cause:** Watchdog timeout or crash loop

**Check:**
1. Serial output for error messages
2. Memory usage (heap should be stable)
3. Remove recent code changes

---

## Next Steps

### Add Features

1. **Web Interface** - Configure via browser
2. **OTA Updates** - Update firmware wirelessly
3. **Data Logging** - Store history to SD card
4. **Alerts** - Send notifications on thresholds
5. **More Sensors** - Add pressure, light, etc.

### Learn More

- Read `ARCHITECTURE.md` for detailed explanations
- Study the manager classes
- Experiment with modifications
- Contribute improvements

---

## Success Checklist

- [ ] Hardware wired correctly
- [ ] Project builds without errors
- [ ] Firmware uploads successfully
- [ ] Serial output shows initialization
- [ ] Display shows temperature and humidity
- [ ] WiFi connects (if configured)
- [ ] MQTT publishes (if configured)
- [ ] Home Assistant shows device (if configured)
- [ ] Sensor readings update
- [ ] Safe boot works (tested)

---

## Getting Help

If you're stuck:

1. **Check serial output** - Most issues show error messages
2. **Verify wiring** - Double-check connections
3. **Test components individually** - Isolate the problem
4. **Check memory usage** - Heap should stay stable
5. **Review logs** - Look for warnings and errors

**Common issues are documented in README.md troubleshooting section.**

---

Congratulations! You now have a working professional-grade temperature monitor! 🎉
