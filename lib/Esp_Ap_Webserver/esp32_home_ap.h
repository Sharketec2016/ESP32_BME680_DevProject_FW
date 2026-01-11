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

#define ESP_WIFI_SSID       "ESP32_BME680_SERVER"
#define ESP_WIFI_PASS       "bme680sensor"
#define ESP_WIFI_CHANNEL    1
#define MAX_STA_CONN        2

extern SemaphoreHandle_t sensor_data_mutex;



void wifi_init_softap(void);
httpd_handle_t start_webserver(void);
httpd_handle_t stop_webserver(httpd_handle_t server);
esp_err_t nvs_setup(void);





#endif
