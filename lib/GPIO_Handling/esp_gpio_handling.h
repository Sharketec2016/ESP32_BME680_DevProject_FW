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