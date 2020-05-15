#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <SPIbus.hpp>

#include "ble_stuff.h"
#include "display.h"
#include "imu.h"
#include "storage.h"
#include "status.h"

#define TAG "main"

void enter_sleep_mode();

tStatus status = {
    .name_owner = NULL,
    .has_weight = false,
    .weight_kg = 0,

    .spm = 55,

    .ble_active = false,
    .sleep_active = false,

    .logging_active = false,
    .sd_is_mounted = false,
    .log_file_suffix = 0,
    .new_log_file = true,

    .imu_is_initialized = false,
   
    .sleep_after_s = 15,
    .sleep_cb = enter_sleep_mode,

    .imu_sampler_rate_hz = 50,

    .counter_run = 0,
};

void enter_sleep_mode() {
    ESP_LOGE(TAG, "Entering deep sleep mode");
    status.sleep_active = true;
    status.counter_run++;

    if(ESP_OK == storage_mount(&status)) {
        storage_write_config(&status);
        storage_unmount(&status);
    }

    esp_deep_sleep_start();
}

extern "C" void app_main(void) {
    esp_err_t ret;

    /* Initialize NVS. */
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    /*
     * Initialize ORC features
     */
    imu_init(&status); // initialize and use VSPI_HOST
    display_init(); // initialize and use HSPI_HOST SPI first
    storage_init(); // uses HSPI_HOST SPI --> don't change the order
    display_start_update_task(&status);

    /*
     * Read storage directly after boot
     */    
    if(ESP_OK == storage_mount(&status)) {
        storage_read_config(&status);
        storage_unmount(&status);
    }

    display_update();
    
    //ble_stuff_init();
    storage_mount(&status);
    imu_start_task(&status);
    
    
}
