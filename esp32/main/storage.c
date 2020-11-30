#include "storage.h"

#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <esp_log.h>
#include <driver/gpio.h>
#include <esp_vfs_fat.h>
#include <driver/sdspi_host.h>
#include <sdmmc_cmd.h>
#include <cJSON.h>


#define MOUNT_POINT "/sdcard"

// Pin definition
#define PIN_NUM_MOSI GPIO_NUM_23
#define PIN_NUM_MISO GPIO_NUM_19
#define PIN_NUM_CLK GPIO_NUM_18
#define PIN_NUM_CS GPIO_NUM_5

#define SPI_DMA_CHAN 1

#define TAG "storage"

uint64_t last_log_entry;
FILE * f_log = NULL;

sdmmc_card_t * card = NULL;
sdmmc_host_t host = SDSPI_HOST_DEFAULT();
sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
esp_vfs_fat_sdmmc_mount_config_t mount_config = {
    .format_if_mount_failed = false,
    .max_files = 5,
    .allocation_unit_size = 16 * 1024
};

esp_err_t storage_init() {
    esp_err_t ret;

    ESP_LOGI(TAG, "Initializing SD card");
    
    host.slot = VSPI_HOST;
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = host.slot;

    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    ret = spi_bus_initialize(host.slot, &bus_cfg, SPI_DMA_CHAN);
    switch (ret) {
    case ESP_ERR_INVALID_ARG:
        ESP_LOGE("EPDIF", "INVALID ARG");
        break;
    case ESP_ERR_INVALID_STATE:
        ESP_LOGE("EPDIF", "INVALID STATE");
        break;
    case ESP_ERR_NO_MEM:
        ESP_LOGE("EPDIF", "INVALID NO MEMORY");
        break;
    case ESP_OK:
        ESP_LOGE("EPDIF", "All OK");
    }
    assert(ret == ESP_OK);
    
    return ESP_OK;
}
esp_err_t storage_mount(tStatus * status_out) {
    ESP_LOGI(TAG, "Mount");
    if(status_out->sd_is_mounted == true) return ESP_OK;

    esp_err_t ret = esp_vfs_fat_sdspi_mount(MOUNT_POINT, &host, &slot_config, &mount_config, &card);
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                "If you want the card to be formatted, set format_if_mount_failed = true.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        return ret;
    }

    sdmmc_card_print_info(stdout, card);
    status_out->sd_is_mounted = true;
    ESP_LOGI(TAG, "Mount successful");

    /*
     * Find free log file
     */
    char buf[200];
    unsigned idx;
    for(idx = 0; idx < 10000; idx++) {
        sprintf(buf, MOUNT_POINT"/log%05d.csv", idx);
        if(access(buf, F_OK) != -1) continue;
        break;
    }
    status_out->log_file_suffix = idx;

    return ESP_OK;
}
esp_err_t storage_unmount(tStatus * status_out) {
    if(status_out->sd_is_mounted == false) return ESP_OK;
    if(f_log) {
        int ret_fclose = fclose(f_log);
        if(ret_fclose != 0) {
            ESP_LOGE(TAG, "Error closing log: %d (fflush)", ferror(f_log));
        }
        f_log = NULL;
    }

    esp_err_t ret = esp_vfs_fat_sdcard_unmount(MOUNT_POINT, card);
    if(ret != ESP_OK) {
        if (ret == ESP_ERR_INVALID_ARG) {
            ESP_LOGE(TAG, "Failed to unmount filesystem. "
                "card argument is unregistered");
        } else if (ret == ESP_ERR_INVALID_STATE) {
            ESP_LOGE(TAG, "Failed to unmount filesystem. "
                "mount hasn’t been called");
        }
    }
    status_out->sd_is_mounted = false;
    if(ret == ESP_OK) ESP_LOGI(TAG, "Unmount successful");
    return ret;
}
esp_err_t storage_read_config(tStatus * status_out) {
    if(status_out == NULL) return ESP_ERR_INVALID_ARG;

    /*
     * Reset out argument
     */
    status_out->name_owner = NULL;
    status_out->logging_active = false;

    FILE * f = fopen(MOUNT_POINT"/config.txt", "rb");
    if(f == NULL) {
        ESP_LOGE(TAG, "Was not possible to read 'config.txt' (sd_is_mounted: %d)", status_out->sd_is_mounted);
        return ESP_FAIL;
    }

    long len = 0;
    char * data = NULL;

    fseek(f, 0, SEEK_END);
    len = ftell(f);
    fseek(f, 0, SEEK_SET);

    data = (char*) malloc(len + 1);
    fread(data, 1, len, f);
    data[len] = '\0';

    cJSON *json = cJSON_Parse(data);
    if(cJSON_HasObjectItem(json, "name_owner")){
            status_out->name_owner = strdup(cJSON_GetStringValue(cJSON_GetObjectItem(json, "name_owner")));
    }
    if(cJSON_HasObjectItem(json, "ble_active")){
        status_out->ble_active = cJSON_GetObjectItem(json, "ble_active")->valueint == 0 ? 0 : 1;
    }
    if(cJSON_HasObjectItem(json, "logging_active")){
        status_out->logging_active = cJSON_GetObjectItem(json, "logging_active")->valueint == 0 ? 0 : 1;
    }
    if(cJSON_HasObjectItem(json, "imu_sample_rate_hz")){
        status_out->imu_sampler_rate_hz = cJSON_GetObjectItem(json, "imu_sample_rate_hz")->valueint;
    }

    cJSON_Delete(json);
    ESP_LOGI(TAG, "Read: %s\n", data);
    free(data);
    fclose(f);

    return ESP_OK;
}

esp_err_t storage_write_config(tStatus * status) {
    if(status == NULL) return ESP_ERR_INVALID_ARG;

    FILE * f = fopen(MOUNT_POINT"/config.txt", "wb");
    if(f == NULL) {
        ESP_LOGE(TAG, "Was not possible to write 'config.txt' (sd_is_mounted: %d)", status->sd_is_mounted);
        return ESP_FAIL;
    }

    cJSON * json = cJSON_CreateObject();
    if(status->name_owner) cJSON_AddStringToObject(json, "name_owner", status->name_owner);
    cJSON_AddBoolToObject(json, "ble_active", status->ble_active==0?false:true);
    cJSON_AddBoolToObject(json, "logging_active", status->logging_active==0?false:true);
    cJSON_AddNumberToObject(json, "imu_sample_rate_hz", status->imu_sampler_rate_hz);
       
    char * out = cJSON_Print(json);
    ESP_LOGI(TAG, "Write: %s", out);
    fprintf(f, out);
    free(out); out = NULL;

    cJSON_Delete(json);

    fclose(f);
    return ESP_OK;
}

esp_err_t storage_write_log(tStatus * status, long accel_x, long accel_y, long accel_z, long yaw, long pitch, long roll, float battery, float last_motion) {
    char buf[100];
    long old_pos;

    if(status == NULL) return ESP_ERR_INVALID_ARG;
    if(!status->logging_active) return ESP_OK;
    if(status->sd_is_mounted == false) return ESP_OK;

    /*
     * If no log file is already created ...
     */
    if(f_log == NULL) {
        sprintf(buf, MOUNT_POINT"/log%05d.csv", status->log_file_suffix);
        f_log = fopen(buf, "w");
        if(f_log == NULL) {
            ESP_LOGE(TAG, "Was not possible to write '%s' (sd_is_mounted: %d)", buf, status->sd_is_mounted);
            return ESP_FAIL;
        }
        old_pos = ftell(f_log);
        fprintf(f_log, "time_delta[ms],accel_x[G],accel_y[G],accel_z[G],yaw[deg],pitch[deg],roll[deg],battery[V],last_motion[s]\r\n");
        last_log_entry = esp_timer_get_time();
        status->counter_log_bytes += ftell(f_log) - old_pos;
    }
    
    /*
     * Write log entry
     */
    uint64_t t = esp_timer_get_time();
    old_pos = ftell(f_log);
    int ret_fprintf = fprintf(f_log, "%.2f, %ld, %ld, %ld, %ld, %ld, %ld, %+.2f, %.2f\r\n", (t - last_log_entry)/1e3, accel_x, accel_y, accel_z, yaw, pitch, roll, battery, last_motion);
    if(ret_fprintf < 0) {
        ESP_LOGE(TAG, "Error writing log: %d (fprint)", ferror(f_log));
        return ESP_FAIL;
    }
    int ret_fflush = fflush(f_log);
    if(ret_fflush != 0) {
        ESP_LOGE(TAG, "Error writing log: %d (fflush)", ferror(f_log));
        return ESP_FAIL;
    }
    last_log_entry = t;
    status->counter_log_bytes += ftell(f_log) - old_pos;
    
    return ESP_OK;
}