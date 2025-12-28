#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "driver/gpio.h"

int fsct_gpio_position = -1;
int fsct_gpio_group = -1;
bool fsct_gpio_bringup_complete = false;

void configure_gpios() {
    gpio_config_t io_conf = {};

    // --- Group 1: Configuration GPIOs ---
    // 22 / 23 dip input for sheet position
    // 25 / 26 / 27 dip input for sheet/group number
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pin_bit_mask = 
        (1ULL << GPIO_NUM_22) | 
        (1ULL << GPIO_NUM_23) | 
        (1ULL << GPIO_NUM_25) | 
        (1ULL << GPIO_NUM_26) | 
        (1ULL << GPIO_NUM_27);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    gpio_config(&io_conf);

    // --- Group 2: Interrupt GPIOs ---
    // Will configure this later

    // --- Group 3: ADC GPIOs ---
    // Will configure this later
}

void set_fsct_gpio_variables() {
    // Read position from GPIOs 22 and 23
    int pos_bit0 = gpio_get_level(GPIO_NUM_22);
    int pos_bit1 = gpio_get_level(GPIO_NUM_23);
    fsct_gpio_position = (pos_bit1 << 1) | pos_bit0;

    // Read group from GPIOs 25, 26, and 27
    int group_bit0 = gpio_get_level(GPIO_NUM_25);
    int group_bit1 = gpio_get_level(GPIO_NUM_26);
    int group_bit2 = gpio_get_level(GPIO_NUM_27);
    fsct_gpio_group = (group_bit2 << 2) | (group_bit1 << 1) | group_bit0;
}

void gpio_startup() {
    configure_gpios();

    // We only need to read the configuration GPIOs once at startup
    // to set the position and group variables. If a user wants to
    // change these values, they must power cycle the device.
    set_fsct_gpio_variables();
    
    // error handling should be added here
    fsct_gpio_bringup_complete = true;
}