#include "esp_timer_handling.h"


esp_timer_handle_t periodic_timer_handle;


const esp_timer_create_args_t timer_args = {
    .callback = timer_callback,
    .name = "test timer"
};


void timer_callback(void* arg)
{
    static uint32_t callback_count = 0x00;
    // ESP_LOGI("Timer", "\rTimer callback was called %d times\n", callback_count++);
}


void timer_setup(void)
{
    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &periodic_timer_handle));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer_handle, TIMER_PERIOD));
}