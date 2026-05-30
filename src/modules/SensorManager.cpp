#include "SensorManager.h"
#include "core/Logger.h"
#include "config/AppConfig.h"
#include "drivers/Aht10Sensor.h"
#include "drivers/DhtSensor.h"

static Aht10Sensor aht10_sensor;
static DhtSensor dht_sensor;

void SensorManager::begin(float temp_offset, float hum_offset) {
    temp_offset_ = temp_offset;
    hum_offset_ = hum_offset;

    LOGI("Sensor", "Auto-detecting temperature sensor...");

    // Try AHT10 first (I2C, more accurate)
    if (aht10_sensor.begin(Config::AHT10_I2C_ADDR)) {
        sensor_type_ = SENSOR_AHT10;
        sensor_ok_ = true;
        LOGI("Sensor", "AHT10 sensor detected and initialized (I2C 0x%02X)", Config::AHT10_I2C_ADDR);
        LOGI("Sensor", "Accuracy: ±0.3°C, ±2%% RH");
        return;
    }

    LOGW("Sensor", "AHT10 not found, trying DHT22...");

    // Fall back to DHT22 (GPIO)
    if (dht_sensor.begin(Config::DHT_SENSOR_PIN)) {
        sensor_type_ = SENSOR_DHT22;
        sensor_ok_ = true;
        LOGI("Sensor", "DHT22 sensor detected and initialized (GPIO %d)", Config::DHT_SENSOR_PIN);
        LOGI("Sensor", "Accuracy: ±0.5°C, ±2%% RH");
        return;
    }

    // No sensor found
    sensor_type_ = SENSOR_NONE;
    sensor_ok_ = false;
    LOGE("Sensor", "No temperature sensor found!");
    LOGE("Sensor", "Please check wiring:");
    LOGE("Sensor", "  - AHT10: SDA=GPIO21, SCL=GPIO22, I2C address 0x38");
    LOGE("Sensor", "  - DHT22: DATA=GPIO4, needs 10kΩ pull-up resistor");
}

SensorManager::PollResult SensorManager::poll(SensorData &data) {
    PollResult result;

    // Rate limiting
    uint32_t now = millis();
    if (now - last_poll_ms_ < Config::SENSOR_POLL_INTERVAL_MS) {
        return result;
    }
    last_poll_ms_ = now;

    if (!sensor_ok_) {
        data.has_temperature = false;
        data.has_humidity = false;
        result.error_occurred = true;
        return result;
    }

    float temp, hum;
    if (readSensor(temp, hum)) {
        // Apply offsets
        temp += temp_offset_;
        hum += hum_offset_;

        // Clamp values
        temp = constrain(temp, Config::TEMP_MIN, Config::TEMP_MAX);
        hum = constrain(hum, Config::HUM_MIN, Config::HUM_MAX);

        // Check if data changed significantly (0.1°C or 1% humidity)
        bool changed = (abs(temp - last_temp_) > 0.1f) ||
                      (abs(hum - last_hum_) > 1.0f);

        data.temperature_c = temp;
        data.humidity = hum;
        data.has_temperature = true;
        data.has_humidity = true;
        data.last_read_ms = now;

        last_temp_ = temp;
        last_hum_ = hum;
        last_read_ms_ = now;

        result.data_changed = changed;

        if (changed) {
            LOGI("Sensor", "[%s] Temp: %.1f°C, Humidity: %.1f%%",
                 sensorName(), temp, hum);
        }
    } else {
        result.error_occurred = true;
        LOGW("Sensor", "Failed to read %s sensor", sensorName());
    }

    return result;
}

void SensorManager::setOffsets(float temp_offset, float hum_offset) {
    temp_offset_ = temp_offset;
    hum_offset_ = hum_offset;
    LOGI("Sensor", "Offsets updated: temp=%.1f, hum=%.1f", temp_offset, hum_offset);
}

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

const char* SensorManager::sensorName() const {
    switch (sensor_type_) {
        case SENSOR_AHT10: return "AHT10";
        case SENSOR_DHT22: return "DHT22";
        case SENSOR_NONE:  return "NONE";
        default:           return "UNKNOWN";
    }
}
