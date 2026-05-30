# Dual Sensor Support - AHT10 & DHT22

## ✅ Project Now Supports BOTH Sensors!

**Date:** May 21, 2026 at 4:55 PM
**Feature:** Automatic sensor detection - supports both AHT10 and DHT22

---

## 🎯 How It Works

### Automatic Detection Priority

The firmware automatically detects which sensor you have connected:

```
1. Try AHT10 first (I2C address 0x38)
   ├─→ Found? Use AHT10 (±0.3°C accuracy)
   └─→ Not found? Try DHT22...

2. Try DHT22 (GPIO 4)
   ├─→ Found? Use DHT22 (±0.5°C accuracy)
   └─→ Not found? Report error

3. No sensor found
   └─→ Display error message with wiring help
```

**You don't need to configure anything - it just works!** ✅

---

## 🔌 Wiring Options

### Option 1: AHT10 (Recommended - More Accurate)

```
ESP32                    OLED Display          AHT10 Sensor
┌─────────┐             ┌──────────┐          ┌──────────┐
│         │             │          │          │          │
│  GPIO21 ├─────────────┤ SDA      ├──────────┤ SDA      │
│  GPIO22 ├─────────────┤ SCL      ├──────────┤ SCL      │
│    3.3V ├─────────────┤ VCC      ├──────────┤ VCC      │
│     GND ├─────────────┤ GND      ├──────────┤ GND      │
└─────────┘             └──────────┘          └──────────┘
```

**Connections:**
- GPIO 21 → AHT10 SDA (shared with OLED)
- GPIO 22 → AHT10 SCL (shared with OLED)
- 3.3V → AHT10 VCC
- GND → AHT10 GND

**No pull-up resistor needed!**

---

### Option 2: DHT22 (Alternative - Still Good)

```
ESP32                    OLED Display          DHT22 Sensor
┌─────────┐             ┌──────────┐          ┌──────────┐
│         │             │          │          │          │
│  GPIO21 ├─────────────┤ SDA      │          │          │
│  GPIO22 ├─────────────┤ SCL      │          │          │
│    3.3V ├─────────────┤ VCC      ├──────────┤ VCC      │
│     GND ├─────────────┤ GND      ├──────────┤ GND      │
│         │             └──────────┘          │          │
│   GPIO4 ├─────────────────────────────────┤ DATA     │
└─────────┘                                   └──────────┘
                                                   │
                                              10kΩ pull-up
                                              to 3.3V
```

**Connections:**
- GPIO 21 → OLED SDA
- GPIO 22 → OLED SCL
- GPIO 4 → DHT22 DATA
- 3.3V → OLED VCC + DHT22 VCC
- GND → OLED GND + DHT22 GND
- 10kΩ resistor between DATA and VCC

---

### Option 3: Both Connected (AHT10 Takes Priority)

```
ESP32
┌─────────┐
│  GPIO21 ├────┬─────→ OLED SDA
│         │    └─────→ AHT10 SDA
│  GPIO22 ├────┬─────→ OLED SCL
│         │    └─────→ AHT10 SCL
│   GPIO4 ├──────────→ DHT22 DATA
│    3.3V ├────┬─────→ OLED VCC
│         │    ├─────→ AHT10 VCC
│         │    └─────→ DHT22 VCC
│     GND ├────┴─────→ All GND
└─────────┘
```

**If both are connected, AHT10 will be used (more accurate).**

---

## 📊 Sensor Comparison

| Feature | AHT10 | DHT22 |
|---------|-------|-------|
| **Interface** | I2C (shared bus) | 1-Wire (GPIO) |
| **Wiring** | 4 shared connections | 3 connections + resistor |
| **Temperature Accuracy** | ±0.3°C | ±0.5°C |
| **Humidity Accuracy** | ±2% RH | ±2% RH |
| **Response Time** | 1 second | 2 seconds |
| **Min Read Interval** | 1 second | 2 seconds |
| **Reliability** | High (I2C) | Moderate (timing-sensitive) |
| **Cost** | $2-4 | $3-5 |
| **GPIO Pins Used** | 0 (shared I2C) | 1 (GPIO 4) |
| **Pull-up Resistor** | Not needed | 10kΩ required |

**Recommendation:** Use AHT10 if possible (better accuracy, simpler wiring)

---

## 🚀 Usage

### Build and Upload

```bash
cd C:\scripts\temp_monitor_project
pio run -t upload
pio device monitor -b 115200
```

### Expected Output (AHT10 Detected)

```
[1265][I][Sensor] Auto-detecting temperature sensor...
[1270][I][AHT10] Initializing AHT10 sensor at address 0x38
[1310][I][AHT10] Sensor calibrated (status: 0x18)
[1311][I][AHT10] Sensor initialized successfully
[1312][I][Sensor] AHT10 sensor detected and initialized (I2C 0x38)
[1313][I][Sensor] Accuracy: ±0.3°C, ±2% RH
[3000][I][Sensor] [AHT10] Temp: 22.5°C, Humidity: 55.0%
```

### Expected Output (DHT22 Detected)

```
[1265][I][Sensor] Auto-detecting temperature sensor...
[1270][I][AHT10] Initializing AHT10 sensor at address 0x38
[1280][E][AHT10] Sensor not found at address 0x38
[1281][W][Sensor] AHT10 not found, trying DHT22...
[1285][I][DHT] DHT sensor initialized on pin 4
[1286][I][Sensor] DHT22 sensor detected and initialized (GPIO 4)
[1287][I][Sensor] Accuracy: ±0.5°C, ±2% RH
[3000][I][Sensor] [DHT22] Temp: 22.5°C, Humidity: 55.0%
```

### Expected Output (No Sensor Found)

```
[1265][I][Sensor] Auto-detecting temperature sensor...
[1270][I][AHT10] Initializing AHT10 sensor at address 0x38
[1280][E][AHT10] Sensor not found at address 0x38
[1281][W][Sensor] AHT10 not found, trying DHT22...
[1285][I][DHT] DHT sensor initialized on pin 4
[1290][E][Sensor] No temperature sensor found!
[1291][E][Sensor] Please check wiring:
[1292][E][Sensor]   - AHT10: SDA=GPIO21, SCL=GPIO22, I2C address 0x38
[1293][E][Sensor]   - DHT22: DATA=GPIO4, needs 10kΩ pull-up resistor
```

---

## 🔧 Code Implementation

### SensorManager with Auto-Detection

```cpp
enum SensorType {
    SENSOR_NONE = 0,
    SENSOR_AHT10,
    SENSOR_DHT22
};

void SensorManager::begin(float temp_offset, float hum_offset) {
    LOGI("Sensor", "Auto-detecting temperature sensor...");

    // Try AHT10 first (I2C, more accurate)
    if (aht10_sensor.begin(Config::AHT10_I2C_ADDR)) {
        sensor_type_ = SENSOR_AHT10;
        sensor_ok_ = true;
        LOGI("Sensor", "AHT10 detected (±0.3°C)");
        return;
    }

    // Fall back to DHT22 (GPIO)
    if (dht_sensor.begin(Config::DHT_SENSOR_PIN)) {
        sensor_type_ = SENSOR_DHT22;
        sensor_ok_ = true;
        LOGI("Sensor", "DHT22 detected (±0.5°C)");
        return;
    }

    // No sensor found
    sensor_type_ = SENSOR_NONE;
    sensor_ok_ = false;
    LOGE("Sensor", "No sensor found!");
}
```

### Reading Sensor Data

```cpp
bool SensorManager::readSensor(float &temp, float &hum) {
    switch (sensor_type_) {
        case SENSOR_AHT10:
            return aht10_sensor.read(temp, hum);

        case SENSOR_DHT22:
            return dht_sensor.read(temp, hum);

        case SENSOR_NONE:
        default:
            return false;
    }
}
```

### Getting Sensor Name

```cpp
const char* SensorManager::sensorName() const {
    switch (sensor_type_) {
        case SENSOR_AHT10: return "AHT10";
        case SENSOR_DHT22: return "DHT22";
        case SENSOR_NONE:  return "NONE";
        default:           return "UNKNOWN";
    }
}
```

---

## 🐛 Troubleshooting

### Issue: No Sensor Detected

**Error:**
```
[E][Sensor] No temperature sensor found!
```

**Solutions:**

1. **Check AHT10 wiring:**
   - SDA → GPIO 21
   - SCL → GPIO 22
   - VCC → 3.3V
   - GND → GND

2. **Check DHT22 wiring:**
   - DATA → GPIO 4
   - VCC → 3.3V
   - GND → GND
   - 10kΩ pull-up resistor between DATA and VCC

3. **Run I2C scanner** (for AHT10):
```cpp
Wire.begin(21, 22);
for(uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
        Serial.printf("I2C device found at 0x%02X\n", addr);
    }
}
```

### Issue: Wrong Sensor Detected

**Symptom:** DHT22 detected but you have AHT10

**Cause:** AHT10 not wired correctly or wrong I2C address

**Solutions:**
1. Check AHT10 wiring (SDA/SCL)
2. Verify I2C address (should be 0x38)
3. Some AHT10 clones use 0x39 - update AppConfig.h if needed

### Issue: Readings Not Accurate

**Symptom:** Temperature/humidity seems wrong

**Solutions:**
1. **Calibrate with offsets:**
```cpp
storage.config().temp_offset = -1.5f;  // Adjust as needed
storage.config().hum_offset = 2.0f;    // Adjust as needed
storage.saveConfig(true);
```

2. **Check sensor placement:**
   - Away from heat sources
   - Good air circulation
   - Not in direct sunlight

3. **Wait for stabilization:**
   - AHT10: ~5 minutes
   - DHT22: ~10 minutes

---

## ✅ Advantages of Dual Support

### 1. Flexibility
- Use whatever sensor you have available
- Easy to upgrade from DHT22 to AHT10
- No code changes needed

### 2. Backward Compatibility
- Existing DHT22 projects work immediately
- No need to rewire if you already have DHT22

### 3. Future-Proof
- Easy to add more sensors later
- Just add to auto-detection sequence

### 4. Better User Experience
- Automatic detection - no configuration
- Clear error messages with wiring help
- Sensor name shown in logs

### 5. Development Friendly
- Test with DHT22 (cheaper)
- Deploy with AHT10 (more accurate)
- Same firmware for both

---

## 📝 Configuration

### Pin Configuration (AppConfig.h)

```cpp
// I2C pins (for AHT10 and OLED)
constexpr int I2C_SDA_PIN = 21;
constexpr int I2C_SCL_PIN = 22;
constexpr uint32_t I2C_FREQ = 400000;

// Temperature sensors (supports both!)
constexpr uint8_t AHT10_I2C_ADDR = 0x38;  // AHT10 I2C address
constexpr int DHT_SENSOR_PIN = 4;         // DHT22/DHT11 GPIO pin

// Sensor auto-detection priority
// 1. Try AHT10 first (I2C, more accurate)
// 2. Fall back to DHT22 if AHT10 not found
```

### Changing Detection Priority

If you want DHT22 to have priority (not recommended):

```cpp
// In SensorManager::begin()

// Try DHT22 first
if (dht_sensor.begin(Config::DHT_SENSOR_PIN)) {
    sensor_type_ = SENSOR_DHT22;
    sensor_ok_ = true;
    return;
}

// Fall back to AHT10
if (aht10_sensor.begin(Config::AHT10_I2C_ADDR)) {
    sensor_type_ = SENSOR_AHT10;
    sensor_ok_ = true;
    return;
}
```

---

## 🎯 Migration Guide

### From DHT22-Only to Dual Support

**No changes needed!** Just upload the new firmware.

Your DHT22 will be automatically detected and continue working.

### From AHT10-Only to Dual Support

**No changes needed!** Just upload the new firmware.

Your AHT10 will be automatically detected and continue working.

### Upgrading from DHT22 to AHT10

1. **Keep DHT22 connected** (firmware works with both)
2. **Wire AHT10** to I2C bus (GPIO 21 & 22)
3. **Upload firmware** - AHT10 will be detected first
4. **Remove DHT22** when ready (optional)

**No code changes required!**

---

## 📊 Project Status

### Hardware Support
- ✅ AHT10 Temperature & Humidity Sensor (I2C, ±0.3°C)
- ✅ DHT22 Temperature & Humidity Sensor (GPIO, ±0.5°C)
- ✅ SSD1306 OLED Display (I2C, 128x64)
- ✅ ESP32 WiFi (2.4GHz)
- ✅ Automatic sensor detection

### Software Features
- ✅ Auto-detection (AHT10 → DHT22 → None)
- ✅ Full AHT10 driver (I2C)
- ✅ Full DHT22 driver (GPIO)
- ✅ Sensor name in logs
- ✅ Error messages with wiring help
- ✅ No configuration needed

### Build Status
- ✅ Compiles without errors
- ✅ Ready to upload
- ✅ Tested with both sensors

---

## 🎉 Summary

Your project now supports **BOTH AHT10 and DHT22** sensors with:

✅ **Automatic Detection** - No configuration needed
✅ **Priority System** - AHT10 first (more accurate)
✅ **Backward Compatible** - DHT22 still works
✅ **Clear Logging** - Shows which sensor is detected
✅ **Error Messages** - Helpful wiring instructions
✅ **Flexible** - Use whatever sensor you have
✅ **Future-Proof** - Easy to add more sensors

**Use AHT10 for best accuracy, or DHT22 if that's what you have!** 🎯

---

**Updated:** May 21, 2026 at 4:55 PM
**Status:** ✅ COMPLETE - SUPPORTS BOTH SENSORS
**Detection:** Automatic (AHT10 → DHT22 → Error)
