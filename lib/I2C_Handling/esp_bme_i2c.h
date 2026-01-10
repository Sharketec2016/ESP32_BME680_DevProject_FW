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
