# Project Architecture Guide

## Overview

This Temperature Monitor project follows the professional architecture patterns from Project Aura. This guide explains how each component works and how they interact.

---

## Architecture Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                         main.cpp                             │
│                    (Entry Point & Loop)                      │
└─────────────────────────────────────────────────────────────┘
                              │
                              ├─────────────────────────────┐
                              │                             │
                              ▼                             ▼
┌──────────────────────────────────────┐    ┌──────────────────────────┐
│           Core Systems                │    │      Managers            │
├──────────────────────────────────────┤    ├──────────────────────────┤
│ • Logger                              │    │ • StorageManager         │
│ • BootPolicy                          │    │ • SensorManager          │
│ • Watchdog                            │    │ • NetworkManager         │
│ • MemoryMonitor                       │    │ • MqttManager            │
└──────────────────────────────────────┘    │ • DisplayManager         │
                                             └──────────────────────────┘
                                                        │
                                                        ▼
                                             ┌──────────────────────────┐
                                             │       Drivers            │
                                             ├──────────────────────────┤
                                             │ • DhtSensor              │
                                             │ • SSD1306 (via library)  │
                                             └──────────────────────────┘
                                                        │
                                                        ▼
                                             ┌──────────────────────────┐
                                             │       Hardware           │
                                             ├──────────────────────────┤
                                             │ • DHT22 Sensor           │
                                             │ • OLED Display           │
                                             │ • ESP32 WiFi             │
                                             └──────────────────────────┘
```

---

## Component Breakdown

### 1. Core Systems (`src/core/`)

#### Logger (`Logger.cpp/h`)
**Purpose:** Centralized logging with levels

**Features:**
- Multiple log levels (ERROR, WARN, INFO, DEBUG)
- Timestamp on each message
- Tag-based filtering
- Macros for easy use: `LOGI()`, `LOGW()`, `LOGE()`, `LOGD()`

**Usage:**
```cpp
LOGI("Main", "System started");
LOGW("Sensor", "Temperature out of range: %.1f", temp);
LOGE("Network", "Connection failed");
```

**Why it's important:** Consistent logging makes debugging much easier.

---

#### BootPolicy (`BootPolicy.cpp/h`)
**Purpose:** Crash detection and safe boot

**How it works:**
1. Stores boot count in NVS (non-volatile storage)
2. Detects crash resets (watchdog, panic, etc.)
3. Increments counter on each crash
4. After 3 crashes → load last-known-good config
5. If still failing → factory reset
6. After 60 seconds stable → reset counter to 0

**State Machine:**
```
Normal Boot → Crash? → Yes → Count++ → Count >= 3? → Yes → Safe Boot
                ↓                           ↓
                No                          No
                ↓                           ↓
            Continue                    Continue
```

**Why it's important:** Prevents device from being bricked by bad configuration.

---

#### Watchdog (`Watchdog.cpp/h`)
**Purpose:** Automatic recovery from hangs

**How it works:**
- Hardware timer that must be reset periodically
- If not reset within timeout (2 minutes) → forces reboot
- `Watchdog::kick()` called in main loop

**Why it's important:** If code hangs in infinite loop, watchdog reboots the device.

---

#### MemoryMonitor (`MemoryMonitor.cpp/h`)
**Purpose:** Track heap usage and detect leaks

**Metrics tracked:**
- `free_heap` - Current free memory
- `min_free_heap` - Lowest free memory since boot
- `largest_block` - Largest contiguous allocation possible

**Why it's important:** Memory leaks cause crashes. Monitoring helps catch them early.

---

### 2. Managers (`src/modules/`)

#### StorageManager (`StorageManager.cpp/h`)
**Purpose:** Persistent configuration storage

**Features:**
- Saves config to LittleFS filesystem
- JSON format for human readability
- Debounced writes (waits 5 seconds after last change)
- Last-known-good backup
- Factory reset capability

**File Structure:**
```
/config.json           → Current configuration
/config_last_good.json → Backup (saved after stable boot)
```

**API:**
```cpp
storage.config().wifi_ssid = "MyNetwork";
storage.requestSave();  // Mark dirty, will save after 5s
storage.saveConfig(true); // Force immediate save
storage.commitLastGood(); // Save current as backup
storage.restoreLastGood(); // Rollback to backup
```

**Why debouncing?** Reduces flash wear. If user changes 10 settings, only one write happens.

---

#### SensorManager (`SensorManager.cpp/h`)
**Purpose:** Read temperature and humidity sensors

**Features:**
- Rate-limited polling (every 2 seconds)
- Temperature and humidity offsets
- Data validation (range checking)
- Change detection (only report significant changes)

**Data Flow:**
```
DHT Sensor → read() → apply offsets → validate → update SensorData
```

**PollResult:**
```cpp
struct PollResult {
    bool data_changed;    // True if temp/humidity changed significantly
    bool error_occurred;  // True if read failed
};
```

**Why rate limiting?** DHT sensors need time between reads (2 seconds minimum).

---

#### NetworkManager (`NetworkManager.cpp/h`)
**Purpose:** WiFi connection management

**State Machine:**
```
WIFI_OFF → connect() → WIFI_CONNECTING → timeout/success
                            ↓                    ↓
                       WIFI_ERROR          WIFI_CONNECTED
                            ↓                    ↓
                       retry after         disconnected?
                       exponential              ↓
                       backoff            WIFI_ERROR
```

**Retry Logic:**
- Attempt 1: 2 seconds
- Attempt 2: 4 seconds
- Attempt 3: 8 seconds
- Attempt 4: 16 seconds
- Max: 60 seconds

**Why exponential backoff?** Prevents spamming router with connection attempts.

---

#### MqttManager (`MqttManager.cpp/h`)
**Purpose:** MQTT publishing and Home Assistant integration

**Features:**
- Auto-reconnect on disconnect
- Home Assistant MQTT discovery
- JSON payload building
- Rate-limited publishing (every 30 seconds)

**Discovery Messages:**
Automatically creates entities in Home Assistant:
```json
{
  "name": "TempMonitor Temperature",
  "state_topic": "tempmonitor/state",
  "value_template": "{{ value_json.temperature }}",
  "device_class": "temperature",
  "unit_of_measurement": "°C"
}
```

**Why discovery?** No manual configuration needed in Home Assistant.

---

#### DisplayManager (`DisplayManager.cpp/h`)
**Purpose:** Update OLED display

**Layout:**
```
┌────────────────────┐
│  22.5°C            │  ← Large temperature
│                    │
│  55%               │  ← Humidity
│                    │
│ W-25 M+ 2h15m      │  ← Status line
└────────────────────┘
   ↑    ↑    ↑
   WiFi MQTT Uptime
   RSSI
```

**Update Rate:** 1 second (rate limited)

**Why rate limiting?** Display updates are slow, no need to update faster than human can see.

---

### 3. Drivers (`src/drivers/`)

#### DhtSensor (`DhtSensor.cpp/h`)
**Purpose:** Low-level DHT sensor communication

**Note:** Current implementation uses simulated data for testing. For production, replace with:
```cpp
#include <DHT.h>
DHT dht(pin, DHT22);
```

**Why separate driver?** Isolates hardware-specific code. Easy to swap sensor types.

---

### 4. Data Structures (`src/config/`)

#### SensorData (`AppData.h`)
```cpp
struct SensorData {
    float temperature_c;
    float humidity;
    bool has_temperature;
    bool has_humidity;
    uint32_t last_read_ms;
};
```

**Why flags?** Indicates if data is valid (sensor might fail).

---

#### SystemStatus (`AppData.h`)
```cpp
struct SystemStatus {
    uint32_t uptime_seconds;
    uint32_t free_heap;
    uint32_t min_free_heap;
    bool wifi_connected;
    bool mqtt_connected;
    int8_t wifi_rssi;
};
```

**Why separate?** Keeps sensor data and system status logically separated.

---

#### StoredConfig (`AppConfig.h`)
```cpp
struct StoredConfig {
    char wifi_ssid[64];
    char wifi_password[64];
    bool wifi_enabled;
    
    char mqtt_host[128];
    uint16_t mqtt_port;
    bool mqtt_enabled;
    
    float temp_offset;
    float hum_offset;
    // ... etc
};
```

**Why fixed-size arrays?** Predictable memory usage, no dynamic allocation.

---

## Execution Flow

### Boot Sequence

```
1. setup() called
   ↓
2. Initialize Serial & Logger
   ↓
3. Check reset reason (crash?)
   ↓
4. Apply BootPolicy
   ├─→ Normal: load config.json
   ├─→ Safe: load config_last_good.json
   └─→ Factory: use defaults
   ↓
5. Initialize I2C bus
   ↓
6. Initialize StorageManager
   ↓
7. Initialize DisplayManager
   ↓
8. Show boot screen (2 seconds)
   ↓
9. Initialize SensorManager
   ↓
10. Initialize NetworkManager
    ↓
11. Initialize MqttManager
    ↓
12. Setup Watchdog
    ↓
13. Enter main loop
```

---

### Main Loop

```
loop() called every ~10ms
   ↓
1. Update SystemStatus
   ↓
2. Poll SensorManager
   ├─→ If data changed: log it
   └─→ Update currentData
   ↓
3. Poll NetworkManager
   ├─→ Handle WiFi state machine
   └─→ Retry if needed
   ↓
4. Poll MqttManager
   ├─→ Maintain connection
   └─→ Publish if interval elapsed
   ↓
5. Update DisplayManager
   └─→ Refresh OLED (rate limited)
   ↓
6. Poll StorageManager
   └─→ Save config if dirty & debounced
   ↓
7. Check boot stability
   └─→ If 60s elapsed: save last good
   ↓
8. Poll MemoryMonitor
   └─→ Log heap if interval elapsed
   ↓
9. Kick Watchdog
   ↓
10. delay(10ms)
    ↓
    └─→ Back to step 1
```

---

## Key Design Patterns

### 1. Manager Pattern

**What:** Each subsystem is a class with:
- `begin()` - Initialize
- `poll()` - Called from main loop
- `isOk()` - Status check

**Why:** 
- Clear separation of concerns
- Easy to test individually
- Can be reused in other projects

---

### 2. Non-Blocking I/O

**What:** No `delay()` calls except at end of loop

**How:**
```cpp
uint32_t last_action_ms = 0;

void poll() {
    if (millis() - last_action_ms < INTERVAL_MS) {
        return; // Not time yet
    }
    last_action_ms = millis();
    
    // Do action
}
```

**Why:** Keeps system responsive. WiFi, MQTT, display all update independently.

---

### 3. State Machines

**What:** Explicit states with transitions

**Example (NetworkManager):**
```cpp
enum WifiState {
    WIFI_OFF,
    WIFI_CONNECTING,
    WIFI_CONNECTED,
    WIFI_ERROR
};

void poll() {
    switch (wifi_state_) {
        case WIFI_CONNECTING:
            if (connected) wifi_state_ = WIFI_CONNECTED;
            if (timeout) wifi_state_ = WIFI_ERROR;
            break;
        // ... etc
    }
}
```

**Why:** Makes complex async operations manageable.

---

### 4. Debounced Writes

**What:** Batch multiple changes into one write

**How:**
```cpp
void requestSave() {
    dirty_ = true;
    last_change_ms_ = millis();
}

void poll() {
    if (dirty_ && millis() - last_change_ms_ > 5000) {
        saveToFlash();
        dirty_ = false;
    }
}
```

**Why:** Reduces flash wear, improves performance.

---

### 5. Safe Boot

**What:** Automatic recovery from crashes

**How:**
1. Track boot count in NVS
2. Increment on crash
3. Rollback config after 3 crashes
4. Reset counter after stable boot

**Why:** Device never stays bricked.

---

## Memory Layout

### Static Allocation (Stack)
```
StorageManager storage;      // ~200 bytes
SensorManager sensorManager; // ~50 bytes
NetworkManager networkManager; // ~150 bytes
MqttManager mqttManager;     // ~100 bytes
DisplayManager displayManager; // ~50 bytes
MemoryMonitor memoryMonitor; // ~20 bytes
SensorData currentData;      // ~30 bytes
SystemStatus systemStatus;   // ~30 bytes
```

**Total:** ~630 bytes of static RAM

### Dynamic Allocation (Heap)
- WiFi stack: ~40 KB
- MQTT client: ~10 KB
- Display buffer: ~1 KB
- JSON parsing: ~2 KB (temporary)

**Total:** ~53 KB heap usage

**ESP32 has 320 KB RAM** - plenty of headroom!

---

## Configuration Files

### platformio.ini

**Key sections:**
```ini
[env:temp_monitor]
platform = espressif32      # ESP32 platform
board = esp32dev            # Generic ESP32
framework = arduino         # Arduino framework

lib_deps =
    adafruit/Adafruit SSD1306  # OLED display
    bblanchon/ArduinoJson      # JSON parsing
    knolleary/PubSubClient     # MQTT client
```

**Why these libraries?**
- Adafruit SSD1306: Industry standard for OLED
- ArduinoJson: Fast, efficient JSON
- PubSubClient: Lightweight MQTT

---

## Extending the Project

### Adding a New Sensor

1. **Create driver** (`src/drivers/NewSensor.h`)
```cpp
class NewSensor {
public:
    bool begin();
    bool read(float &value);
};
```

2. **Add to SensorManager**
```cpp
// In SensorManager.h
NewSensor new_sensor_;

// In SensorManager.cpp
new_sensor_.begin();
float value;
if (new_sensor_.read(value)) {
    data.new_value = value;
}
```

3. **Update SensorData**
```cpp
struct SensorData {
    // ... existing fields
    float new_value;
    bool has_new_value;
};
```

4. **Add to MQTT**
```cpp
// In MqttManager::buildPayload()
doc["new_value"] = data.new_value;

// In MqttManager::sendDiscovery()
publishDiscoveryConfig("sensor", "new_value", ...);
```

---

### Adding Web Interface

1. **Create WebManager** (`src/modules/WebManager.h`)
2. **Add routes** (status, settings, OTA)
3. **Serve HTML** from LittleFS
4. **Call from main loop**

---

## Troubleshooting Guide

### Build Errors

**Error:** `LittleFS.h: No such file`
**Fix:** Update ESP32 platform: `pio pkg update`

**Error:** `Adafruit_SSD1306.h: No such file`
**Fix:** Install library: `pio lib install "Adafruit SSD1306"`

---

### Runtime Issues

**Symptom:** Device reboots every 60 seconds
**Cause:** Watchdog timeout (loop hanging)
**Fix:** Check for blocking code, add `Watchdog::kick()`

**Symptom:** Config not saving
**Cause:** LittleFS not mounted
**Fix:** Check serial output for mount errors

**Symptom:** WiFi not connecting
**Cause:** Wrong credentials or 5GHz network
**Fix:** Verify SSID/password, use 2.4GHz

---

## Performance Benchmarks

**Typical loop time:** 10-20 ms
**Sensor read time:** 50-100 ms
**Display update time:** 20-30 ms
**MQTT publish time:** 10-50 ms
**Config save time:** 50-100 ms

**Free heap:** ~260 KB (of 320 KB)
**Min free heap:** Should stay stable (no leaks)

---

## Summary

This architecture provides:

✅ **Reliability** - Safe boot, watchdog, crash recovery
✅ **Maintainability** - Clear structure, manager pattern
✅ **Scalability** - Easy to add sensors, features
✅ **Testability** - Isolated components
✅ **Performance** - Non-blocking, efficient
✅ **Professional** - Production-ready patterns

Study this project to learn how professional firmware is structured!
