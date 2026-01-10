#include "project.h"


struct bme68x_dev bme;
struct bme68x_conf bme_conf;
struct bme68x_data bme_data;
struct bme68x_heatr_conf heatr_conf;

struct bme68x_data global_sensor_data; 

QueueHandle_t sensor_data_queue;
SemaphoreHandle_t sensor_data_mutex;




gpio_config_t led_config = {
    .intr_type = GPIO_INTR_DISABLE,
    .mode = GPIO_MODE_OUTPUT,
    .pin_bit_mask = (1U << LED),
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .pull_up_en = GPIO_PULLUP_DISABLE
};

/* Heater temperature in degree Celsius */
uint16_t temp_prof[10] = { 200, 240, 280, 320, 360, 360, 320, 280, 240, 200 };

/* Heating duration in milliseconds */
uint16_t dur_prof[10] = { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 };

/**
 * @brief Task to toggle an LED to indicate system activity
 * 
 * @param pvParameters 
 */
void aliveTask(void *pvParameters)
{
    static uint8_t toggle = 0;
    while(1)
    {
        // ESP_LOGI(tag, "LED State: %s", toggle == 0 ? "OFF" : "ON");
        gpio_set_level(LED, toggle);
        toggle ^= 1;
        vTaskDelay(BLINK_DELAY / portTICK_PERIOD_MS);
    }
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
 * @brief Print the default configuration of the BME680 sensor
 * 
 */
void printDefaultConfig(void)
{
    ESP_LOGI(tag, "Default OverSampling Humidity: %d", bme_conf.os_hum);
    ESP_LOGI(tag, "Default OverSampling Temperature: %d", bme_conf.os_temp);
    ESP_LOGI(tag, "Default OverSampling Pressure: %d", bme_conf.os_pres);
    ESP_LOGI(tag, "Default Filter: %d", bme_conf.filter);
}

/**
 * @brief Setup the project components, including mutex, GPIO, I2C bud and master device, and BME680 sensor
 * 
 */
void setup(void)
{
    sensor_data_mutex = xSemaphoreCreateMutex();
    ESP_LOGI(tag, "Setting up project");
    ESP_ERROR_CHECK(gpio_config(&led_config));
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_config, &i2c_bus_handle));
    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_bus_handle, &i2c_dev_config, &i2c_dev_handle));
    setupBmeI2C(&bme, BME68X_I2C_INTF); //We are using I2C for comms


    int8_t rslt = bme68x_init(&bme);
    bme68x_check_rslt("bme68x_init", rslt);
    
    
    rslt = bme68x_get_conf(&bme_conf, &bme);
    bme68x_check_rslt("bme68x_get_conf", rslt);

    wifi_init_softap();

    sensor_data_queue = xQueueCreate(10, sizeof(struct bme68x_data));    
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

/**
 * @brief Task to sample sensor data and update the global sensor data
 * 
 * @details This task will continously sample data from the BME sensor, and if available, update the global sensor data struct with the lastest. It uses a mutex to ensure safe access to the global data.
 * @param pvParameters 
 */
void sampleDataTask(void *pvParameters)
{
    (void)pvParameters;
    static int8_t rslt;
    static uint8_t n_fields = 1;
    static uint32_t sample_count;
    static uint8_t hdr_cnt = 0;
    static uint32_t del_period;
    while(1)
    {

        del_period = bme68x_get_meas_dur(BME_SAMPLE_MODE, &bme_conf, &bme) + (1000 * 1000); //delay period appears to be in units of us. Dividing this down will decrease the delay

        bme.delay_us( (del_period * DELAY_FACTOR) , bme.intf_ptr);
        
        rslt = bme68x_get_data(BME_SAMPLE_MODE, &bme_data, &n_fields, &bme);
        bme68x_check_rslt("bme68x_get_data", rslt);
        if(rslt == BME68X_OK)
        {
#ifdef BME68X_USE_FPU
            printf("%lu, %.2f, %.2f, %.2f, %.2f, 0x%x, %d, %d\n",
                   sample_count,
                   bme_data.temperature,
                   bme_data.pressure,
                   bme_data.humidity,
                   bme_data.gas_resistance,
                   bme_data.status,
                   bme_data.gas_index,
                   bme_data.meas_index);
#else
            printf("Sample: %lu | Temperature (C): %d | Pressure (Pa): %lu | Humidity (%%): %lu\n",
                   sample_count,
                   (bme_data.temperature / 100),
                   (long unsigned int)bme_data.pressure,
                   (long unsigned int)(bme_data.humidity / 1000)
            );
#endif
        printf("Sending new data to queue\n");
        if(xSemaphoreTake(sensor_data_mutex, pdMS_TO_TICKS(100)) == pdTRUE)
        {
            memcpy(&global_sensor_data, &bme_data, sizeof(struct bme68x_data));
            xSemaphoreGive(sensor_data_mutex);
            printf("Updated latest sensor data\n");
        }

        sample_count++;
        }
    }
}

/**
 * @brief Main application entry point
 * 
 */
void app_main() 
{
    nvs_setup();
    setup();
    configBME();
    httpd_handle_t server = start_webserver();
    xTaskCreate(aliveTask, "Alive LED Blink", 2048, NULL, tskIDLE_PRIORITY, NULL);
    xTaskCreate(sampleDataTask, "Data Acquisition Task", 2048, NULL, tskIDLE_PRIORITY, NULL);
    vTaskStartScheduler();
    while(1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}