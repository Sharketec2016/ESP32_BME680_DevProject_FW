#include "bme68x.h"
#include "string.h"
#include "esp_log.h"
#include "stdio.h"

extern const char* tag;


void bme68x_check_rslt(const char api_name[], int8_t rslt);
void spin(void);