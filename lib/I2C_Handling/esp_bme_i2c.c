/**
 * @file esp_bme_i2c.c
 * @brief I2C communication implementation for BME680 sensor
 * 
 * @details This file implements the I2C master communication driver for the BME680 sensor
 * on the ESP32 platform. It provides read and write functions that interface with the
 * BME68X sensor API, handling low-level I2C transactions with proper error checking and
 * timeout management. The module also includes I2C bus and device configuration initialization.
 * 
 * Functions:
 * - bme68x_i2c_read(): Performs I2C register read via transmit-receive
 * - bme68x_i2c_write(): Performs I2C register write with address prepend
 * - initialize_i2c(): Sets up I2C bus and device handles
 * 
 * Error Handling:
 * - I2C errors mapped to BME68X_E_COM_FAIL
 * - Success returns BME68X_OK
 * - Timeout handling via ESP_ERROR_CHECK
 * 
 * @author Matthew Buchkowski
 * @date January 2026
 * @version 1.0
 */

#include "esp_bme_i2c.h"

const char* i2c_tag = "BME680 I2C";


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

    // if(!reg_data || !intf_ptr)
    // {
    //     ESP_LOGE(i2c_tag, "Error: null pointers were passed for either reg_data or intf_ptr for i2c_read");
    //     return BME68X_E_NULL_PTR;
    // }

    esp_err_t err = ESP_OK;

    for(uint8_t retries = 0x00; retries<I2C_COMMS_RETRIES; retries++)
    {
        err = i2c_master_transmit_receive(
            i2c_dev_handle,
            &reg_addr,
            1,
            reg_data,
            len,
            pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS)
        );
        if(err == ESP_OK)
        {
            if(retries > 0)
            {
                ESP_LOGW(i2c_tag, "Warning: I2C read took %d retries", retries);
            }
            return BME68X_OK;
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // small backoff
    }

    ESP_LOGE("BME680", "I2C read failed after %d retries", I2C_COMMS_RETRIES);
    return BME68X_E_COM_FAIL; 
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

    // if(!reg_data || !intf_ptr)
    // {
    //     ESP_LOGE(i2c_tag, "Error: null pointers were passed for either reg_data or intf_ptr for i2c_write");
    //     return BME68X_E_NULL_PTR;
    // }

    uint8_t tx_buf[len+1];
    tx_buf[0] = reg_addr;
    memcpy(&tx_buf[1], reg_data, len);

    esp_err_t err = ESP_OK;

    for(uint8_t retries = 0x00; retries < I2C_COMMS_RETRIES; retries++)
    {
        err = i2c_master_transmit(
            i2c_dev_handle,
            tx_buf, 
            len+1,
            pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS)
        );

        if(err == ESP_OK)
        {
            if(retries > 0)
            {
                ESP_LOGW(i2c_tag, "Warning: I2C write took %d retries", retries);
            }
            return BME68X_OK;
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // small backoff
    }

    ESP_LOGE("BME680", "I2C write failed after %d retries", I2C_COMMS_RETRIES);
    return BME68X_E_COM_FAIL; 
}

void initialize_i2c(void)
{
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_config, &i2c_bus_handle));
    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_bus_handle, &i2c_dev_config, &i2c_dev_handle));
}
