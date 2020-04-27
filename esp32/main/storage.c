#include "storage.h"

#include <esp_log.h>
#include <driver/gpio.h>
#include <esp_vfs_fat.h>
#include <driver/sdspi_host.h>
#include <sdmmc_cmd.h>
#include <unistd.h>

#define MOUNT_POINT "/sdcard"

// Pin definition
#define PIN_NUM_MOSI GPIO_NUM_27
#define PIN_NUM_MISO GPIO_NUM_12
#define PIN_NUM_CLK GPIO_NUM_26
#define PIN_NUM_CS GPIO_NUM_14

#define SPI_DMA_CHAN 1

#define TAG "storage"

sdmmc_card_t * card = NULL;

esp_err_t storage_init() {
    esp_err_t ret;

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
    const char mount_point[] = MOUNT_POINT;

    ESP_LOGI(TAG, "Initializing SD card");

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.slot = HSPI_HOST;

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = HSPI_HOST;

    ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);
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

    
    FILE * f = fopen(MOUNT_POINT"/name.txt", "r");
    char line[64];
    if(f != NULL) {
        fgets(line, sizeof(line), f);
        fclose(f);
        ESP_LOGI(TAG, "content if name.txt: %s", line);
    }
    ESP_LOGI(TAG, "No name.txt found");
    
    return ESP_OK;
}