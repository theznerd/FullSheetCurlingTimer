#include <stdio.h>
#include "esp_log.h"
#include "adc/battery_adc.hpp"
#include "gpio/gpiostartup.hpp"
#include "gatt/gattstartup.hpp"
#include "nvs_flash.h"

extern "C" void app_main(void) {
    // NVS initialization for ESP-NOW
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    } else {
        ESP_ERROR_CHECK(ret);
    }

    // GPIO initialization needs to take place before GATT initialization so
    // that any GPIO-based configuration settings are available to GATT.
    gpio_startup();

    // Battery ADC initialization (ADC1 CH7 / GPIO35)
    ESP_ERROR_CHECK(battery_adc_init());

    ESP_LOGI("FSCTMain", "FSCT Position: %d", fsct_gpio_position);
    ESP_LOGI("FSCTMain", "FSCT Group: %d", fsct_gpio_group);

    // Initialize the BLE stack and GATT services
    initialize_nimble();

    // Diagnostic Log
    ESP_LOGI("FSCTMain", "Nimble initialized");
    printf("Hello from FSCT Timer!\n");

    // Let's test out the battery ADC
    int battery_mv = 0;
    ESP_ERROR_CHECK(battery_adc_read_battery_mv(&battery_mv));
    ESP_LOGI("FSCTMain", "Battery Voltage: %d mV", battery_mv);

    int battery_percent = 0;
    ESP_ERROR_CHECK(battery_adc_read_percent(&battery_percent));
    ESP_LOGI("FSCTMain", "Battery Percentage: %d %%", battery_percent);
}