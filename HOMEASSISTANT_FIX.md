# Home Assistant Discovery Fix - Technical Documentation

## Problem Summary

Home Assistant sensors were not appearing in the dashboard despite MQTT messages being published successfully to `tempmonitor/state`.

## Root Cause Analysis

### Issue 1: MQTT Buffer Size Too Small
**Symptom:** Discovery messages appeared to send but weren't received by Home Assistant

**Root Cause:** 
- PubSubClient library default buffer: **256 bytes**
- Home Assistant discovery messages: **~500-700 bytes**
- Messages were being silently truncated/dropped

**Evidence:**
```
Serial log showed: "Discovery sent: temperature"
But Home Assistant never received the message
```

**Fix:**
```cpp
mqtt_client_->setBufferSize(1024);  // Increased from default 256
```

### Issue 2: Discovery Timing
**Symptom:** 30-second delay before sensors appeared

**Root Cause:**
- Discovery was sent inside `publish()` function
- `publish()` only runs every 30 seconds (rate limited)
- Discovery should happen immediately after connection

**Fix:**
```cpp
// In connect() function - send immediately
if (connected) {
    LOGI("MQTT", "Connected to broker");
    if (cfg.mqtt_discovery) {
        sendDiscovery();
        discovery_sent_ = true;
    }
    return true;
}
```

### Issue 3: Compilation Errors (Blocking Development)

#### 3a. SerialConfig Name Conflict
**Error:**
```
error: 'SerialConfig' referred to as 'class'
note: 'SerialConfig' has a previous declaration here
enum SerialConfig {
```

**Root Cause:** ESP32 Arduino framework already defines `SerialConfig` as an enum in `HardwareSerial.h`

**Fix:** Renamed class to `SerialConfigMenu`

#### 3b. Wrong Library Dependency
**Error:**
```
error: #error For RASPBERRY_PI_PICO_W board using CYW43439 WiFi only
```

**Root Cause:** 
- Specified "ESP Async WebServer" in platformio.ini
- PlatformIO pulled `AsyncTCP_RP2040W` (Raspberry Pi library)
- Should use ESP32-specific libraries

**Fix:** 
- Removed external AsyncWebServer dependency
- Using built-in `WebServer` and `DNSServer` from ESP32 framework

#### 3c. Static Constexpr Linker Errors
**Error:**
```
undefined reference to `WebConfig::DNS_PORT'
multiple definition of `WebConfig::DNS_PORT'
```

**Root Cause:** 
- Static constexpr members need external definitions in C++11/14 when ODR-used
- Defining in header caused multiple definition errors

**Fix:** Moved constants to file scope in `.cpp` file
```cpp
// In WebConfig.cpp
static const uint16_t WEB_SERVER_PORT = 80;
static const uint16_t DNS_PORT = 53;
```

## Testing & Verification

### Before Fix:
```
[MQTT] Connected to broker
[MQTT] Discovery sent: temperature
[MQTT] Discovery sent: humidity
[MQTT] Discovery sent: wifi_rssi
[MQTT] Discovery sent: uptime
```
**Result:** No sensors in Home Assistant

### After Fix:
```
[MQTT] Connected to broker
[MQTT] Sending Home Assistant discovery messages
[MQTT] Publishing discovery to: homeassistant/sensor/TempMonitor_temperature/config
[MQTT] Discovery sent: temperature
[MQTT] Publishing discovery to: homeassistant/sensor/TempMonitor_humidity/config
[MQTT] Discovery sent: humidity
[MQTT] Publishing discovery to: homeassistant/sensor/TempMonitor_wifi_rssi/config
[MQTT] Discovery sent: wifi_rssi
[MQTT] Publishing discovery to: homeassistant/sensor/TempMonitor_uptime/config
[MQTT] Discovery sent: uptime
```
**Result:** ✅ All 4 sensors appear in Home Assistant immediately

## Discovery Message Format

**Topic Pattern:**
```
homeassistant/sensor/{device_name}_{sensor_name}/config
```

**Example Topic:**
```
homeassistant/sensor/TempMonitor_temperature/config
```

**Payload Structure:**
```json
{
  "unique_id": "TempMonitor_temperature",
  "name": "TempMonitor temperature",
  "state_topic": "tempmonitor/state",
  "value_template": "{{ value_json.temperature }}",
  "device_class": "temperature",
  "unit_of_measurement": "°C",
  "device": {
    "identifiers": ["TempMonitor"],
    "name": "TempMonitor",
    "model": "TempMonitor",
    "manufacturer": "DIY",
    "sw_version": "1.0.0"
  }
}
```

**Payload Size:** ~550 bytes (requires buffer > 256 bytes)

## Files Modified

### src/modules/MqttManager.cpp
- Added `setBufferSize(1024)` in `begin()`
- Moved discovery call from `publish()` to `connect()`
- Added debug logging for discovery topics and payloads
- Added error logging for failed publishes

### src/ui/SerialConfig.h/cpp
- Renamed class from `SerialConfig` to `SerialConfigMenu`
- Updated all method definitions

### src/ui/WebConfig.h/cpp
- Moved static constants to file scope
- Removed class static members

### platformio.ini
- Removed "ESP Async WebServer" dependency
- Using built-in WebServer and DNSServer

## Git Commit History

```
03fb124 - Fix: Increase MQTT buffer size and add discovery debug logging
a26a9a8 - Fix: Send MQTT discovery immediately after connection
d9aeb07 - Fix: Move all constants to file scope in WebConfig.cpp
223d786 - Fix: Remove duplicate static constexpr definitions
6798038 - Fix: Define static constexpr members outside class
af1a66f - Fix: Remove AsyncWebServer dependency, use built-in WebServer
a2504b0 - Fix: Rename SerialConfig to SerialConfigMenu to avoid name conflict
26c0663 - Add serial and web configuration interfaces (v1.1.0)
7af4513 - Fix config file creation and improve AHT10 initialization
```

## Lessons Learned

1. **Always check library buffer sizes** - Default values are often too small for complex payloads
2. **Send discovery immediately** - Don't delay critical setup messages
3. **Check for name conflicts** - Especially with framework-defined types
4. **Verify library compatibility** - PlatformIO can pull wrong platform libraries
5. **Use file-scope constants** - Simpler than dealing with C++11 static constexpr ODR rules
6. **Add debug logging** - Essential for diagnosing MQTT issues

## Performance Impact

- **Memory:** +768 bytes for larger MQTT buffer (1024 vs 256)
- **Connection time:** Discovery now happens immediately (was 30s delay)
- **Reliability:** 100% discovery success rate (was 0% due to truncation)

## Future Improvements

1. Make MQTT buffer size configurable
2. Add retry logic for failed discovery publishes
3. Add MQTT connection status to OLED display
4. Support custom discovery prefix (not just "homeassistant")
