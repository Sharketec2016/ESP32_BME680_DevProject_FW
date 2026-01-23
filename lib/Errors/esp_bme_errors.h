/**
 * @file esp_bme_errors.h
 * @brief Error handling and diagnostic utilities for BME680 sensor
 * 
 * @details This header provides error checking and handling functionality for the BME680
 * sensor driver. It defines the interface for error checking of BME68X API return codes
 * and provides diagnostic logging. The module handles various error conditions including
 * communication failures, null pointer exceptions, device detection issues, and self-test
 * failures. Critical errors trigger an infinite loop (spin) to safely halt execution.
 * 
 * Supported error codes:
 * - BME68X_OK: Successful operation
 * - BME68X_E_NULL_PTR: Null pointer error
 * - BME68X_E_COM_FAIL: Communication failure with sensor
 * - BME68X_E_INVALID_LENGTH: Incorrect parameter length
 * - BME68X_E_DEV_NOT_FOUND: Device not found on bus
 * - BME68X_E_SELF_TEST: Self-test failure
 * - BME68X_W_NO_NEW_DATA: No new data available (warning)
 * 
 * @author Matthew Buchkowski
 * @date January 2026
 * @version 1.0
 */

#include "bme68x.h"
#include "string.h"
#include "esp_log.h"
#include "stdio.h"

extern const char* tag;


void bme68x_check_rslt(const char api_name[], int8_t rslt);
void spin(void);