#pragma once

#include <Arduino.h>

namespace Logger {
    enum Level {
        NONE = 0,
        ERROR = 1,
        WARN = 2,
        INFO = 3,
        DEBUG = 4
    };

    void begin(Stream &serial, Level level);
    void setLevel(Level level);
    Level getLevel();

    void log(Level level, const char *tag, const char *format, ...);
}

// Logging macros
#define LOGE(tag, format, ...) Logger::log(Logger::ERROR, tag, format, ##__VA_ARGS__)
#define LOGW(tag, format, ...) Logger::log(Logger::WARN, tag, format, ##__VA_ARGS__)
#define LOGI(tag, format, ...) Logger::log(Logger::INFO, tag, format, ##__VA_ARGS__)
#define LOGD(tag, format, ...) Logger::log(Logger::DEBUG, tag, format, ##__VA_ARGS__)
