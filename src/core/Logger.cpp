#include "Logger.h"
#include <stdarg.h>

namespace Logger {
    static Stream *s_serial = nullptr;
    static Level s_level = INFO;

    void begin(Stream &serial, Level level) {
        s_serial = &serial;
        s_level = level;
    }

    void setLevel(Level level) {
        s_level = level;
    }

    Level getLevel() {
        return s_level;
    }

    void log(Level level, const char *tag, const char *format, ...) {
        if (!s_serial || level > s_level) {
            return;
        }

        // Level prefix
        const char *level_str;
        switch (level) {
            case ERROR: level_str = "E"; break;
            case WARN:  level_str = "W"; break;
            case INFO:  level_str = "I"; break;
            case DEBUG: level_str = "D"; break;
            default:    level_str = "?"; break;
        }

        // Print timestamp, level, and tag
        char prefix[64];
        snprintf(prefix, sizeof(prefix), "[%lu][%s][%s] ",
                 millis(), level_str, tag);
        s_serial->print(prefix);

        // Print formatted message
        char buffer[256];
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);

        s_serial->println(buffer);
    }
}
