#include "esp32_home_ap.h"

const char* server_tag = "ESP32 Home Server";


static esp_err_t hello_get_handler(httpd_req_t *req);
static esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err);



static const httpd_uri_t hello_world_uri = {
    .uri       = "/hello",
    .method    = HTTP_GET,
    .handler   = hello_get_handler,
    .user_ctx  = NULL
};




static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    printf("Event %ld\n", event_id);
}

void wifi_init_softap(void)
{
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT(); //Grab the default wifi config

    esp_wifi_init(&cfg);


    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL);

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
    if (strlen(ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config);
    esp_wifi_start();

    ESP_LOGI(server_tag, "wifi_init_softap finished.SSID:%s password:%s",
             ESP_WIFI_SSID, ESP_WIFI_PASS);
             

}


static esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
    return ESP_OK;
}

static esp_err_t hello_get_handler(httpd_req_t *req)
{
    const char* resp_str = (const char*)"Hello World!";
    httpd_resp_send(req, resp_str, strlen(resp_str));
    return ESP_OK;
}

httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    ESP_LOGI(server_tag, "Starting server on port: %d", config.server_port);

    if(httpd_start(&server, &config) == ESP_OK) 
    {
        ESP_LOGI(server_tag, "Registering URI handlers");
        httpd_register_uri_handler(server, &hello_world_uri);
        httpd_register_err_handler(server, HTTPD_404_NOT_FOUND, http_404_error_handler);
        return server;
    }
    ESP_LOGI(server_tag, "Error starting server!");
    return NULL;
}

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