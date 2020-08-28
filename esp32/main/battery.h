#ifndef __BATTERY_H__
#define __BATTERY_H__

#include <stdlib.h>
#include <stdio.h>
#include <esp_log.h>
#include <driver/gpio.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define BAT_ADC (GPIO34)

#ifdef __cplusplus
extern "C" {
#endif

void battery_init();
float getBatterySoc(float old);

#ifdef __cplusplus
}
#endif

#endif /* BATTERY_H__ */