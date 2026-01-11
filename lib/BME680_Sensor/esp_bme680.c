#include "esp_bme680.h"

struct bme68x_dev bme;
struct bme68x_conf bme_conf;
struct bme68x_heatr_conf heatr_conf;
struct bme68x_data global_sensor_data; 

SemaphoreHandle_t sensor_data_mutex;
/* Heater temperature in degree Celsius */
uint16_t temp_prof[10] = { 200, 240, 280, 320, 360, 360, 320, 280, 240, 200 };

/* Heating duration in milliseconds */
uint16_t dur_prof[10] = { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 };


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

/**
 * @brief Configure the BME680 Sensor. Includes bme structure and setting up heater parameters
 * 
 */
void configureBme680Sensor(void)
{
    setupBmeI2C(&bme, BME68X_I2C_INTF); //We are using I2C for comms


    int8_t rslt = bme68x_init(&bme);
    bme68x_check_rslt("bme68x_init", rslt);
    
    
    rslt = bme68x_get_conf(&bme_conf, &bme);
    bme68x_check_rslt("bme68x_get_conf", rslt);

    configBME();
}


/** @brief user defined function for a us delay. 
 * @param period period of time to delay in us
 * @param int_ptr void pointer to extra information
 * 
 * @return void
*/
void user_delay_us(uint32_t period, void *intf_ptr)
{
    (void)intf_ptr;
    vTaskDelay(pdMS_TO_TICKS(period / 1000));
}

/**
 * @brief Setup I2C communication with the BME680 sensor
 * 
 * @param bme 
 * @param intf 
 */
void setupBmeI2C(struct bme68x_dev* bme, uint8_t intf)
{
    bme->intf = BME68X_I2C_INTF;
    bme->read = bme68x_i2c_read;
    bme->write = bme68x_i2c_write;
    bme->delay_us = user_delay_us;
    bme->amb_temp = 25;
}

/**
 * @brief Configure the heater settings for the BME680 sensor
 * 
 */
void configHeater(void)
{
    int8_t rslt;
    heatr_conf.enable = BME68X_ENABLE;
    heatr_conf.heatr_temp_prof = temp_prof;
    heatr_conf.heatr_dur_prof = dur_prof;
    heatr_conf.profile_len = 10;
    rslt = bme68x_set_heatr_conf(BME_SAMPLE_MODE, &heatr_conf, &bme);
    bme68x_check_rslt("bme68x_set_heatr_conf", rslt);
}

/**
 * @brief Configure the BME680 Sensor with desired settings
 * 
 */
void configBME(void)
{
    int8_t rslt;
    bme_conf.os_temp = BME68X_OS_2X;
    bme_conf.os_pres = BME68X_OS_16X;
    bme_conf.os_hum = BME68X_OS_1X;
    bme_conf.filter = BME68X_FILTER_OFF;
    bme_conf.odr = BME68X_ODR_NONE;

    rslt = bme68x_set_conf(&bme_conf, &bme);
    bme68x_check_rslt("bme68x_set_conf", rslt);

    configHeater();

    rslt = bme68x_set_op_mode(BME_SAMPLE_MODE, &bme);
    bme68x_check_rslt("bme68x_set_op_mode", rslt);
}

void measureBME680Data(struct bme68x_data* bme_data)
{
    int8_t rslt;
    uint8_t n_fields = 1;

    uint32_t del_period = bme68x_get_meas_dur(BME_SAMPLE_MODE, &bme_conf, &bme) + (1000 * 1000); //delay period appears to be in units of us. Dividing this down will decrease the delay

    bme.delay_us( (del_period * DELAY_FACTOR) , bme.intf_ptr);
    
    rslt = bme68x_get_data(BME_SAMPLE_MODE, bme_data, &n_fields, &bme);
    bme68x_check_rslt("bme68x_get_data", rslt);
    
    #ifdef PRINT_SENSOR_DATA
    if(rslt == BME68X_OK)
    {



    #ifdef BME68X_USE_FPU
    printf("%lu, %.2f, %.2f, %.2f, %.2f, 0x%x, %d, %d\n",
        sample_count,
        bme_data->temperature,
        bme_data->pressure,
        bme_data->humidity,
        bme_data->gas_resistance,
        bme_data->status,
        bme_data->gas_index,
        bme_data->meas_index);
    #else
    printf("Temperature (C): %d | Pressure (Pa): %lu | Humidity (%%): %lu\n",
        (bme_data->temperature / 100),
        (long unsigned int)(bme_data->pressure),
        (long unsigned int)(bme_data->humidity / 1000)
    );
    #endif
    #endif
    }
}



