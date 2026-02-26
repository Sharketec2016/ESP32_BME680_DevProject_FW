#ifndef __ESP_WDOG_H
#define __ESP_WDOG_H

#include "esp_task_wdt.h"
#include "esp_err.h"

#define TWDT_timeout  3000  //this is in ms

extern esp_task_wdt_user_handle_t aliveTaskWdogHandle;
extern esp_task_wdt_user_handle_t sampleDataTaskWdogHandle;

void initWDOG(void);


#endif