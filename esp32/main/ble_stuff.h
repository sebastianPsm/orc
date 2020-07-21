# ifndef __BLE_STUFF_H__
# define __BLE_STUFF_H__

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_gatts_api.h>
#include <esp_gap_ble_api.h>
#include <esp_bt_defs.h>
#include "esp_gatt_common_api.h"

#define PROFILE_NUM                 1
#define PROFILE_APP_IDX             0
#define ESP_APP_ID                  0x55
#define PRI_SVC_INST_ID             0
#define SEC_SVC_INST_ID             0
/* The max length of characteristic value. When the GATT client performs a write or prepare write operation,
*  the data length must be less than GATTS_DEMO_CHAR_VAL_LEN_MAX. 
*/
#define GATTS_DEMO_CHAR_VAL_LEN_MAX 500
#define PREPARE_BUF_MAX_SIZE        1024
#define CHAR_DECLARATION_SIZE       (sizeof(uint8_t))
#define ADV_CONFIG_FLAG             (1 << 0)
#define SCAN_RSP_CONFIG_FLAG        (1 << 1)

enum {
    IDX_PRI_SVC,
    IDX_PRI_SVC_RowerData_CharDecl,
    IDX_PRI_SVC_RowerData_CharVal,

    HRS_PRI_SVC_TableCount,
};

enum {
    IDX_SEC_SVC,
    IDX_SEC_SVC_BatLevel_CharDecl,
    IDX_SEC_SVC_BatLevel_CharVal,

    HRS_SEC_SVC_TableCount,
};

typedef struct {
    uint8_t * prepare_buf;
    int prepare_len;
} prepare_type_env_t;

struct gatts_profile_inst {
    esp_gatts_cb_t gatts_cb;
    uint16_t gatts_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_handle;
    esp_gatt_srvc_id_t service_id;
    uint16_t char_handle;
    esp_bt_uuid_t char_uuid;
    esp_gatt_perm_t perm;
    esp_gatt_char_prop_t property;
    uint16_t descr_handle;
    esp_bt_uuid_t descr_uuid;
};

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t ble_stuff_init();

#ifdef __cplusplus
}
#endif

# endif /* __BLE_STUFF_H__ */