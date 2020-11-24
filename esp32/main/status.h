#ifndef __STATUS_H__
#define __STATUS_H__

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    /*
     * Battery
     */
    float battery_voltage;

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
    
    unsigned long sensor_timestamp;
    long rot_matrix[9];
    long accel[3];
    long gyro[3];

    /*
     * system data
     */
    void * analysis;
    bool ble_active; // Bluetooth LE 
    bool sleep_active; // Sleep mode is active

    bool logging_active; // Logging data active
    bool sd_is_mounted; // SD card status
    unsigned log_file_suffix; // Log file suffix
    bool new_log_file; // log file already created

    bool imu_is_initialized; // IMU sensor status
    uint32_t sleep_after_s; // Start deep sleep after x number of seconds
    void (*sleep_cb)(); // Sleep callback is called if sleep_after_s exceeded

    uint16_t imu_sampler_rate_hz; // IMU sample rate in Hz (4 .. 1000 Hz)

    bool print_quat_and_accel;
    
    /*
     * Statistic
     */
    unsigned counter_run;
    long counter_log_bytes;
} tStatus;


#endif /* __RUNTIME_STATUS_AND_CONFIG_H__ */