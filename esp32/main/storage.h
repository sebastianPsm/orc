#ifndef __STORAGE_H__
#define __STORAGE_H__

#include <stdbool.h>
#include <esp_err.h>
#include "status.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t storage_init();
esp_err_t storage_mount();
esp_err_t storage_unmount();

esp_err_t storage_read_config(tStatus * status_out);

#ifdef __cplusplus
}
#endif

#endif /* __STORAGE_H__ */