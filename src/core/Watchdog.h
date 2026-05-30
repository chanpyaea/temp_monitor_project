#pragma once

#include <Arduino.h>

class Watchdog {
public:
    static void setup(uint32_t timeout_ms);
    static void kick();
};
