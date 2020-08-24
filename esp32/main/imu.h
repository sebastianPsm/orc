#ifndef __IMU_H__
#define __IMU_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#include <esp_log.h>

#include <mpu_idf_esp32.h>
#include <driver/include/mltypes.h>
#include <driver/eMPL/inv_mpu_dmp_motion_driver.h>
#include <mllite/ml_math_func.h>

#include "status.h"

#define I2C_SDA_GPIO (GPIO_NUM_21)
#define I2C_SCL_GPIO (GPIO_NUM_22)
#define MPU_INT_GPIO (GPIO_NUM_13)

typedef enum {
    IMU_ORIENT_UNDEFINED,
    IMU_ORIENT_PORTRAIT,
    IMU_ORIENT_LANDSCAPE,
    IMU_ORIENT_REVERSE_PORTRAIT,
    IMU_ORIENT_REVERSE_LANDSCAPE
} tImuOrientation;

typedef struct {
    tImuOrientation orientation;
    char isI2cInitialized;
    char isMpuInitialized;
    char isDmpInitialized;
} tImu;

typedef enum {
    IMU_OK,
    IMU_UNKNOWN
} tImuResult;

void imu_init(tStatus * status);
tImuResult imu_term();

#ifdef __cplusplus
}
#endif

#endif /* __IMU_H__ */