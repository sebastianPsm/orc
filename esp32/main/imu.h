#ifndef __IMU_H__
#define __IMU_H__

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <esp_log.h>
#include <driver/spi_master.h>

static constexpr int MOSI = 23;
static constexpr int MISO = 19;
static constexpr int SCLK = 18;
static constexpr int CS = 5;
static constexpr uint32_t CLOCK_SPEED = 1000000;  // up to 1MHz for all registers, and 20MHz for sensor data registers only

#ifdef __cplusplus
extern "C" {
#endif

void imu_init();
void imu_start_task();

#ifdef __cplusplus
}
#endif

#endif /* __IMU_H__ */