/**
 * @file esp_bme680.c
 * @brief BME680 environmental sensor implementation and data acquisition
 * 
 * @details This file implements the complete BME680 sensor driver for the ESP32 platform.
 * It handles sensor initialization, configuration of measurement modes, heater settings,
 * oversample rates, and filtering. The module provides data acquisition with thread-safe
 * access via FreeRTOS semaphore. It also implements the air quality index (IAQ) calculation
 * algorithm that combines humidity and gas resistance readings into a composite score.
 * 
 * Key features:
 * - Sensor initialization and configuration management
 * - Forced mode operation for periodic data sampling
 * - Temperature, pressure, humidity, and gas resistance measurement
 * - Gas sensor heater control and timing management
 * - IAQ calculation based on gas resistance and humidity reference levels
 * - Mutex-protected global sensor data for FreeRTOS task safety
 * 
 * @author Matthew Buchkowski
 * @date January 2026
 * @version 1.0
 */

#include "esp_bme680.h"

struct bme68x_dev bme;
struct bme68x_conf bme_conf;
struct bme68x_heatr_conf heatr_conf;
struct bme_sensor_data global_sensor_data;

struct bme_sensor sensor;

bool valid_data = false;

SemaphoreHandle_t sensor_data_mutex;

#if BME_SAMPLE_MODE == BME68X_SEQUENTIAL_MODE
/* Heater temperature in degree Celsius */
uint16_t temp_prof[10] = { 200, 240, 280, 320, 360, 360, 320, 280, 240, 200 };

/* Heating duration in milliseconds */
uint16_t dur_prof[10] = { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 };

#elif BME_SAMPLE_MODE == BME68X_FORCED_MODE
/* Heater temperature in degrees Celsius */
uint16_t temp_prof = 300;
/* Heating duration in milliseconds */
uint16_t dur_prof = 100;
#endif


// static void configHeater(void);
// static void configBME(void);
static void configHeater(struct bme68x_heatr_conf *heatr_conf, struct bme68x_dev *dev);
static void configBME(struct bme68x_conf* conf, struct bme68x_dev *dev);
static void configureBme680Sensor(void);
static void user_delay_us(uint32_t period, void *intf_ptr);
static void setupBmeI2C(struct bme68x_dev* setupBme);

float calculate_iaq(float gas_resistance, float humidity) {
    // Replace the 50000.0 with your actual baseline
    const float GAS_BASELINE = 806516.0f;
    const float HUM_REFERENCE = 40.0f; // Ideal humidity 40%

    float hum_score, gas_score;

    // 1. Calculate Humidity Score (Weight: 25%)
    // If hum is 40%, score is 25. If 0% or 100%, score is 0.
    if (humidity >= HUM_REFERENCE) {
        hum_score = (100.0f - humidity) / (100.0f - HUM_REFERENCE) * 25.0f;
    } else {
        hum_score = (humidity / HUM_REFERENCE) * 25.0f;
    }

    // 2. Calculate Gas Score (Weight: 75%)
    // If resistance is >= Baseline, score is 75.
    // As resistance drops (pollution increases), score drops.
    gas_score = (gas_resistance / GAS_BASELINE) * 75.0f;

    // 3. Combine and Clamp
    float total_score = hum_score + gas_score;

    if (total_score > 100.0f) total_score = 100.0f;
    if (total_score < 0.0f)   total_score = 0.0f;

    return total_score;
}

/**
 * @brief Configure the heater settings for the BME680 sensor
 *
 */
static void configHeater(struct bme68x_heatr_conf *heatr_conf, struct bme68x_dev *dev)
{
    int8_t rslt;
    heatr_conf->enable = BME_HEATER_EN;

#if BME_SAMPLE_MODE == BME68X_SEQUENTIAL_MODE
    heatr_conf->heatr_temp_prof = temp_prof;
    heatr_conf->heatr_dur_prof = dur_prof;
    heatr_conf->profile_len = 10;
#elif BME_SAMPLE_MODE == BME68X_FORCED_MODE
    heatr_conf->heatr_temp = temp_prof;
    heatr_conf->heatr_dur = dur_prof;
    heatr_conf->profile_len = 0x00;
#endif




    rslt = bme68x_set_heatr_conf(BME_SAMPLE_MODE, heatr_conf, dev);
    bme68x_check_rslt("bme68x_set_heatr_conf", rslt);
}

/**
 * @brief Configure the BME680 Sensor with desired settings
 *
 */
static void configBME(struct bme68x_conf* conf, struct bme68x_dev *dev)
{
    int8_t rslt;
    conf->os_temp    = BME_TEMP_SR;
    conf->os_pres    = BME_PRES_SR;
    conf->os_hum     = BME_HUM_SR;
    conf->filter     = BME_FILTER;
    conf->odr        = BME_ODR;

    rslt = bme68x_set_conf(conf, dev);
    bme68x_check_rslt("bme68x_set_conf", rslt);



    configHeater(&sensor.heatr, &sensor.dev);

    rslt = bme68x_set_op_mode(BME_SAMPLE_MODE, dev);
    bme68x_check_rslt("bme68x_set_op_mode", rslt);

    memcpy(&bme, &sensor.dev, sizeof(sensor.dev));
    memcpy(&bme_conf, &sensor.conf, sizeof(sensor.conf));
    memcpy(&heatr_conf, &sensor.heatr, sizeof(sensor.heatr));
}


/**
 * @brief Configure the BME680 Sensor. Includes bme structure and setting up heater parameters
 * 
 */
static void configureBme680Sensor(void)
{
    
    setupBmeI2C(&sensor.dev); //We are using I2C for comms


    int8_t rslt = bme68x_init(&sensor.dev);
    bme68x_check_rslt("bme68x_init", rslt);
    
    
    rslt = bme68x_get_conf(&sensor.conf, &sensor.dev);
    bme68x_check_rslt("bme68x_get_conf", rslt);

    configBME(&sensor.conf, &sensor.dev);
}


/** @brief user defined function for a us delay. 
 * @param period period of time to delay in us
 * @param int_ptr void pointer to extra information
 * 
 * @return void
*/
static void user_delay_us(uint32_t period, void *intf_ptr)
{
    (void)intf_ptr;
    uint32_t ticks = pdMS_TO_TICKS(period / 1000);

    if ((ticks == 0) && (period > 0)) ticks = 1;
    vTaskDelay(ticks);
}

/**
 * @brief Setup I2C communication with the BME680 sensor
 * 
 * @param setupBme
 * @param intf
 */
static void setupBmeI2C(struct bme68x_dev* setupBme)
{
    setupBme->intf = BME68X_I2C_INTF;
    setupBme->read = bme68x_i2c_read;
    setupBme->write = bme68x_i2c_write;
    setupBme->delay_us = user_delay_us;
    setupBme->amb_temp = 25;
}


/**
 * @brief Query data from the BME sensor. 
 * @details Function handles the data acquisiton differently depending on whether or not we are in forced mode.
 * Data acquistion is started by setting the op mode, if in forced. This is not necessary in rolling sampling mode
 * as we data is continously polled. We then acquire the appropriate delay until we can query the next data point. 
 * After the delay we confidently gather data from the sensor and pass into a helper function for calculating 
 * the values in a easy to interpret format. 
 * 
 * @param bme_data 
 */
void measureBME680Data(struct bme_sensor_data* bme_data)
{
    int8_t rslt;
    uint8_t n_fields = 1;
    valid_data = false;
#if BME_SAMPLE_MODE == BME68X_FORCED_MODE
    n_fields = 0;
    rslt = bme68x_set_op_mode(BME_SAMPLE_MODE, &bme);
    bme68x_check_rslt("bme68x_set_op_mode", rslt);
#endif


    uint32_t del_period = bme68x_get_meas_dur(BME_SAMPLE_MODE, &bme_conf, &bme);

    bme.delay_us( (del_period * DELAY_FACTOR) , bme.intf_ptr);
    
    rslt = bme68x_get_data(BME_SAMPLE_MODE, &bme_data->bme_results, &n_fields, &bme);
    bme68x_check_rslt("bme68x_get_data", rslt);

    if (rslt == BME68X_OK && n_fields > 0) {
        valid_data = true;
        bme_data->iaq = calculate_iaq(bme_data->bme_results.gas_resistance, bme_data->bme_results.humidity / 1000.0);

    }

#ifdef PRINT_SENSOR_DATA
    if(rslt == BME68X_OK) {
#ifdef BME68X_USE_FPU
        printf("%lu, %.2f, %.2f, %.2f, %.2f, 0x%x, %d, %d\n",
            sample_count,
            bme_data->bme_results.temperature,
            bme_data->bme_results.pressure,
            bme_data->bme_results.humidity,
            bme_data->bme_results.gas_resistance,
            bme_data->bme_results.status,
            bme_data->bme_results.gas_index,
            bme_data->bme_resultsmeas_index);
#else
        printf("Temperature (C): %d | Pressure (Pa): %lu | Humidity (%%): %lu\n",
            (bme_data->temperature / 100),
            (long unsigned int)(bme_data->bme_results.pressure),
            (long unsigned int)(bme_data->bme_results.humidity / 1000)
        );
#endif
    }
#endif

}


/**
 * @brief Initialize the BME680 Sensor, including creating the sensor data mutex
 *
 */
void initializeBME680(void)
{
    ESP_LOGI(tag, "Creating sensor data mutex");
    sensor_data_mutex = xSemaphoreCreateMutex();
    if(sensor_data_mutex == NULL)
    {
        ESP_LOGE(tag, "Failed to create sensor data mutex");
        return;
    }

    configureBme680Sensor();
}



