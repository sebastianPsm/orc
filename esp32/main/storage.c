#include "storage.h"

#include <string.h>
#include <unistd.h>
#include <esp_log.h>
#include <driver/gpio.h>
#include <esp_vfs_fat.h>
#include <driver/sdspi_host.h>
#include <sdmmc_cmd.h>
#include <cJSON.h>


#define MOUNT_POINT "/sdcard"

// Pin definition
#define PIN_NUM_MOSI GPIO_NUM_27
#define PIN_NUM_MISO GPIO_NUM_12
#define PIN_NUM_CLK GPIO_NUM_26
#define PIN_NUM_CS GPIO_NUM_14

#define SPI_DMA_CHAN 1

#define TAG "storage"

sdmmc_card_t * card = NULL;
sdmmc_host_t host = SDSPI_HOST_DEFAULT();
sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
esp_vfs_fat_sdmmc_mount_config_t mount_config = {
    .format_if_mount_failed = false,
    .max_files = 5,
    .allocation_unit_size = 16 * 1024
};

esp_err_t storage_init() {
    ESP_LOGI(TAG, "Initializing SD card");
    
    host.slot = HSPI_HOST;
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = HSPI_HOST;
    
    return ESP_OK;
}
esp_err_t storage_mount(tStatus * status_out) {
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
    return ESP_OK;
}
esp_err_t storage_unmount(tStatus * status_out) {
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
    return ret;
}
esp_err_t storage_read_config(tStatus * status_out) {
    if(status_out == NULL) return ESP_ERR_INVALID_ARG;

    /*
     * Reset out argument
     */
    status_out->name_owner = NULL;
    status_out->has_weight = false;
    status_out->weight_kg = 0;
    status_out->logging_active = false;

    FILE * f = fopen(MOUNT_POINT"/config.txt", "rb");
    if(f != NULL) {
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
        if(cJSON_HasObjectItem(json, "weight_kg")){
            status_out->has_weight = true;
            status_out->weight_kg = cJSON_GetObjectItem(json, "weight_kg")->valueint;
        }
        if(cJSON_HasObjectItem(json, "ble_active")){
            status_out->ble_active = cJSON_GetObjectItem(json, "ble_active")->valueint == 0 ? 0 : 1;
        }
        if(cJSON_HasObjectItem(json, "logging_active")){
            status_out->logging_active = cJSON_GetObjectItem(json, "logging_active")->valueint == 0 ? 0 : 1;
        }

        cJSON_Delete(json);
        ESP_LOGI(TAG, "%s\n", data);
        free(data);
        fclose(f);
    } else {
        ESP_LOGE(TAG, "Was not possible to read 'config.txt\n");
        return ESP_FAIL;
    }
    return ESP_OK;
}