# ifndef __DISPLAY_H__
# define  __DISPLAY_H__

#include <stdlib.h>
#include "epd2in9b.h"
#include "epdpaint.h"
#include "ubuntu16.h"
#include "Calibri.h"
#include "bt_logo.h"
#include "status.h"

#ifdef __cplusplus
extern "C" {
#endif

void display_init();
void show_boot_screen();
void display_start_update_task(const tStatus * status);
void display_update();
void show_error_screen(const tStatus * status);

#ifdef __cplusplus
}
#endif

# endif /* __DISPLAY_H__ */