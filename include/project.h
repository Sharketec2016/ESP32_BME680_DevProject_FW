/**
 * @file project.h
 * @brief Main project header file with central include aggregation
 * 
 * @details This header file serves as the central aggregation point for all project dependencies
 * and configurations. It includes FreeRTOS components for task management and synchronization,
 * as well as all custom module headers for GPIO handling, I2C communication, BME680 sensor
 * interface, and WiFi/webserver functionality. This file is included by the main application
 * to ensure all necessary components are available.
 * 
 * @author Matthew Buchkowski
 * @date January 2026
 * @version 1.0
 */

#ifndef PROJECT_H
#define PROJECT_H
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp32_home_ap.h"
#include "freertos/semphr.h"
#include "esp_gpio_handling.h"
#include "esp_bme680.h"




#endif