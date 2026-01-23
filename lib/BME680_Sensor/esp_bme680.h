/**
 * @file esp_bme680.h
 * @brief BME680 environmental sensor interface and configuration
 * 
 * @details This header defines the interface for the Bosch BME680 sensor, which measures
 * temperature, pressure, humidity, and gas resistance. It includes sensor configuration
 * macros for sampling modes, oversample rates, filtering, and heater settings. The module
 * provides functions for sensor initialization, data acquisition, and air quality index (IAQ)
 * calculation. All sensor data access is protected by a mutex for thread-safe FreeRTOS
 * integration.
 * 
 * Sensor configurations:
 * - Sample mode: Forced mode for periodic measurements
 * - Temperature/Pressure/Humidity oversampling: Configurable rates
 * - IIR filtering: 15-coefficient filter for noise reduction
 * - Heater: Gas sensor heating for improved accuracy
 * 
 * @author Matthew Buchkowski
 * @date January 2026
 * @version 1.0
 */

#ifndef __ESP_BME680_H__
#define __ESP_BME680_H__
#include <stdint.h>
#include "bme68x.h"
#include "esp_bme_errors.h"
#include "esp_bme_i2c.h"


// #define PRINT_SENSOR_DATA 
// #define BME68X_USE_FPU

#define BME_SAMPLE_MODE BME68X_FORCED_MODE      //Sample mode for BME sensor
#define BME_TEMP_SR     BME68X_OS_2X            //Oversample rate for Temperature
#define BME_PRES_SR     BME68X_OS_16X           //Oversample rate for Pressure
#define BME_HUM_SR      BME68X_OS_1X            //Oversample rate for Humidity
#define BME_FILTER      BME68X_FILTER_SIZE_15   //IIR filter profile
#define BME_ODR         BME68X_ODR_NONE         //Standby time
#define BME_HEATER_EN   BME68X_ENABLE_HEATER   //Enable/Disable Heater

#define DELAY_FACTOR 1        //units of sec



typedef struct bme_sensor_data {
    struct bme68x_data bme_results;
    float iaq;
}bme_sensor_data_t;




extern struct bme_sensor_data global_sensor_data;
extern bool valid_data;


void measureBME680Data(struct bme_sensor_data* bme_data);
void initializeBME680(void);
float calculate_iaq(float gas_resistance, float humidity);



#endif /* __ESP_BME680_H__ */