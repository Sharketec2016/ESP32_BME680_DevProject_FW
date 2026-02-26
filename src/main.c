/**
 * @file main.c
 * @brief Main application entry point and core task management for ESP32 BME680 sensor project
 * 
 * @details This file implements the primary application logic for an ESP32-based environmental
 * monitoring system using the BME680 sensor. It manages system initialization, task creation,
 * and coordination between multiple subsystems including GPIO control, I2C communication,
 * sensor data acquisition, WiFi connectivity, and a web server interface.
 * 
 * The application runs three main tasks:
 * - app_main(): Initializes all hardware and software components, then creates worker tasks
 * - aliveTask: Periodically toggles an LED to indicate system activity and proper scheduling
 * - sampleDataTask: Continuously acquires environmental data from the BME680 sensor
 * 
 * Key responsibilities:
 * - System setup and initialization (NVS, GPIO, I2C, BME680, WiFi)
 * - Sensor data acquisition with thread-safe access via mutex
 * - Web server hosting for remote data access
 * - Task scheduling and FreeRTOS integration
 * 
 * @author Matthew Buchkowski
 * @date January 2026
 * @version 1.0
 */

#include "project.h"

TaskHandle_t aliveTaskHandle;
TaskHandle_t sampleDataTaskHandle;



/**
 * @brief Task to toggle an LED to indicate system activity
 * 
 * @param pvParameters 
 */
void aliveTask(void *pvParameters)
{
    (void)pvParameters;
    esp_task_wdt_add(NULL);
    static uint8_t toggle = 0;
    while(1)
    {
        // esp_task_wdt_reset_user(aliveTaskWdogHandle);
        esp_task_wdt_reset();
        toggle_led(&toggle);
        vTaskDelay(BLINK_DELAY / portTICK_PERIOD_MS);
    }
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
    esp_task_wdt_add(NULL);
    while(1)
    {
        if(xSemaphoreTake(sensor_data_mutex, pdMS_TO_TICKS(100)) == pdTRUE)
        {
            measureBME680Data(&global_sensor_data);
            xSemaphoreGive(sensor_data_mutex);
            esp_task_wdt_reset();
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}



/**
 * @brief Setup the project components, including mutex, GPIO, I2C bud and master device, and BME680 sensor
 * 
 */
void setup(void)
{
    ESP_LOGI(tag, "Setting up project");

    ESP_LOGI(tag, "Setting up NVS");
    nvs_setup();

    ESP_LOGI(tag, "Initializing GPIO");
    setup_gpio();

    ESP_LOGI(tag, "Initializing I2C");
    initialize_i2c();

    ESP_LOGI(tag, "Initializing BME680 Sensor");
    initializeBME680();

    ESP_LOGI(tag, "Initializing WiFi");
    wifi_init_softap();

    ESP_LOGI(tag, "Initalizing WatchDog");
    initWDOG();

    ESP_LOGI(tag, "Setup complete");
}


/**
 * @brief Main application entry point
 * 
 */
void app_main() 
{
    setup();

    start_webserver();
    xTaskCreate(aliveTask, "Alive LED Blink", 2048, NULL, tskIDLE_PRIORITY+1, &aliveTaskHandle);
    xTaskCreate(sampleDataTask, "Data Acquisition Task", 4096, NULL, tskIDLE_PRIORITY+1, &sampleDataTaskHandle);
    

    // ESP_ERROR_CHECK(esp_task_wdt_add_user("Alive LED Blink", &aliveTaskWdogHandle));
    // ESP_ERROR_CHECK(esp_task_wdt_add_user("Data Acquistion Task", &sampleDataTaskWdogHandle));


    
    // esp_task_wdt_delete(NULL);

    ESP_LOGI(tag, "Application complete");
}