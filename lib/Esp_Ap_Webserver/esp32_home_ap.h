/**
 * @file esp32_home_ap.h
 * @brief WiFi access point and web server interface
 * 
 * @details This header defines the WiFi Access Point (AP) configuration and web server
 * interface for remote monitoring and control. The module provides WiFi soft AP initialization
 * with WPA2-PSK security, an HTTP server with REST API endpoints for sensor data retrieval,
 * and an interactive dashboard for real-time environmental monitoring. Non-Volatile Storage
 * (NVS) initialization is also provided for persistent configuration storage.
 * 
 * Web server features:
 * - HTTP server on port 80
 * - RESTful JSON API for sensor data (/sensor_data endpoint)
 * - Interactive HTML dashboard with real-time updates (/index)
 * - Hello World test endpoint (/hello)
 * - 404 error handler
 * 
 * WiFi configuration:
 * - SSID: ESP32_BME680_SERVER
 * - Password: bme680sensor (WPA2-PSK)
 * - Max simultaneous connections: 2
 * - Channel: 1
 * 
 * @author Matthew Buchkowski
 * @date January 2026
 * @version 1.0
 */

#ifndef __ESP32_HOME_SERVER_H__
#define __ESP32_HOME_SERVER_H__

#include <stdio.h>
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "bme68x.h"
#include "esp_bme680.h"

#ifndef ESP_WIFI_SSID
#define ESP_WIFI_SSID "DEFAULT_SSID"
#endif

#ifndef ESP_WIFI_PASS
#define ESP_WIFI_PASS "DEFAULT_PASS"
#endif


#define ESP_WIFI_CHANNEL    1
#define MAX_STA_CONN        2

extern SemaphoreHandle_t sensor_data_mutex;



void wifi_init_softap(void);
httpd_handle_t start_webserver(void);
httpd_handle_t stop_webserver(httpd_handle_t server);
esp_err_t nvs_setup(void);





#endif
