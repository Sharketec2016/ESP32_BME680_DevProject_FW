#ifndef __ESP_WDOG_H
#define __ESP_WDOG_H

#include "esp_system.h"
#include "esp_private/panic_internal.h"
#include "esp_task_wdt.h"
#include "esp_bme_errors.h"
#include "esp_err.h"



#define TWDT_timeout  3000  //this is in ms



void initWDOG(void);


#endif