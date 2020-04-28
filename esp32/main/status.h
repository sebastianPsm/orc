#ifndef __STATUS_H__
#define __STATUS_H__

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    // rowing data
    char * name_owner; // Name of the device owner
    uint8_t spm; // strokes per minute
    bool has_weight;
    uint8_t weight_kg; // Weight [kg] of the entire vessel

    // system data
    bool ble_active; // Bluetooth LE 
    bool logging_active; // Logging data active
} tStatus;


#endif /* __RUNTIME_STATUS_AND_CONFIG_H__ */