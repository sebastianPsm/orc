#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_log.h"
#include "nvs_flash.h"

#include <SPIbus.hpp>
#include "ble_stuff.h"
#include "display.h"
#include "imu.h"
#include "storage.h"

extern "C" void app_main(void) {
    esp_err_t ret;

    /* Initialize NVS. */
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    imu_init(); // initialize and use VSPI_HOST
    display_init(); // initialize HSPI_HOST SPI first
    storage_init(); // uses HSPI_HOST SPI --> don't change the order

    //ble_stuff_init();
    
    imu_start_task();

}
