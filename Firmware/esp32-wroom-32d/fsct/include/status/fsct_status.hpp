#pragma once

#include <cstdint>
#include "esp_err.h"

namespace FSCT {

// Battery status is updated periodically from the application side.
struct BatteryStatus {
    int battery_mv;
    int battery_percent;
    uint64_t last_update_ms;
};

// Beam status is updated on GPIO edge changes.
struct BeamStatus {
    bool beam_broken;
    uint64_t last_change_ms;
};

// Battery
void fsct_status_set_battery(int battery_mv, int battery_percent, uint64_t update_ms);
BatteryStatus fsct_status_get_battery();

// Beam
void fsct_status_set_beam(bool beam_broken, uint64_t change_ms);
BeamStatus fsct_status_get_beam();

} // namespace FSCT
