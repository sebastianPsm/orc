#ifndef __MPU_IDF_ESP32_H__
#define __MPU_IDF_ESP32_H__
#include <sdkconfig.h>

#include "mpu_idf_esp32_hal.h"

#ifdef CONFIG_MOTIONDRIVER_6050
#define MPU6050
#endif

#ifdef CONFIG_MOTIONDRIVER_6500
#define MPU6500
#endif

#ifdef CONFIG_MOTIONDRIVER_9150
#define MPU9150
#endif

#ifdef CONFIG_MOTIONDRIVER_9250
#define MPU9250
#endif

#endif /* __MPU_IDF_ESP32_H__ */