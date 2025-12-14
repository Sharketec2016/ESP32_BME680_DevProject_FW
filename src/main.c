#include "project.h"


struct bme68x_dev bme;
struct bme68x_conf bme_conf;
struct bme68x_data bme_data;
static uint8_t sensor_addr;






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