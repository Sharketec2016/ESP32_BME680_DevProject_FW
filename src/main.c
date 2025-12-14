#include "project.h"

static const char* tag = "BME680 Project";
struct bme68x_dev bme;
struct bme68x_conf bme_conf;
struct bme68x_data bme_data;
static uint8_t sensor_addr;



void bme68x_check_rslt(const char api_name[], int8_t rslt)
{
    switch (rslt)
    {
        case BME68X_OK:

            /* Do nothing */
            ESP_LOGI(tag, "API name [%s] Success", api_name);
            break;
        case BME68X_E_NULL_PTR:
            printf("API name [%s]  Error [%d] : Null pointer\r\n", api_name, rslt);
            break;
        case BME68X_E_COM_FAIL:
            printf("API name [%s]  Error [%d] : Communication failure\r\n", api_name, rslt);
            break;
        case BME68X_E_INVALID_LENGTH:
            printf("API name [%s]  Error [%d] : Incorrect length parameter\r\n", api_name, rslt);
            break;
        case BME68X_E_DEV_NOT_FOUND:
            printf("API name [%s]  Error [%d] : Device not found\r\n", api_name, rslt);
            break;
        case BME68X_E_SELF_TEST:
            printf("API name [%s]  Error [%d] : Self test error\r\n", api_name, rslt);
            break;
        case BME68X_W_NO_NEW_DATA:
            printf("API name [%s]  Warning [%d] : No new data found\r\n", api_name, rslt);
            break;
        default:
            printf("API name [%s]  Error [%d] : Unknown error code\r\n", api_name, rslt);
            break;
    }
}

/*!
 * I2C read function map to ESP32 platform
 */
int8_t bme68x_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    uint8_t dev_addr = *(uint8_t *)intf_ptr;
    esp_err_t err = i2c_master_transmit_receive(
        i2c_dev_handle,
        &reg_addr,
        1,
        reg_data,
        len,
        pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS)
    );
    return (err == ESP_OK) ? BME68X_OK : BME68X_E_COM_FAIL;

    
}

/*!
 * I2C write function map to ESP32 platform
 */
int8_t bme68x_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    uint8_t tx_buf[len+1];
    tx_buf[0] = reg_addr;
    memcpy(&tx_buf[1], reg_data, len);
    esp_err_t err = i2c_master_transmit(
        i2c_dev_handle,
        tx_buf, 
        len+1,
        pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS)
    );
    return (err == ESP_OK) ? BME68X_OK : BME68X_E_COM_FAIL;
}

void user_delay_ms(uint32_t period, void *intf_ptr)
{
    (void)intf_ptr;
    vTaskDelay(pdMS_TO_TICKS(period));
}

void setupBmeI2C(struct bme68x_dev* bme, uint8_t intf)
{
    bme->intf = BME68X_I2C_INTF;
    bme->read = bme68x_i2c_read;
    bme->write = bme68x_i2c_write;
    bme->delay_us = user_delay_ms;
    bme->amb_temp = 25;
}


void setup(void)
{
    ESP_LOGI(tag, "Setting up project");
    ESP_ERROR_CHECK(gpio_config(&led_config));
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_config, &i2c_bus_handle));
    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_bus_handle, &i2c_dev_config, &i2c_dev_handle));
    setupBmeI2C(&bme, BME68X_I2C_INTF); //We are using I2C for comms


    int8_t rslt = bme68x_init(&bme);
    bme68x_check_rslt("bme68x_init", rslt);
    
    
    
    rslt = bme68x_get_conf(&bme_conf, &bme);
    bme68x_check_rslt("bme68x_get_conf", rslt);



}

void app_main() 
{
    setup();


    uint8_t toggle = 0;
    while(1)
    {
        ESP_LOGI(tag, "LED State: %s", toggle == 0 ? "OFF" : "ON");
        gpio_set_level(LED, toggle);
        toggle ^= 1;
        vTaskDelay(DELAY / portTICK_PERIOD_MS);
    }
}