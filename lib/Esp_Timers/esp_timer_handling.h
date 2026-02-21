#ifndef __ESP_TIMER_H
#define __ESP_TIMER_H

#include "esp_bme_errors.h"
#include "esp_timer.h"
#include "esp_bme680.h"


#define TIMER_PERIOD 1000 //1 ms as us

#define I2C_RETRY_MOD (TIMER_PERIOD * 1000) //1 second





void timer_callback(void* arg);
void timer_setup(void);








#endif