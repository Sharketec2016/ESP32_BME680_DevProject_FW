#include "project.h"

QueueHandle_t sensor_data_queue;
SemaphoreHandle_t sensor_data_mutex;


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
        toggle_led(&toggle);
        vTaskDelay(BLINK_DELAY / portTICK_PERIOD_MS);
    }
}




/**
 * @brief Setup the project components, including mutex, GPIO, I2C bud and master device, and BME680 sensor
 * 
 */
void setup(void)
{
    sensor_data_mutex = xSemaphoreCreateMutex();
    if(sensor_data_mutex == NULL)
    {
        ESP_LOGE(tag, "Failed to create sensor data mutex");
        return;
    }

    ESP_LOGI(tag, "Setting up project");
    ESP_LOGI(tag, "Initializing GPIO");
    setup_gpio();

    ESP_LOGI(tag, "Initializing I2C");
    initialize_i2c();

    ESP_LOGI(tag, "Configuring BME680 Sensor");
    configureBme680Sensor();

    ESP_LOGI(tag, "Setting up WiFi Access Point");
    // wifi_init_softap();

    ESP_LOGI(tag, "Setup complete");
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
    while(1)
    {
        if(xSemaphoreTake(sensor_data_mutex, pdMS_TO_TICKS(100)) == pdTRUE)
        {
            printf("Sampling new sensor data...\n");
            measureBME680Data(&global_sensor_data);
            printf("Updated latest sensor data\n");
            xSemaphoreGive(sensor_data_mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/**
 * @brief Main application entry point
 * 
 */
void app_main() 
{
    // nvs_setup();
    setup();

    // httpd_handle_t server = start_webserver();
    xTaskCreate(aliveTask, "Alive LED Blink", 2048, NULL, tskIDLE_PRIORITY, NULL);
    xTaskCreate(sampleDataTask, "Data Acquisition Task", 2048, NULL, tskIDLE_PRIORITY, NULL);
    ESP_LOGI(tag, "Application complete");
    // while(1)
    // {
    //     vTaskDelay(1000 / portTICK_PERIOD_MS);
    // }
}