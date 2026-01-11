#include "esp_gpio_handling.h"
const char* gpio_tag = "GPIO Handling";

gpio_config_t led_config = {
    .intr_type = GPIO_INTR_DISABLE,
    .mode = GPIO_MODE_OUTPUT,
    .pin_bit_mask = (1U << LED),
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .pull_up_en = GPIO_PULLUP_DISABLE
};

void setup_gpio(void)
{
    ESP_ERROR_CHECK(gpio_config(&led_config));
}

void toggle_led(uint8_t *toggle)
{
    #ifdef ALIVE_GPIO_DEBUG
    ESP_LOGI(gpio_tag, "LED State: %s", toggle == 0 ? "OFF" : "ON");
    #endif


    if(gpio_set_level(LED, *toggle) != ESP_OK)
    {
        #ifdef ALIVE_GPIO_DEBUG
        ESP_LOGE(gpio_tag, "Failed to set LED GPIO level");
        #endif
    }
    else
    {
        *toggle ^= 1;
    }
}