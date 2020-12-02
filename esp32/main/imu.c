#include "imu.h"
#include "analysis.h"

#define tag "imu"

tImu h;

signed char gyro_orientation[9] = { 1, 0, 0,
                                    0, 1, 0,
                                    0, 0, 1};

static void tap_cb(unsigned char direction, unsigned char count) {
    //ESP_LOGI(tag, "direction: %d, count: %d\n", direction, count);
}
static void orient_cb(unsigned char orientation) {
	switch (orientation) {
	case ANDROID_ORIENT_PORTRAIT:
        h.orientation = IMU_ORIENT_PORTRAIT;
        break;
	case ANDROID_ORIENT_LANDSCAPE:
        h.orientation = IMU_ORIENT_LANDSCAPE;
        break;
	case ANDROID_ORIENT_REVERSE_PORTRAIT:
        h.orientation = IMU_ORIENT_REVERSE_PORTRAIT;
        break;
	case ANDROID_ORIENT_REVERSE_LANDSCAPE:
        h.orientation = IMU_ORIENT_REVERSE_LANDSCAPE;
        break;
	default:
        h.orientation = IMU_ORIENT_UNDEFINED;
		break;
	}
}
void mpu_interrupt_cb(void * data) {
    tStatus * status = (tStatus *) data;

    short sensors;
    unsigned char more;
    long quat[4];
    long accel_l[3];
    long gyro_l[3];
    short accel_s[3];
    short gyro_s[3];

    dmp_read_fifo(gyro_s, accel_s, quat, &status->sensor_timestamp, &sensors, &more);
    if (sensors & (INV_WXYZ_QUAT|INV_XYZ_ACCEL|INV_XYZ_GYRO)) {
        inv_quaternion_to_rotation(quat, status->rot_matrix);
        accel_l[0] = accel_s[0]; accel_l[1] = accel_s[1]; accel_l[2] = accel_s[2];
        gyro_l[0] = gyro_s[0]; gyro_l[1] = gyro_s[1]; gyro_l[2] = gyro_s[2];
        inv_q_rotate(quat, accel_l, status->accel);
        inv_q_rotate(quat, gyro_l, status->gyro);
    }
    analysis_add(status->analysis, quat, status->accel, status->gyro);
}


void imu_init(tStatus * status) {
    int res;

    h.orientation = IMU_ORIENT_UNDEFINED;
    h.isI2cInitialized = 0;
    h.isMpuInitialized = 0;
    h.isDmpInitialized = 0;
    status->imu_is_initialized = 0;

    ESP_LOGI(tag, "Initialize...");

    res = (int) esp32_i2c_init(I2C_NUM_0, I2C_SDA_GPIO, true, I2C_SCL_GPIO, true, 200000, 10);
    if(res) {
        ESP_LOGE(tag, "I2C not initialized: %d", res);
        return;
    }
    h.isI2cInitialized = 1;

    
    h.int_param.interrupt_pin = (unsigned int) MPU_INT_GPIO;
    h.int_param.cb = mpu_interrupt_cb;
    h.int_param.arg = (tStatus *) status;
    res = (int) mpu_init(&(h.int_param));
    if(res) {
        MPL_LOGE("MPU not initialized: %d", res);
        return;
    }
    h.isMpuInitialized = 1;

    mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL | INV_XYZ_COMPASS);
    mpu_set_sample_rate(status->imu_sampler_rate_hz);

    res = dmp_load_motion_driver_firmware();
    if(res) {
        MPL_LOGE("DMP not initialized: %d", res);
        return;
    }
    h.isDmpInitialized = 1;
    status->imu_is_initialized = 1;

    dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_orientation));
    dmp_register_tap_cb(tap_cb);
    dmp_register_android_orient_cb(orient_cb);
    unsigned short dmp_features = DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_TAP |
        DMP_FEATURE_ANDROID_ORIENT | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO |
        DMP_FEATURE_GYRO_CAL;
    dmp_enable_feature(dmp_features);
    dmp_set_fifo_rate(20);
    mpu_set_dmp_state(1);

    ESP_LOGI(tag, "done (i2c: %d, mpu: %d, dmp: %d)", h.isI2cInitialized, h.isMpuInitialized, h.isDmpInitialized);

    short int_status = 0;
    mpu_get_int_status(&int_status);
    ESP_LOGI(tag, "mpu_get_int_status: %d\n", int_status);
}
tImuResult imu_term() {
    return IMU_OK;
}