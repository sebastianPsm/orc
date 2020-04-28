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

tStatus status = {
    .name_owner = NULL,
    .spm = 0,
    .has_weight = false,
    .weight_kg = 0,
    .ble_active = false,
    .logging_active = false,    
};

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
    imu_init(); // initialize and use VSPI_HOST
    display_init(); // initialize HSPI_HOST SPI first
    storage_init(); // uses HSPI_HOST SPI --> don't change the order

    /*
     * Read storage directly after boot
     */    
    if(ESP_OK == storage_mount()) {
        storage_read_config(&status);
        storage_unmount();
    }
    
    display_update(&status);

    //ble_stuff_init();
    
    imu_start_task();


}
