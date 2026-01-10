#ifndef PROJECT_H
#define PROJECT_H
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "bme68x.h"
#include "esp_bme_i2c.h"
#include "esp_bme_errors.h"
#include "esp32_home_ap.h"
#include "freertos/semphr.h"



#define LED GPIO_NUM_2
#define BLINK_DELAY 100
#define SAMPLE_DATA_DELAY 1000

#define DELAY_FACTOR 0.1     //units of sec

#define BME_SAMPLE_MODE BME68X_SEQUENTIAL_MODE




extern QueueHandle_t sensor_data_queue;
extern struct bme68x_data global_sensor_data; 
extern SemaphoreHandle_t sensor_data_mutex;

#endif