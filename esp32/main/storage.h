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
esp_err_t storage_write_log(tStatus * status, long accel_x, long accel_y, long accel_z, long yaw, long pitch, long roll, float battery, float last_motion);

#ifdef __cplusplus
}
#endif

#endif /* __STORAGE_H__ */