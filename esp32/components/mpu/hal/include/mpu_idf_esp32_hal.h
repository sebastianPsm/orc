#ifndef __MPU_IDF_ESP32_HAL_H__
#define __MPU_IDF_ESP32_HAL_H__

#include <esp_log.h>
#include <esp_timer.h>
#include <driver/gpio.h>
#include <driver/i2c.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#include "../../motion_driver_6.12/msp430/eMD-6.0/core/driver/eMPL/inv_mpu.h"

#pragma GCC diagnostic ignored "-Wmisleading-indentation"

/**
 *  @brief      Perform a blocking delay.
 *  @param[in]  num_ms  Number of milliseconds to delay.
 *  @return     0 if successful.
 */
int esp32_delay_ms(unsigned long num_ms);

/**
 *  @brief      Get current clock count.
 *  Timer overflow will occur after 2^32 milliseconds.
 *  @param[out] count   Timer count in milliseconds.
 *  @return      0 if successful.
 */
int esp32_get_clock_ms(unsigned long *count);

esp_err_t esp32_i2c_init(i2c_port_t port, int sda_io_num, bool sda_pullup_en, int scl_io_num, bool scl_pullup_en, uint32_t clk_speed, uint32_t timeout_ms);
esp_err_t esp32_i2c_term();
int esp32_i2c_write(unsigned char slave_addr, unsigned char reg_addr, unsigned char length, unsigned char const *data);
int esp32_i2c_read(unsigned char slave_addr, unsigned char reg_addr, unsigned char length, unsigned char *data);
int reg_int_cb(struct int_param_s *int_param);

void __no_operation();

#define i2c_write   esp32_i2c_write
#define i2c_read    esp32_i2c_read

#define min(x, y) (((x) < (y)) ? (x) : (y))

#define MPL_LOGI(fmt, ...) ESP_LOG_LEVEL_LOCAL(ESP_LOG_INFO, "mpu", fmt, ##__VA_ARGS__)
#define MPL_LOGE(fmt, ...) ESP_LOG_LEVEL_LOCAL(ESP_LOG_ERROR, "mpu", fmt, ##__VA_ARGS__)

#endif /* __MPU_IDF_ESP32_HAL_H__ */