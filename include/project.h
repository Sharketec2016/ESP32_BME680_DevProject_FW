#ifndef PROJECT_H
#define PROJECT_H
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "stdio.h"
#include "string.h"
#include "esp_log.h"
#include "bme68x.h"
#include "driver/i2c.h"
#include "driver/i2c_master.h"

#define LED GPIO_NUM_2
#define DELAY 800
#define I2C_FREQ_HZ         400000     // 400kHz
#define I2C_PORT            I2C_NUM_0

#define DEVICE_ADDR         0x76       // example device address
#define I2C_MASTER_TIMEOUT_MS 1000


gpio_config_t led_config = {
    .intr_type = GPIO_INTR_DISABLE,
    .mode = GPIO_MODE_OUTPUT,
    .pin_bit_mask = (1U << LED),
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .pull_up_en = GPIO_PULLUP_DISABLE
};

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



#endif