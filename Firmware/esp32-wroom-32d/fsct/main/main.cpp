#include <stdio.h>
#include "esp_log.h"
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

    ESP_LOGI("FSCTMain", "FSCT Position: %d", fsct_gpio_position);
    ESP_LOGI("FSCTMain", "FSCT Group: %d", fsct_gpio_group);

    // Initialize the BLE stack and GATT services
    initialize_nimble();

    // Diagnostic Log
    ESP_LOGI("FSCTMain", "Nimble initialized");
    printf("Hello from FSCT Timer!\n");
}