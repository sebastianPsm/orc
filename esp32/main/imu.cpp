#include "imu.h"

#include "SPIbus.hpp"
#include "MPU.hpp"        // main file, provides the class itself
#include "mpu/math.hpp"   // math helper for dealing with MPU data
#include "mpu/types.hpp"  // MPU data types and definitions

#include "storage.h"

static const char* TAG = "imu";

MPU_t MPU;  // create a default MPU object

mpud::types::accel_fs_t accelScale = mpud::ACCEL_FS_4G;
mpud::types::gyro_fs_t gyroScale = mpud::GYRO_FS_500DPS;
static constexpr gpio_num_t kInterruptPin = GPIO_NUM_13;  // GPIO_NUM
uint64_t last_motion;

static constexpr mpud::int_config_t kInterruptConfig{
    .level = mpud::INT_LVL_ACTIVE_HIGH,
    .drive = mpud::INT_DRV_PUSHPULL,
    .mode  = mpud::INT_MODE_PULSE50US,
    .clear = mpud::INT_CLEAR_STATUS_REG  //
};

void imu_init(tStatus * status) {
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
    status->imu_is_initialized = true;

    ESP_ERROR_CHECK(MPU.initialize());  // initialize the chip and set initial configurations

    /*
     * Initialize sleep mode
     */
    esp_sleep_enable_ext0_wakeup(kInterruptPin, 1);

    /*
     * Motion detection
     */
    mpud::mot_config_t config = {
        .threshold = 6,
    };
    MPU.setMotionDetectConfig(config);
    MPU.setMotionFeatureEnabled(true);

    MPU.setSampleRate(status->imu_sampler_rate_hz);
    MPU.setAccelFullScale(accelScale);
    MPU.setGyroFullScale(gyroScale);
}

static IRAM_ATTR void imuISR(TaskHandle_t taskHandle) {
    BaseType_t HPTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(taskHandle, &HPTaskWoken);
    if (HPTaskWoken == pdTRUE) portYIELD_FROM_ISR();
}

static void imu_read_task(void * data) {
    tStatus * status = (tStatus *) data;

    constexpr uint16_t kFIFOPacketSize = 12;
    mpud::float_axes_t accelG;   // accel axes in (g) gravity format
    mpud::float_axes_t gyroDPS;  // gyro axes in (DPS) º/s format

    ESP_LOGI(TAG, "Start task... ");

    /*
     * use LED (GPIO_NUM_2) for motion detection
     */
    gpio_pad_select_gpio(GPIO_NUM_2);
    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);

    /*
     * Setup FIFO
     */
    ESP_ERROR_CHECK(MPU.setFIFOConfig(mpud::FIFO_CFG_ACCEL | mpud::FIFO_CFG_GYRO));
    ESP_ERROR_CHECK(MPU.setFIFOEnabled(true));

    /*
     * Setup interrupt
     */
    constexpr gpio_config_t kGPIOConfig {
        .pin_bit_mask = (uint64_t) 0x1 << kInterruptPin,
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type    = GPIO_INTR_POSEDGE  //
    };
    gpio_config(&kGPIOConfig);
    gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
    gpio_isr_handler_add(kInterruptPin, imuISR, xTaskGetCurrentTaskHandle());
    ESP_ERROR_CHECK(MPU.setInterruptConfig(kInterruptConfig));
    ESP_ERROR_CHECK(MPU.setInterruptEnabled(mpud::INT_EN_RAWDATA_READY|mpud::INT_EN_MOTION_DETECT));

    // Ready to start reading
    ESP_ERROR_CHECK(MPU.resetFIFO());  // start clean

    int64_t t_old = esp_timer_get_time();
    last_motion = t_old;

    ESP_LOGI(TAG, "Task started");
    while (true) {
        // Wait for notification from mpuISR
        uint32_t notificationValue = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (notificationValue > 1) {
            ESP_LOGW(TAG, "Task Notification higher than 1, value: %d", notificationValue);
            MPU.resetFIFO();
            continue;
        }
        int64_t t = esp_timer_get_time();

        uint16_t fifocount = MPU.getFIFOCount();
        if (esp_err_t err = MPU.lastError()) {
            ESP_LOGE(TAG, "Error reading fifo count, %#X", err);
            MPU.resetFIFO();
            continue;
        }
        if (fifocount > kFIFOPacketSize * 2) {
            if (!(fifocount % kFIFOPacketSize)) {
                ESP_LOGE(TAG, "Sample Rate too high!, not keeping up the pace!, count: %d", fifocount);
            } else {
                ESP_LOGE(TAG, "FIFO Count misaligned! Expected: %d, Actual: %d", kFIFOPacketSize, fifocount);
            }
            MPU.resetFIFO();
            continue;
        }

        // Burst read data from FIFO
        uint8_t buffer[kFIFOPacketSize];
        if (esp_err_t err = MPU.readFIFO(kFIFOPacketSize, buffer)) {
            ESP_LOGE(TAG, "Error reading sensor data, %#X", err);
            MPU.resetFIFO();
            continue;
        }
        // Format
        mpud::raw_axes_t rawAccel, rawGyro;
        rawAccel.x = buffer[0] << 8 | buffer[1];
        rawAccel.y = buffer[2] << 8 | buffer[3];
        rawAccel.z = buffer[4] << 8 | buffer[5];
        rawGyro.x  = buffer[6] << 8 | buffer[7];
        rawGyro.y  = buffer[8] << 8 | buffer[9];
        rawGyro.z  = buffer[10] << 8 | buffer[11];

        /*
         * Sleep function
         * - get reason for the interrupt
         * - if motion, then reset 'last_motion' with current time
         * - if 'sleep_after_s' exceeds, then go sleep
         */
        mpud::types::int_stat_t mpu_status = MPU.getInterruptStatus();
        if(mpu_status & mpud::INT_STAT_MOTION_DETECT)
            last_motion = t;
        
        if((t - last_motion) > 5*1e6){
            gpio_set_level(GPIO_NUM_2, 1);
        } else {
            gpio_set_level(GPIO_NUM_2, 0);
        }

        if((t - last_motion) > (uint64_t) (status->sleep_after_s * 1e6)) {
            /*
             * Disable further data interrupts from IMU (will wakeup the ESP),
             * but keep motion detect interrupts
             */
            ESP_ERROR_CHECK(MPU.setInterruptEnabled(mpud::INT_EN_MOTION_DETECT));
            vTaskDelay(2 / portTICK_RATE_MS);

            status->sleep_cb();
        }

        // Convert
        accelG = mpud::accelGravity(rawAccel, accelScale);
        gyroDPS = mpud::gyroDegPerSec(rawGyro, gyroScale);

        storage_write_log(status, accelG.x, accelG.y, accelG.z, gyroDPS[0], gyroDPS[1], gyroDPS[2], 0.0, (t-last_motion)/1e6);

        t_old = t;
    }
    vTaskDelete(NULL);
}

void imu_start_task(tStatus * status) {
    xTaskCreate(imu_read_task, "imu_read_task", 4 * 1024, (void *) status, 6, NULL);
}