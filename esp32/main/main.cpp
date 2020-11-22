#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <esp_log.h>
#include <esp_sleep.h>
#include <nvs_flash.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "ble_stuff.h"
#include "display.h"
#include "storage.h"
#include "status.h"
#include "imu.h"
#include "analysis.h"
#include "battery.h"

#ifdef __cplusplus
extern "C" {
#endif
#include <mpu_idf_esp32.h>
#include <driver/include/mltypes.h>
#include <driver/eMPL/inv_mpu_dmp_motion_driver.h>
#include <mllite/ml_math_func.h>
#ifdef __cplusplus
}
#endif

#define tag "main"

void enter_sleep_mode();
void accel_cb(void *);

tStatus status = {
    .battery_voltage = 0,
    .name_owner = NULL,
    .has_weight = false,
    .weight_kg = 0,

    .spm = 55,

    .sensor_timestamp = 0,
    .rot_matrix = {0},
    .accel = {0},
    .gyro = {0},

    .analysis = NULL,
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
    .print_quat_and_accel = true,

    .counter_run = 0,
    .counter_log_bytes = 0
};

void enter_sleep_mode() {
    ESP_LOGE(tag, "Entering deep sleep mode");

    status.sleep_active = true;
    status.counter_run++;

    //if(ESP_OK == storage_mount(&status)) {
    //    storage_write_config(&status);
    //    storage_unmount(&status);
    //}

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
    ESP_ERROR_CHECK(ret);

    uxTaskGetStackHighWaterMark(xTaskGetCurrentTaskHandle());

    /*
     * Initialize ORC features
     */
    battery_init();
    display_init(); // initialize and use HSPI_HOST SPI first
    //storage_init(); // uses HSPI_HOST SPI --> don't change the order
    display_start_update_task(&status);
    status.analysis = analysis_init(status.print_quat_and_accel);
    imu_init(&status);
    if(!status.imu_is_initialized) {
        show_error_screen(&status);
        for(;;) vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    //ble_stuff_init();

    //heap_caps_print_heap_info(MALLOC_CAP_DEFAULT);

    /*
     * Read storage directly after boot
     */    
    //if(ESP_OK == storage_mount(&status)) {
    //    storage_read_config(&status);
    //    storage_unmount(&status);
    //}
    show_boot_screen();
    
    //storage_mount(&status);

    for(;;) {
        vTaskDelay(7000 / portTICK_PERIOD_MS);
        status.battery_voltage = getBatterySoc(status.battery_voltage);
        display_update();
    }
}


