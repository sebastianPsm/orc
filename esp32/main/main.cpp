#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_log.h"
#include "nvs_flash.h"
 
#include "ble_stuff.h"
#include "display.h"
#include "imu.h"

#define DISPLAY_ACTIVE (0)

extern "C" void app_main(void) {
    esp_err_t ret;

    /* Initialize NVS. */
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
    
#if DISPLAY_ACTIVE
    display_init();
#endif /* DISPLAY_ACTIVE */

    //ble_stuff_init();
    
    imu_init();

    imu_start_task();

}
