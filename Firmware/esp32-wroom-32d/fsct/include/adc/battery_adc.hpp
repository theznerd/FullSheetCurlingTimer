#pragma once

#include <stdint.h>
#include "esp_err.h"

// ---- Hardware configuration (edit these to match your voltage divider) ----
// Rtop: battery+ -> ADC node
// Rbottom: ADC node -> GND
// Battery voltage is estimated as: Vbatt = Vadc * (Rtop + Rbottom) / Rbottom
//
// You can override these at compile time with -D if desired.
#ifndef FSCT_BATTERY_DIVIDER_R_TOP_OHMS
#define FSCT_BATTERY_DIVIDER_R_TOP_OHMS 100000U // 100k
#endif

#ifndef FSCT_BATTERY_DIVIDER_R_BOTTOM_OHMS
#define FSCT_BATTERY_DIVIDER_R_BOTTOM_OHMS 47000U // 47k
#endif

// ---- Battery percentage configuration (simple linear approximation) ----
// These defaults are a basic starting point for an 18650 under light load.
// Adjust if you want the percentage to hit 0%/100% at different voltages.
#ifndef FSCT_BATTERY_EMPTY_MV
#define FSCT_BATTERY_EMPTY_MV 3000 // Arguably safe "empty" voltage for an 18650
#endif

#ifndef FSCT_BATTERY_FULL_MV
#define FSCT_BATTERY_FULL_MV 4200 // Typical full charge voltage for an 18650
#endif

// Initializes ADC1 Channel 7 (GPIO35) in oneshot mode for battery sensing.
esp_err_t battery_adc_init();

// Optional cleanup.
esp_err_t battery_adc_deinit();

// Set the divider scale factor used to convert ADC pin voltage to battery voltage.
// Example: if Rtop=200k and Rbottom=100k, scale=(Rtop+Rbottom)/Rbottom=3.0
// Default is 1.0 (no scaling).
esp_err_t battery_adc_set_divider_scale(float scale);

// Convenience helper if you'd rather specify the actual resistor values.
// Equivalent to calling battery_adc_set_divider_scale((Rtop+Rbottom)/Rbottom).
esp_err_t battery_adc_set_divider_resistors(uint32_t r_top_ohms, uint32_t r_bottom_ohms);

// Reads the ADC pin voltage (GPIO35) in millivolts.
// Returns ESP_ERR_NOT_SUPPORTED if calibration is unavailable.
esp_err_t battery_adc_read_pin_mv(int* out_mv);

// Reads the estimated battery voltage in millivolts using the configured divider scale.
esp_err_t battery_adc_read_battery_mv(int* out_mv);

// Reads a basic 0-100% estimate based on FSCT_BATTERY_EMPTY_MV..FSCT_BATTERY_FULL_MV.
// This is a linear mapping and is intentionally "basic".
esp_err_t battery_adc_read_percent(int* out_percent);
