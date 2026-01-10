#ifndef PROJECT_H
#define PROJECT_H
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp32_home_ap.h"
#include "freertos/semphr.h"
#include "esp_gpio_handling.h"
#include "esp_bme680.h"



extern QueueHandle_t sensor_data_queue;
extern struct bme68x_data global_sensor_data; 
extern SemaphoreHandle_t sensor_data_mutex;

#endif