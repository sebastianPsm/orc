#ifndef __STATUS_H__
#define __STATUS_H__

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    /*
     * Personal data
     */
    char * name_owner; // Name of the device owner
    bool has_weight;
    uint8_t weight_kg; // Weight [kg] of the entire vessel
    
    /*
     * rowing data
     */
    uint8_t spm; // strokes per minute
    

    /*
     * system data
     */
    bool ble_active; // Bluetooth LE 
    bool logging_active; // Logging data active
    bool sd_is_mounted; // SD card status
    bool imu_is_initialized; // IMU sensor status
    uint32_t sleep_after_s; // Start deep sleep after x number of seconds
} tStatus;


#endif /* __RUNTIME_STATUS_AND_CONFIG_H__ */