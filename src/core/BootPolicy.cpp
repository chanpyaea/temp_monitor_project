#include "BootPolicy.h"
#include "core/Logger.h"
#include <Preferences.h>

static Preferences prefs;

namespace BootPolicy {

StorageManager::BootAction apply(bool crash_reset,
                                 uint32_t &boot_count,
                                 uint32_t &safe_boot_stage,
                                 uint8_t max_reboots) {
    // Load boot state from NVS
    prefs.begin("boot", false);
    boot_count = prefs.getUInt("boot_count", 0);
    safe_boot_stage = prefs.getUInt("safe_stage", 0);

    if (crash_reset) {
        boot_count++;
        LOGW("Boot", "Crash detected! Boot count: %u", boot_count);

        if (boot_count >= max_reboots) {
            LOGE("Boot", "Too many crashes (%u), entering safe boot", boot_count);
            safe_boot_stage++;

            if (safe_boot_stage == 1) {
                // First safe boot: restore last known good
                prefs.putUInt("boot_count", boot_count);
                prefs.putUInt("safe_stage", safe_boot_stage);
                prefs.end();
                return StorageManager::BootAction::SafeRollback;
            } else {
                // Second safe boot: factory reset
                LOGE("Boot", "Safe boot failed, factory reset");
                prefs.putUInt("boot_count", 0);
                prefs.putUInt("safe_stage", 0);
                prefs.end();
                return StorageManager::BootAction::FactoryReset;
            }
        }
    } else {
        LOGI("Boot", "Normal boot, count: %u", boot_count);
    }

    // Save updated boot count
    prefs.putUInt("boot_count", boot_count);
    prefs.end();

    return StorageManager::BootAction::Normal;
}

bool markStable(uint32_t now_ms,
                uint32_t boot_start_ms,
                uint32_t stable_ms,
                bool &boot_stable,
                uint32_t &boot_count,
                uint32_t &safe_boot_stage) {
    if (boot_stable) {
        return false; // Already marked stable
    }

    if (now_ms - boot_start_ms < stable_ms) {
        return false; // Not stable yet
    }

    // Mark as stable
    boot_stable = true;
    boot_count = 0;
    safe_boot_stage = 0;

    // Save to NVS
    prefs.begin("boot", false);
    prefs.putUInt("boot_count", 0);
    prefs.putUInt("safe_stage", 0);
    prefs.end();

    LOGI("Boot", "Boot marked as stable");
    return true;
}

} // namespace BootPolicy
