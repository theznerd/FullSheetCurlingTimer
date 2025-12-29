#include "adc/battery_adc.hpp"
#include <cmath>

#include "esp_log.h"
#include "esp_check.h"

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

namespace {
static const char* kTag = "FSCTBatteryADC";

static adc_oneshot_unit_handle_t s_adc1_handle = nullptr;
static adc_cali_handle_t s_cali_handle = nullptr;
static bool s_cali_enabled = false;
static float s_divider_scale = 1.0f;

// GPIO35 maps to ADC1 channel 7 on ESP32.
static constexpr adc_unit_t kUnit = ADC_UNIT_1;
static constexpr adc_channel_t kChannel = ADC_CHANNEL_7;
static constexpr adc_atten_t kAtten = ADC_ATTEN_DB_12;
static constexpr adc_bitwidth_t kBitwidth = ADC_BITWIDTH_DEFAULT;

static bool init_calibration(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_bitwidth_t bitwidth) {
    if (s_cali_enabled) {
        return true;
    }

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    adc_cali_curve_fitting_config_t cali_config = {};
    cali_config.unit_id = unit;
    cali_config.atten = atten;
    cali_config.bitwidth = bitwidth;
    if (adc_cali_create_scheme_curve_fitting(&cali_config, &s_cali_handle) == ESP_OK) {
        s_cali_enabled = true;
        ESP_LOGI(kTag, "ADC calibration: curve fitting");
        return true;
    }
#elif ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    adc_cali_line_fitting_config_t cali_config = {};
    cali_config.unit_id = unit;
    cali_config.atten = atten;
    cali_config.bitwidth = bitwidth;
    // 0 lets ESP-IDF pick eFuse-calibrated Vref (or a default if not available).
    cali_config.default_vref = 0;
    if (adc_cali_create_scheme_line_fitting(&cali_config, &s_cali_handle) == ESP_OK) {
        s_cali_enabled = true;
        ESP_LOGI(kTag, "ADC calibration: line fitting");
        return true;
    }
#endif

    s_cali_enabled = false;
    s_cali_handle = nullptr;
    ESP_LOGW(kTag, "ADC calibration unavailable; voltage conversion not supported");
    return false;
}

static void deinit_calibration() {
    if (!s_cali_enabled || s_cali_handle == nullptr) {
        s_cali_enabled = false;
        s_cali_handle = nullptr;
        return;
    }

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    adc_cali_delete_scheme_curve_fitting(s_cali_handle);
#elif ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    adc_cali_delete_scheme_line_fitting(s_cali_handle);
#endif

    s_cali_enabled = false;
    s_cali_handle = nullptr;
}
} // namespace

esp_err_t battery_adc_set_divider_resistors(uint32_t r_top_ohms, uint32_t r_bottom_ohms) {
    if (r_bottom_ohms == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    const double scale = (static_cast<double>(r_top_ohms) + static_cast<double>(r_bottom_ohms)) /
                         static_cast<double>(r_bottom_ohms);
    return battery_adc_set_divider_scale(static_cast<float>(scale));
}

esp_err_t battery_adc_init() {
    if (s_adc1_handle != nullptr) {
        return ESP_OK;
    }

    // Configure divider scaling from the compile-time resistor values.
    ESP_RETURN_ON_ERROR(
        battery_adc_set_divider_resistors(FSCT_BATTERY_DIVIDER_R_TOP_OHMS, FSCT_BATTERY_DIVIDER_R_BOTTOM_OHMS),
        kTag, "invalid battery divider resistor config");

    adc_oneshot_unit_init_cfg_t init_config = {};
    init_config.unit_id = kUnit;
    init_config.ulp_mode = ADC_ULP_MODE_DISABLE;
#if defined(ADC_ONESHOT_CLK_SRC_DEFAULT)
    init_config.clk_src = ADC_ONESHOT_CLK_SRC_DEFAULT;
#elif defined(ADC_RTC_CLK_SRC_DEFAULT)
    init_config.clk_src = ADC_RTC_CLK_SRC_DEFAULT;
#else
    init_config.clk_src = static_cast<adc_oneshot_clk_src_t>(0);
#endif

    ESP_RETURN_ON_ERROR(adc_oneshot_new_unit(&init_config, &s_adc1_handle), kTag, "adc_oneshot_new_unit failed");

    adc_oneshot_chan_cfg_t chan_config = {
        .atten = kAtten,
        .bitwidth = kBitwidth,
    };
    ESP_RETURN_ON_ERROR(adc_oneshot_config_channel(s_adc1_handle, kChannel, &chan_config), kTag,
                        "adc_oneshot_config_channel failed");

    // Best-effort calibration (required for mV conversion).
    (void)init_calibration(kUnit, kChannel, kAtten, kBitwidth);

    return ESP_OK;
}

esp_err_t battery_adc_deinit() {
    deinit_calibration();

    if (s_adc1_handle != nullptr) {
        esp_err_t err = adc_oneshot_del_unit(s_adc1_handle);
        s_adc1_handle = nullptr;
        return err;
    }

    return ESP_OK;
}

esp_err_t battery_adc_set_divider_scale(float scale) {
    if (!std::isfinite(scale) || scale < 1.0f) {
        return ESP_ERR_INVALID_ARG;
    }

    s_divider_scale = scale;
    return ESP_OK;
}

esp_err_t battery_adc_read_pin_mv(int* out_mv) {
    if (out_mv == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    if (s_adc1_handle == nullptr) {
        return ESP_ERR_INVALID_STATE;
    }

    int raw = 0;
    ESP_RETURN_ON_ERROR(adc_oneshot_read(s_adc1_handle, kChannel, &raw), kTag, "adc_oneshot_read failed");

    if (!s_cali_enabled || s_cali_handle == nullptr) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    int mv = 0;
    ESP_RETURN_ON_ERROR(adc_cali_raw_to_voltage(s_cali_handle, raw, &mv), kTag, "adc_cali_raw_to_voltage failed");

    *out_mv = mv;
    return ESP_OK;
}

esp_err_t battery_adc_read_battery_mv(int* out_mv) {
    if (out_mv == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    int pin_mv = 0;
    esp_err_t err = battery_adc_read_pin_mv(&pin_mv);
    if (err != ESP_OK) {
        return err;
    }

    float batt_mv_f = static_cast<float>(pin_mv) * s_divider_scale;
    if (!std::isfinite(batt_mv_f) || batt_mv_f < 0.0f) {
        return ESP_FAIL;
    }

    *out_mv = static_cast<int>(std::lround(batt_mv_f));
    return ESP_OK;
}

esp_err_t battery_adc_read_percent(int* out_percent) {
    if (out_percent == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    int batt_mv = 0;
    esp_err_t err = battery_adc_read_battery_mv(&batt_mv);
    if (err != ESP_OK) {
        return err;
    }

    const int empty_mv = FSCT_BATTERY_EMPTY_MV;
    const int full_mv = FSCT_BATTERY_FULL_MV;
    if (full_mv <= empty_mv) {
        return ESP_ERR_INVALID_STATE;
    }

    if (batt_mv <= empty_mv) {
        *out_percent = 0;
        return ESP_OK;
    }
    if (batt_mv >= full_mv) {
        *out_percent = 100;
        return ESP_OK;
    }

    const int range_mv = full_mv - empty_mv;
    const int within_mv = batt_mv - empty_mv;

    // Rounded integer percent.
    const int percent = static_cast<int>(std::lround((within_mv * 100.0) / range_mv));
    *out_percent = (percent < 0) ? 0 : (percent > 100) ? 100 : percent;
    return ESP_OK;
}
