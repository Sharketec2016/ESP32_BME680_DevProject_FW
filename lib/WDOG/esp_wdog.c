#include "esp_wdog.h"


esp_task_wdt_user_handle_t aliveTaskWdogHandle;
esp_task_wdt_user_handle_t sampleDataTaskWdogHandle;



void initWDOG(void)
{
    esp_task_wdt_config_t wdog_config = {
        .idle_core_mask = (1 << CONFIG_FREERTOS_NUMBER_OF_CORES) - 1,
        .timeout_ms = TWDT_timeout,
        .trigger_panic = false
    };
    #if !CONFIG_ESP_TASK_WDT_INIT
    ESP_ERROR_CHECK(esp_task_wdt_init(&wdog_config));
    #else
    ESP_ERROR_CHECK(esp_task_wdt_reconfigure(&wdog_config));
    #endif //CONFIG_ESP_TASK_WDT_INIT
}
