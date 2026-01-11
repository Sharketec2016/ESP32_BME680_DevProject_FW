#ifndef __ESP_BME680_H__
#define __ESP_BME680_H__
#include <stdint.h>
#include "bme68x.h"
#include "esp_bme_errors.h"
#include "esp_bme_i2c.h"


// #define PRINT_SENSOR_DATA 

#define BME_SAMPLE_MODE BME68X_SEQUENTIAL_MODE
#define DELAY_FACTOR 0.1     //units of sec



extern struct bme68x_data global_sensor_data; 



void measureBME680Data(struct bme68x_data* bme_data);
void setupBmeI2C(struct bme68x_dev* bme, uint8_t intf);
void configureBme680Sensor(void);
static void user_delay_us(uint32_t period, void *intf_ptr);
static void configHeater(void);
static void configBME(void);
void initializeBME680(void);



#endif /* __ESP_BME680_H__ */