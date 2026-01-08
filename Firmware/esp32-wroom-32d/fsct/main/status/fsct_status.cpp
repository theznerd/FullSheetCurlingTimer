#include "status/fsct_status.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"

namespace FSCT {
namespace {
portMUX_TYPE s_status_lock = portMUX_INITIALIZER_UNLOCKED;

BatteryStatus s_battery_status = {
    .battery_mv = -1,
    .battery_percent = -1,
    .last_update_ms = 0,
};

BeamStatus s_beam_status = {
    .beam_broken = false,
    .last_change_ms = 0,
};
} // namespace

void fsct_status_set_battery(int battery_mv, int battery_percent, uint64_t update_ms) {
    portENTER_CRITICAL(&s_status_lock);
    s_battery_status.battery_mv = battery_mv;
    s_battery_status.battery_percent = battery_percent;
    s_battery_status.last_update_ms = update_ms;
    portEXIT_CRITICAL(&s_status_lock);
}

BatteryStatus fsct_status_get_battery() {
    portENTER_CRITICAL(&s_status_lock);
    BatteryStatus copy = s_battery_status;
    portEXIT_CRITICAL(&s_status_lock);
    return copy;
}

void fsct_status_set_beam(bool beam_broken, uint64_t change_ms) {
    portENTER_CRITICAL(&s_status_lock);
    s_beam_status.beam_broken = beam_broken;
    s_beam_status.last_change_ms = change_ms;
    portEXIT_CRITICAL(&s_status_lock);
}

BeamStatus fsct_status_get_beam() {
    portENTER_CRITICAL(&s_status_lock);
    BeamStatus copy = s_beam_status;
    portEXIT_CRITICAL(&s_status_lock);
    return copy;
}

} // namespace FSCT
