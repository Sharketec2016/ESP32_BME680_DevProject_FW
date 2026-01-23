/**
 * @file esp_bme_i2c.h
 * @brief I2C communication interface for BME680 sensor
 * 
 * @details This header defines the I2C (Inter-Integrated Circuit) communication interface
 * for the BME680 environmental sensor on the ESP32. It provides low-level read and write
 * functions that map to the BME68X sensor API, as well as I2C bus and device configuration
 * structures. The module uses the ESP32 I2C master driver with 400kHz clock frequency
 * and supports 7-bit addressing.
 * 
 * Hardware Configuration:
 * - I2C Port: I2C_NUM_0 (default)
 * - Clock frequency: 400 kHz (standard I2C speed)
 * - SCL pin: GPIO_NUM_22
 * - SDA pin: GPIO_NUM_21
 * - Address length: 7-bit
 * - Device address: 0x76
 * - Timeout: 1000 ms
 * 
 * Features:
 * - Master mode I2C communication
 * - Internal pull-up resistor enable
 * - Glitch filtering with 7-cycle ignore
 * - ACK check enabled
 * - Automatic error mapping to BME68X codes
 * 
 * @author Matthew Buchkowski
 * @date January 2026
 * @version 1.0
 */

#include "stdint.h"
#include "string.h"
#include "driver/i2c_master.h"
#include "driver/i2c.h"
#include "bme68x.h"


#define I2C_FREQ_HZ         400000     // 400kHz
#define I2C_PORT            I2C_NUM_0

#define DEVICE_ADDR         0x76       // example device address
#define I2C_MASTER_TIMEOUT_MS 1000

extern i2c_master_bus_config_t i2c_bus_config;
extern i2c_device_config_t i2c_dev_config;

extern i2c_master_bus_handle_t i2c_bus_handle;
extern i2c_master_dev_handle_t i2c_dev_handle;


int8_t bme68x_i2c_read(uint8_t, uint8_t*, uint32_t, void*);
int8_t bme68x_i2c_write(uint8_t, const uint8_t*, uint32_t, void*);
void initialize_i2c(void);
