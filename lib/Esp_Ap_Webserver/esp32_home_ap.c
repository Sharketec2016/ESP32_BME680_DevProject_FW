#include "esp32_home_ap.h"

const char* server_tag = "ESP32 Home Server";




static esp_err_t hello_get_handler(httpd_req_t *req);
static esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err);
static esp_err_t index_handler(httpd_req_t *req);
static esp_err_t sensor_data_handler(httpd_req_t *req);


/**
 * @brief httpd URI structure for the index page
 * 
 */
static const httpd_uri_t index_uri = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = index_handler,
    .user_ctx  = NULL
};

/**
 * @brief httpd URI structure for the sensor data endpoint
 * 
 */
static const httpd_uri_t sensor_data_uri = {
    .uri       = "/sensor_data",
    .method    = HTTP_GET,
    .handler   = sensor_data_handler,
    .user_ctx  = NULL
};

/**
 * @brief httpd URI structure for the hello world endpoint
 * 
 */
static const httpd_uri_t hello_world_uri = {
    .uri       = "/hello",
    .method    = HTTP_GET,
    .handler   = hello_get_handler,
    .user_ctx  = NULL
};

/**
 * @brief Wifi event handler for esp32 home AP
 * 
 * @param arg 
 * @param event_base 
 * @param event_id 
 * @param event_data 
 */
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    printf("Event %ld\n", event_id);
}


/**
 * @brief Wifi initialization for esp32 home AP
 * 
 */
void wifi_init_softap(void)
{
    ESP_ERROR_CHECK(esp_netif_init());                  //Initialize the underlying TCP/IP stack
    ESP_ERROR_CHECK(esp_event_loop_create_default());   //Create the default event loop
    esp_netif_create_default_wifi_ap();                 //Create default WIFI AP netif instance

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT(); //Grab the default wifi config

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));


    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = ESP_WIFI_SSID,
            .ssid_len = strlen(ESP_WIFI_SSID),
            .channel = ESP_WIFI_CHANNEL,
            .password = ESP_WIFI_PASS,
            .max_connection = MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,
            .pmf_cfg = {
                .required = true
            },
        },
    };
    if (strlen(ESP_WIFI_PASS) >= 8) {
        strcpy((char*)wifi_config.ap.password, ESP_WIFI_PASS);
        wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
        wifi_config.ap.pmf_cfg.required = true;
    } else {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(server_tag, "wifi_init_softap finished. SSID: %s  | Password: %s | Channel: %d", ESP_WIFI_SSID, ESP_WIFI_PASS, ESP_WIFI_CHANNEL);
             

}

/**
 * @brief Error handler for 404 not found
 * 
 * @param req 
 * @param err 
 * @return esp_err_t 
 */
static esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
    return ESP_OK;
}

/**
 * @brief Hello world handler for basic testing
 * 
 * @param req 
 * @return esp_err_t 
 */
static esp_err_t hello_get_handler(httpd_req_t *req)
{
    const char* resp_str = (const char*)"Hello World!";
    httpd_resp_send(req, resp_str, strlen(resp_str));
    return ESP_OK;
}

/**
 * @brief Index handler for serving the main dashboard page. Resolves to "/"
 * 
 * @param req 
 * @return esp_err_t 
 */
static esp_err_t index_handler(httpd_req_t *req)
{
    const char* html = 
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
            "<meta charset='UTF-8'>"
            "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
            "<title>BME680 Sensor Dashboard</title>"
            "<style>"
                "* { margin: 0; padding: 0; box-sizing: border-box; }"
                "body {"
                    "font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;"
                    "background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);"
                    "min-height: 100vh;"
                    "padding: 20px;"
                "}"
                ".container {"
                    "max-width: 800px;"
                    "margin: 0 auto;"
                "}"
                "h1 {"
                    "color: white;"
                    "text-align: center;"
                    "margin-bottom: 30px;"
                    "font-size: 2.5em;"
                    "text-shadow: 2px 2px 4px rgba(0,0,0,0.3);"
                "}"
                ".grid {"
                    "display: grid;"
                    "grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));"
                    "gap: 20px;"
                "}"
                ".sensor-box {"
                    "background: white;"
                    "padding: 30px;"
                    "border-radius: 15px;"
                    "box-shadow: 0 8px 16px rgba(0,0,0,0.2);"
                    "transition: transform 0.3s ease;"
                "}"
                ".sensor-box:hover {"
                    "transform: translateY(-5px);"
                "}"
                ".sensor-title {"
                    "font-size: 1.2em;"
                    "color: #555;"
                    "margin-bottom: 15px;"
                    "font-weight: 600;"
                "}"
                ".sensor-value {"
                    "font-size: 2.5em;"
                    "font-weight: bold;"
                    "background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);"
                    "background-clip: text;"
                    "-webkit-background-clip: text;"
                    "-webkit-text-fill-color: transparent;"
                "}"
                ".loading {"
                    "color: #999;"
                    "font-style: italic;"
                "}"
                ".footer {"
                    "text-align: center;"
                    "color: white;"
                    "margin-top: 40px;"
                    "font-size: 0.9em;"
                    "opacity: 0.8;"
                "}"
                ".status {"
                    "display: inline-block;"
                    "width: 10px;"
                    "height: 10px;"
                    "border-radius: 50%;"
                    "background: #4caf50;"
                    "margin-right: 8px;"
                    "animation: pulse 2s infinite;"
                "}"
                "@keyframes pulse {"
                    "0%, 100% { opacity: 1; }"
                    "50% { opacity: 0.5; }"
                "}"
            "</style>"
        "</head>"
        "<body>"
            "<div class='container'>"
                "<h1>🌡️ BME680 Sensor Dashboard</h1>"
                "<div class='grid'>"
                    "<div class='sensor-box'>"
                        "<div class='sensor-title'>🌡️ Temperature</div>"
                        "<div class='sensor-value loading' id='temp'>--</div>"
                    "</div>"
                    "<div class='sensor-box'>"
                        "<div class='sensor-title'>📊 Pressure</div>"
                        "<div class='sensor-value loading' id='pressure'>--</div>"
                    "</div>"
                    "<div class='sensor-box'>"
                        "<div class='sensor-title'>💧 Humidity</div>"
                        "<div class='sensor-value loading' id='humidity'>--</div>"
                    "</div>"
                    "<div class='sensor-box'>"
                        "<div class='sensor-title'>💨 Gas Resistance</div>"
                        "<div class='sensor-value loading' id='gas'>--</div>"
                    "</div>"
                "</div>"
                "<div class='footer'>"
                    "<span class='status'></span>Live updating every second | ESP32 BME680"
                "</div>"
            "</div>"
            "<script>"
                "function updateData() {"
                    "fetch('/sensor_data')"
                    ".then(response => response.json())"
                    ".then(data => {"
                        "document.getElementById('temp').innerHTML = data.temperature.toFixed(2) + ' <small>°C</small>';"
                        "document.getElementById('pressure').innerHTML = data.pressure.toFixed(2) + ' <small>hPa</small>';"
                        "document.getElementById('humidity').innerHTML = data.humidity.toFixed(2) + ' <small>%</small>';"
                        "document.getElementById('gas').innerHTML = data.gas.toFixed(0) + ' <small>Ω</small>';"
                        "document.querySelectorAll('.sensor-value').forEach(el => el.classList.remove('loading'));"
                    "})"
                    ".catch(err => {"
                        "console.error('Error fetching data:', err);"
                    "});"
                "}"
                "setInterval(updateData, 1000);"
                "updateData();"
            "</script>"
        "</body>"
        "</html>";

    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, html, strlen(html));
    return ESP_OK;
}

/**
 * @brief Sensor data handler to update the index handler, when queried, with the lastest sensor data
 * 
 * @param req 
 * @return esp_err_t 
 */
static esp_err_t sensor_data_handler(httpd_req_t *req)
{
    struct bme68x_data local_bme_data;
    // Try to receive data from the queue
    if(xSemaphoreTake(sensor_data_mutex, pdMS_TO_TICKS(100)) == pdTRUE)
    {
        memcpy(&local_bme_data, &global_sensor_data, sizeof(struct bme68x_data));
        xSemaphoreGive(sensor_data_mutex);
        char json_response[256];
        snprintf(json_response, sizeof(json_response), 
        "{"
            "\"temperature\": %.2f,"
            "\"pressure\": %.2f,"
            "\"humidity\": %.2f,"
            "\"gas\": %.2f"
        "}",
            (float)local_bme_data.temperature / 100.0, // Convert to °C
            (float)local_bme_data.pressure,
            (float)local_bme_data.humidity / 1000.0, // Convert to %
            (float)local_bme_data.gas_resistance
        );

        httpd_resp_set_type(req, "application/json");
        httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
        httpd_resp_send(req, json_response, strlen(json_response));

    }
    else
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Sensor data unavailable");
    }
    return ESP_OK;
}

/**
 * @brief Start the webserver for esp32 home AP
 * 
 * @return httpd_handle_t 
 */
httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;
    config.max_uri_handlers=8;

    ESP_LOGI(server_tag, "Starting server on port: %d", config.server_port);

    if(httpd_start(&server, &config) == ESP_OK) 
    {
        ESP_LOGI(server_tag, "Registering URI handlers");
        httpd_register_uri_handler(server, &hello_world_uri);
        httpd_register_err_handler(server, HTTPD_404_NOT_FOUND, http_404_error_handler);
        esp_err_t ret = httpd_register_uri_handler(server, &index_uri);
        if(ret == ESP_OK)
        {
            ESP_LOGI(server_tag, "Index URI handler registered");
        }
        else
        {
            ESP_LOGI(server_tag, "Failed to register Index URI handler");
        }

        ret = httpd_register_uri_handler(server, &sensor_data_uri);
        if(ret == ESP_OK)
        {
            ESP_LOGI(server_tag, "Sensor Data URI handler registered");
        }
        else
        {
            ESP_LOGI(server_tag, "Failed to register Sensor Data URI handler");
        }

        ESP_LOGI(server_tag, "Webserver started successfully");
        return server;
    }
    ESP_LOGI(server_tag, "Error starting server!");
    return NULL;
}

/**
 * @brief Stop the webserver for esp32 home AP
 * 
 * @param server 
 * @return httpd_handle_t 
 */
httpd_handle_t stop_webserver(httpd_handle_t server)
{
    ESP_LOGI(server_tag, "Stopping webserver");
    if(httpd_stop(server)  == ESP_OK)
    {
        ESP_LOGI(server_tag, "Webserver stopped");
        return NULL;
    }
    ESP_LOGI(server_tag, "Failed to stop webserver");
    return server;
}


/**
 * @brief Setup Non-Volatile Storage (NVS) for wifi credentials and other data
 * @details Initializes NVS and handles potential errors related to no free pages or new version found by erasing and re-initializing.
 * 
 * @return esp_err_t 
 */
esp_err_t nvs_setup(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
    return ret;
}





