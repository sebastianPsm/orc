#include "imu.h"

#include "SPIbus.hpp"
#include "MPU.hpp"        // main file, provides the class itself
#include "mpu/math.hpp"   // math helper for dealing with MPU data
#include "mpu/types.hpp"  // MPU data types and definitions

static const char* TAG = "imu";

MPU_t MPU;  // create a default MPU object

void imu_init() {
    spi_device_handle_t mpu_spi_handle;
    // Initialize SPI on VSPI host through SPIbus interface:
    vspi.begin(MOSI, MISO, SCLK);
    vspi.addDevice(0, CLOCK_SPEED, CS, &mpu_spi_handle);

    MPU.setBus(vspi);  // set bus port, not really needed here since default is VSPI
    MPU.setAddr(mpu_spi_handle);  // set spi_device_handle, always needed!
    
    while (esp_err_t err = MPU.testConnection()) {
        ESP_LOGE(TAG, "Failed to connect to the MPU, error=%#X", err);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    ESP_LOGI(TAG, "MPU connection successful!");

    ESP_ERROR_CHECK(MPU.initialize());  // initialize the chip and set initial configurations
}
void imu_read() {
    // Reading sensor data
    printf("Reading sensor data:\n");
    mpud::raw_axes_t accelRaw;   // x, y, z axes as int16
    mpud::raw_axes_t gyroRaw;    // x, y, z axes as int16
    mpud::raw_axes_t mag; 
    mpud::float_axes_t accelG;   // accel axes in (g) gravity format
    mpud::float_axes_t gyroDPS;  // gyro axes in (DPS) º/s format
    int16_t temp;
    while (true) {
        // Read
        MPU.acceleration(&accelRaw);  // fetch raw data from the registers
        MPU.rotation(&gyroRaw);       // fetch raw data from the registers
        MPU.heading(&mag);
        MPU.temperature(&temp);
        // MPU.motion(&accelRaw, &gyroRaw);  // read both in one shot
        // Convert
        accelG = mpud::accelGravity(accelRaw, mpud::ACCEL_FS_4G);
        gyroDPS = mpud::gyroDegPerSec(gyroRaw, mpud::GYRO_FS_500DPS);
        

        // Debug
        printf("accel: [%+6.2f %+6.2f %+6.2f ] (G) \t", accelG.x, accelG.y, accelG.z);
        printf("gyro: [%+7.2f %+7.2f %+7.2f ] (º/s)\t", gyroDPS[0], gyroDPS[1], gyroDPS[2]);
        printf("heading: [%d %d %d ]\t", mag.x, mag.y, mag.z);
        printf("temp: %.2f degC\n", mpud::tempCelsius(temp));
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}