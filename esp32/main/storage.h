#ifndef __STORAGE_H__
#define __STORAGE_H__

#include <stdbool.h>
#include <esp_err.h>
#include "status.h"

typedef struct {
    long accel_x;
    long accel_y;
    long accel_z;
    long gyro_x;
    long gyro_y;
    long gyro_z;
    float battery;
    float last_motion;
} tLogEntry;

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t storage_init();
esp_err_t storage_mount(tStatus * status_out);
esp_err_t storage_unmount(tStatus * status_out);

esp_err_t storage_read_config(tStatus * status_out);
esp_err_t storage_write_config(tStatus * status);
esp_err_t storage_write_log(tStatus * status, tLogEntry * tLogEntry);

#ifdef __cplusplus
}
#endif

#endif /* __STORAGE_H__ */