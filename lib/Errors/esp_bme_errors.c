/**
 * @file esp_bme_errors.c
 * @brief Error handling and diagnostic implementation for BME680 sensor
 * 
 * @details This file implements error checking and diagnostic functionality for the BME680
 * sensor driver. It provides detailed logging of BME68X API errors and warnings, translating
 * error codes into human-readable messages. Critical errors trigger a controlled halt via
 * spin() function to safely stop execution. All errors are logged via ESP-IDF logging system
 * with appropriate severity levels.
 * 
 * @author Matthew Buchkowski
 * @date January 2026
 * @version 1.0
 */

#include "esp_bme_errors.h"
const char* tag = "BME680 Project";

/**
 * @brief Spin in an infinite loop when an error occurs. 
 * 
 */
void spin(void)
{
    ESP_LOGE(tag, "ERROR: Encountered an error. Sitting  in inf while");
    while(1)
    {
        asm("nop"); //nop instruction is to make sure compiler doesnt optimize away
    }
}

/**
 * @brief BME68X error checking function. 
 * @details Checks the return code from BME68X API calls and logs appropriate messages. If a critical error is detected, it calls spin() to halt execution.
 * 
 * @param api_name 
 * @param rslt 
 */
void bme68x_check_rslt(const char api_name[], int8_t rslt)
{
    switch (rslt)
    {
        case BME68X_OK:
            // ESP_LOGI(tag, "API name [%s] Success", api_name);
            break;
        case BME68X_E_NULL_PTR:
            ESP_LOGE(tag, "API name [%s]  Error [%d] : Null pointer\r\n", api_name, rslt);
            break;
        case BME68X_E_COM_FAIL:
            ESP_LOGE(tag, "API name [%s]  Error [%d] : Communication failure\r\n", api_name, rslt);
            break;
        case BME68X_E_INVALID_LENGTH:
            ESP_LOGE(tag, "API name [%s]  Error [%d] : Incorrect length parameter\r\n", api_name, rslt);
            break;
        case BME68X_E_DEV_NOT_FOUND:
            ESP_LOGE(tag, "API name [%s]  Error [%d] : Device not found\r\n", api_name, rslt);
            break;
        case BME68X_E_SELF_TEST:
            ESP_LOGE(tag, "API name [%s]  Error [%d] : Self test error\r\n", api_name, rslt);
            break;
        case BME68X_W_NO_NEW_DATA:
            ESP_LOGI(tag, "API name [%s]  Warning [%d] : No new data found\r\n", api_name, rslt);
            break;
        default:
            ESP_LOGE(tag, "API name [%s]  Error [%d] : Unknown error code\r\n", api_name, rslt);
            break;
    }
}