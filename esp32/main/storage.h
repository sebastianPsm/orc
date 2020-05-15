#ifndef __STORAGE_H__
#define __STORAGE_H__

#include <stdbool.h>
#include <esp_err.h>
#include "status.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t storage_init();
esp_err_t storage_mount(tStatus * status_out);
esp_err_t storage_unmount(tStatus * status_out);

esp_err_t storage_read_config(tStatus * status_out);
esp_err_t storage_write_config(tStatus * status);
esp_err_t storage_write_log(tStatus * status, float accel_x, float accel_y, float accel_z, float gyro_x, float gyro_y, float gyro_z, float battery, float last_motion);

#ifdef __cplusplus
}
#endif

#endif /* __STORAGE_H__ */