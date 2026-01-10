#include "esp_bme_i2c.h"




i2c_master_bus_config_t i2c_bus_config = {
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .scl_io_num = GPIO_NUM_22,
    .sda_io_num = GPIO_NUM_21,
    .i2c_port = I2C_PORT,
    .flags.enable_internal_pullup = true,
    .glitch_ignore_cnt = 7,
    .intr_priority = 0
};

i2c_device_config_t i2c_dev_config = {
    .dev_addr_length = I2C_ADDR_BIT_7,
    .device_address = DEVICE_ADDR,
    .scl_speed_hz = I2C_FREQ_HZ,
    .scl_wait_us = 0,
    .flags.disable_ack_check = false
};

i2c_master_bus_handle_t i2c_bus_handle;
i2c_master_dev_handle_t i2c_dev_handle;


/**
 * @brief I2C read function map to ESP32 platform
 * 
 * @param reg_addr 
 * @param reg_data 
 * @param len 
 * @param intf_ptr 
 * @return int8_t 
 */
int8_t bme68x_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
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

/**
 * @brief I2C write function map to ESP32 platform
 * 
 * @param reg_addr 
 * @param reg_data 
 * @param len 
 * @param intf_ptr 
 * @return int8_t 
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

void initialize_i2c(void)
{
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_config, &i2c_bus_handle));
    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_bus_handle, &i2c_dev_config, &i2c_dev_handle));
}