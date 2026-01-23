/**
 * @file esp_gpio_handling.c
 * @brief GPIO configuration and LED control implementation
 * 
 * @details This file implements GPIO initialization and LED toggle functionality for the
 * ESP32. It configures GPIO2 as a simple output pin for the onboard LED and provides
 * a toggle function used by the alive task to indicate system responsiveness. The toggle
 * function maintains state via pointer parameter and includes debug logging capability.
 * 
 * Features:
 * - GPIO configuration via esp_gpio_config_t structure
 * - LED toggle with XOR operation for state management
 * - Optional debug logging for LED state changes
 * - Error handling with ESP_LOGE for failed GPIO operations
 * - No pull-up/pull-down resistors for clean output control
 * 
 * @author Matthew Buchkowski
 * @date January 2026
 * @version 1.0
 */

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