#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "stdio.h"
#include "string.h"
#include "esp_log.h"

#define LED GPIO_NUM_2
#define DELAY 200

static const char* tag = "BME680 Project";

gpio_config_t led_config = {
    .intr_type = GPIO_INTR_DISABLE,
    .mode = GPIO_MODE_OUTPUT,
    .pin_bit_mask = (1U << LED),
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .pull_up_en = GPIO_PULLUP_DISABLE
};


void setup(void)
{
    ESP_LOGI(tag, "Setting up project");
    gpio_config(&led_config);
}

void app_main() 
{
    setup();
    uint8_t toggle = 0;
    while(1)
    {
        ESP_LOGI(tag, "LED State: %s", toggle == 0 ? "OFF" : "ON");
        gpio_set_level(LED, toggle);
        toggle ^= 1;
        vTaskDelay(DELAY / portTICK_PERIOD_MS);
    }
}