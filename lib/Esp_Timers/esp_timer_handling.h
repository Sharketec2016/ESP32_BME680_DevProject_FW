#ifndef __ESP_TIMER_H
#define __ESP_TIMER_H

#include "esp_bme_errors.h"
#include "esp_timer.h"



#define TIMER_PERIOD 1000000 //1 second as us


void timer_callback(void* arg);
void timer_setup(void);








#endif