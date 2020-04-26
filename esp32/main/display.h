# ifndef __DISPLAY_H__
# define  __DISPLAY_H__

#include <stdlib.h>
#include "epd2in9b.h"
#include "epdpaint.h"
#include "ubuntu16.h"
#include "Calibri.h"
#include "bt_logo.h"

typedef struct {
    unsigned spm; // strokes per minute
    bool ble_active; // Bluetooth LE active
} tDisplayStruct;

#ifdef __cplusplus
extern "C" {
#endif

void display_init();
void show_boot_screen();
void display_update();

#ifdef __cplusplus
}
#endif

# endif /* __DISPLAY_H__ */