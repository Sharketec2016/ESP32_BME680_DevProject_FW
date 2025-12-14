#ifndef PROJECT_H
#define PROJECT_H
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "bme68x.h"
#include "esp_bme_i2c.h"
#include "esp_bme_errors.h"

#define LED GPIO_NUM_2
#define BLINK_DELAY 800
#define SAMPLE_DATA_DELAY 1000


gpio_config_t led_config = {
    .intr_type = GPIO_INTR_DISABLE,
    .mode = GPIO_MODE_OUTPUT,
    .pin_bit_mask = (1U << LED),
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .pull_up_en = GPIO_PULLUP_DISABLE
};





#endif