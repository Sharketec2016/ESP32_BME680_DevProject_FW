/**
 * @file esp_gpio_handling.h
 * @brief GPIO configuration and LED control interface
 * 
 * @details This header provides GPIO configuration and LED toggle functionality for the
 * ESP32 platform. The module manages GPIO setup for the onboard LED connected to GPIO2,
 * which is used as a "alive" indicator to demonstrate proper task scheduling and system
 * responsiveness. GPIO configuration includes proper pull-up/pull-down disable and
 * interrupt disable for simple output-only operation.
 * 
 * LED Configuration:
 * - GPIO pin: GPIO_NUM_2 (onboard LED)
 * - Blink interval: 1000 milliseconds
 * - Mode: GPIO_MODE_OUTPUT
 * - Interrupts: Disabled
 * - Pull-ups/downs: Disabled
 * 
 * Debug Features:
 * - ALIVE_GPIO_DEBUG macro for verbose logging of LED state changes
 * - Error checking with ESP_LOGE on GPIO operation failures
 * 
 * @author Matthew Buchkowski
 * @date January 2026
 * @version 1.0
 */

#ifndef __GPIO_HANDLING_H__
#define __GPIO_HANDLING_H__

#include "driver/gpio.h"
#include "esp_bme_errors.h"
/********Debug Macros************/
// #define ALIVE_GPIO_DEBUG

/********************************/


#define LED GPIO_NUM_2
#define BLINK_DELAY 1000  //milliseconds




void toggle_led(uint8_t *toggle);
void setup_gpio(void);

#endif  // __GPIO_HANDLING_H__