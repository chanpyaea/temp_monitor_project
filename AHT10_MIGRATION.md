# AHT10 Sensor Update - Migration Guide

## ✅ Project Updated to Use AHT10 Sensor

**Date:** May 21, 2026
**Change:** Replaced DHT22 with AHT10 Temperature and Humidity Sensor

---

## 🔄 What Changed

### Files Modified
1. ✅ `src/drivers/Aht10Sensor.cpp` - New AHT10 driver (created)
2. ✅ `src/drivers/Aht10Sensor.h` - New AHT10 driver header (created)
3. ✅ `src/modules/SensorManager.cpp` - Updated to use AHT10
4. ✅ `src/config/AppConfig.h` - Updated pin configuration
5. ✅ `src/main.cpp` - Updated comments

### Files Removed (Old DHT22 driver)
- ❌ `src/drivers/DhtSensor.cpp` - No longer needed
- ❌ `src/drivers/DhtSensor.h` - No longer needed

---

## 🎯 Why AHT10 is Better

| Feature | DHT22 | AHT10 |
|---------|-------|-------|
| **Interface** | 1-Wire (GPIO) | I2C (shared bus) |
| **Accuracy** | ±0.5°C, ±2% RH | ±0.3°C, ±2% RH |
| **Response Time** | 2 seconds | 1 second |
| **Wiring** | Needs pull-up resistor | No pull-up needed |
| **Reliability** | Moderate | High |
| **Cost** | ~$3-5 | ~$2-4 |

**Key Advantages:**
- ✅ Uses I2C (same bus as OLED display)
- ✅ No separate GPIO pin needed
- ✅ No pull-up resistor required
- ✅ More accurate readings
- ✅ Faster response time
- ✅ Better long-term stability

---

## 🔌 New Hardware Connections

### AHT10 Wiring (I2C)

```
ESP32                    AHT10 Sensor
┌─────────┐             ┌──────────┐
│         │             │          │
│  GPIO21 ├─────────────┤ SDA      │
│  GPIO22 ├─────────────┤ SCL      │
│    3.3V ├─────────────┤ VCC      │
│     GND ├─────────────┤ GND      │
└─────────┘             └──────────┘
```

### Complete Wiring Diagram

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

**Note:** Both OLED and AHT10 share the same I2C bus (GPIO 21 & 22)

### Connection Table

| ESP32 Pin | OLED | AHT10 | Notes |
|-----------|------|-------|-------|
| GPIO 21 | SDA | SDA | I2C Data (shared) |
| GPIO 22 | SCL | SCL | I2C Clock (shared) |
| 3.3V | VCC | VCC | Power (shared) |
| GND | GND | GND | Ground (shared) |

**No GPIO 4 connection needed anymore!** ✅

---

## 📝 Configuration Changes

### Old Configuration (DHT22)
```cpp
// AppConfig.h
constexpr int TEMP_SENSOR_PIN = 4;  // GPIO pin for DHT22
```

### New Configuration (AHT10)
```cpp
// AppConfig.h
constexpr uint8_t AHT10_I2C_ADDR = 0x38;  // I2C address
```

**No pin configuration needed - uses I2C bus!**

---

## 🔧 Code Changes

### SensorManager Initialization

**Before (DHT22):**
```cpp
LOGI("Sensor", "Initializing DHT sensor on pin %d", Config::TEMP_SENSOR_PIN);
if (dht_sensor.begin(Config::TEMP_SENSOR_PIN)) {
    sensor_ok_ = true;
}
```

**After (AHT10):**
```cpp
LOGI("Sensor", "Initializing AHT10 sensor on I2C bus");
if (aht10_sensor.begin()) {
    sensor_ok_ = true;
}
```

### Sensor Reading

**Before (DHT22):**
```cpp
// Simulated data or DHT library
temperature = dht.readTemperature();
humidity = dht.readHumidity();
```

**After (AHT10):**
```cpp
// Real I2C communication
if (aht10_sensor.read(temperature, humidity)) {
    // Data is valid
}
```

---

## 🚀 How to Use

### Step 1: Wire Hardware
Connect AHT10 to I2C bus (GPIO 21 & 22) - same as OLED display.

### Step 2: Build & Upload
```bash
cd C:\scripts\temp_monitor_project
pio run -t upload
```

### Step 3: Monitor
```bash
pio device monitor -b 115200
```

### Expected Output
```
[1265][I][Sensor] Initializing AHT10 sensor on I2C bus
[1270][I][AHT10] Initializing AHT10 sensor at address 0x38
[1310][I][AHT10] Sensor calibrated (status: 0x18)
[1311][I][AHT10] Sensor initialized successfully
[1312][I][Sensor] AHT10 sensor initialized successfully
[3000][I][Sensor] Temp: 22.5°C, Humidity: 55.0%
```

---

## 🐛 Troubleshooting

### Issue: Sensor Not Found

**Error:**
```
[1270][E][AHT10] Sensor not found at address 0x38
```

**Solutions:**
1. Check wiring (SDA/SCL not swapped)
2. Verify 3.3V power
3. Try I2C scanner to find address:

```cpp
// Add to setup() temporarily
Wire.begin(21, 22);
for(uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
        Serial.printf("I2C device found at 0x%02X\n", addr);
    }
}
```

### Issue: Calibration Failed

**Error:**
```
[1280][E][AHT10] Sensor not calibrated (status: 0x00)
```

**Solutions:**
1. Power cycle the sensor (unplug and replug)
2. Wait 40ms after power-on before initialization
3. Check I2C pull-up resistors (usually built-in on ESP32)

### Issue: Reading Always Same Value

**Cause:** Sensor not responding

**Solutions:**
1. Check I2C bus speed (should be 400kHz)
2. Verify sensor is genuine AHT10 (not counterfeit)
3. Try different I2C address (some use 0x39)

---

## 📊 AHT10 Specifications

| Parameter | Value |
|-----------|-------|
| **I2C Address** | 0x38 (default) |
| **Supply Voltage** | 2.0V - 5.5V |
| **Temperature Range** | -40°C to +85°C |
| **Humidity Range** | 0% to 100% RH |
| **Temperature Accuracy** | ±0.3°C |
| **Humidity Accuracy** | ±2% RH |
| **Response Time** | 5-30 seconds (63% of step change) |
| **Measurement Time** | ~75ms |
| **Min Read Interval** | 1 second |

---

## 🔍 I2C Communication Details

### AHT10 Commands

| Command | Hex | Description |
|---------|-----|-------------|
| Initialize | 0xE1 | Calibrate sensor |
| Trigger Measurement | 0xAC | Start reading |
| Soft Reset | 0xBA | Reset sensor |

### Data Format

**6 bytes returned:**
- Byte 0: Status byte
- Bytes 1-3: Humidity (20 bits)
- Bytes 3-5: Temperature (20 bits)

**Conversion:**
```cpp
humidity = (raw_humidity * 100.0) / 1048576.0;
temperature = ((raw_temp * 200.0) / 1048576.0) - 50.0;
```

---

## ✅ Advantages of This Change

### 1. Simplified Wiring
- **Before:** 4 connections (OLED) + 3 connections (DHT22) = 7 wires
- **After:** 4 connections shared between OLED and AHT10 = 4 wires

### 2. No Pull-up Resistor Needed
- **Before:** DHT22 needs 10kΩ pull-up resistor
- **After:** AHT10 uses I2C (pull-ups built into ESP32)

### 3. Better Accuracy
- **Before:** DHT22 ±0.5°C, ±2% RH
- **After:** AHT10 ±0.3°C, ±2% RH

### 4. Faster Response
- **Before:** DHT22 needs 2 seconds between reads
- **After:** AHT10 needs 1 second between reads

### 5. More Reliable
- **Before:** DHT22 uses timing-sensitive 1-Wire protocol
- **After:** AHT10 uses robust I2C protocol

---

## 📚 Additional Resources

### Datasheets
- [AHT10 Datasheet](http://www.aosong.com/en/products-40.html)
- [AHT10 Application Note](http://www.aosong.com/en/products-40.html)

### Libraries (Optional)
If you want to use a library instead of our driver:
```ini
# In platformio.ini
lib_deps =
    adafruit/Adafruit AHTX0@^2.0.0
```

### I2C Tools
```bash
# Scan I2C bus (Arduino sketch)
#include <Wire.h>
void setup() {
    Wire.begin(21, 22);
    Serial.begin(115200);
    for(uint8_t addr = 1; addr < 127; addr++) {
        Wire.beginTransmission(addr);
        if (Wire.endTransmission() == 0) {
            Serial.printf("Found: 0x%02X\n", addr);
        }
    }
}
```

---

## 🎯 Migration Checklist

- [x] AHT10 driver created
- [x] SensorManager updated
- [x] AppConfig updated
- [x] DHT22 driver removed
- [x] Wiring diagram updated
- [x] Documentation updated
- [x] Build configuration verified
- [x] Ready to use!

---

## 🚀 Next Steps

1. **Wire your AHT10** to I2C bus (GPIO 21 & 22)
2. **Build the project** - `pio run`
3. **Upload firmware** - `pio run -t upload`
4. **Monitor output** - `pio device monitor -b 115200`
5. **Verify readings** - Should see temperature and humidity

---

## 📞 Support

If you have issues:
1. Check wiring (SDA/SCL correct)
2. Verify I2C address (0x38 or 0x39)
3. Run I2C scanner to detect devices
4. Check serial output for error messages

---

**Migration Complete!** ✅

Your project now uses the superior AHT10 sensor with I2C communication!

**Updated:** May 21, 2026 at 4:43 PM
