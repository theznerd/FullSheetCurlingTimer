#include <stdio.h>
#include "esp_log.h"
#include "adc/battery_adc.hpp"
#include "gpio/gpiostartup.hpp"
#include "gatt/gattstartup.hpp"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

namespace {
constexpr gpio_num_t kBeamGpio = GPIO_NUM_21;
constexpr char kBeamTag[] = "BeamDemo";

QueueHandle_t s_beam_event_queue = nullptr;
volatile TickType_t s_last_isr_tick = 0;

void IRAM_ATTR beam_gpio_isr_handler(void* arg) {
    const TickType_t now_tick = xTaskGetTickCountFromISR();
    const TickType_t last_tick = s_last_isr_tick;

    // Simple chatter guard (LM393 / wiring can bounce a bit)
    if ((now_tick - last_tick) < pdMS_TO_TICKS(20)) {
        return;
    }
    s_last_isr_tick = now_tick;

    const uint32_t gpio_num = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(arg));
    BaseType_t higher_prio_woken = pdFALSE;
    (void)xQueueSendFromISR(s_beam_event_queue, &gpio_num, &higher_prio_woken);
    if (higher_prio_woken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

void beam_event_task(void* /*arg*/) {
    int last_level = gpio_get_level(kBeamGpio);
    ESP_LOGI(kBeamTag, "Initial: %s (level=%d)", last_level ? "BEAM BROKEN" : "beam intact", last_level);

    while (true) {
        uint32_t gpio_num = 0;
        if (xQueueReceive(s_beam_event_queue, &gpio_num, portMAX_DELAY) != pdTRUE) {
            continue;
        }
        if (gpio_num != static_cast<uint32_t>(kBeamGpio)) {
            continue;
        }

        const int level = gpio_get_level(kBeamGpio);
        if (level == last_level) {
            continue;
        }
        last_level = level;

        // Per gpio_startup(): Low = beam intact, High = beam broken
        ESP_LOGI(kBeamTag, "%s", level ? "BEAM BROKEN" : "beam restored");

        // Beep the buzzer briefly on beam break/restored
        gpio_set_level(GPIO_NUM_19, 1); // Buzzer ON
        vTaskDelay(50 / portTICK_PERIOD_MS);
        gpio_set_level(GPIO_NUM_19, 0); // Buzzer OFF

    }

}

void start_beam_demo() {
    if (s_beam_event_queue != nullptr) {
        return;
    }

    s_beam_event_queue = xQueueCreate(16, sizeof(uint32_t));
    if (s_beam_event_queue == nullptr) {
        ESP_LOGE(kBeamTag, "Failed to create event queue");
        return;
    }

    // ISR service is installed in gpio_startup(); we only add the handler here.
    esp_err_t err = gpio_isr_handler_add(
        kBeamGpio,
        beam_gpio_isr_handler,
        reinterpret_cast<void*>(static_cast<uintptr_t>(kBeamGpio)));
    if (err != ESP_OK) {
        ESP_LOGE(kBeamTag, "gpio_isr_handler_add failed: %s", esp_err_to_name(err));
        return;
    }

    xTaskCreate(beam_event_task, "beam_event", 4096, nullptr, 10, nullptr);
    ESP_LOGI(kBeamTag, "Beam-break demo active on GPIO %d", static_cast<int>(kBeamGpio));
}
} // namespace

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

    // Beam-break interrupt demo (GPIO21). Logs "BEAM BROKEN" / "beam restored".
    start_beam_demo();

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

    // Beep the piezo three times
    for(int i = 0; i < 3; i++) {
        ESP_LOGI("FSCTMain", "Beep %d", i+1);
        gpio_set_level(GPIO_NUM_19, 1); // Buzzer ON
        vTaskDelay(50 / portTICK_PERIOD_MS);
        gpio_set_level(GPIO_NUM_19, 0); // Buzzer OFF
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}