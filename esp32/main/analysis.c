#include "analysis.h"
#include <esp_log.h>

#define TAG "analysis"

#define N (512)
#define M (128)

//static float query[M] = { -0.4804, -0.4736, -0.4628, -0.4488, -0.4336, -0.4172, -0.4020, -0.3880, -0.3732, -0.3508, -0.3300, -0.3096, -0.2904, -0.2728, -0.2532, -0.2348, -0.2156, -0.2008, -0.1864, -0.1744, -0.1624, -0.1504, -0.1440, -0.1356, -0.1268, -0.1200, -0.1164, -0.1168, -0.1184, -0.1200, -0.1316, -0.1356, -0.1372, -0.1416, -0.1488, -0.1632, -0.1724, -0.1784, -0.1792, -0.1840, -0.1976, -0.2084, -0.2164, -0.2320, -0.2496, -0.2660, -0.2828, -0.2936, -0.3004, -0.3104, -0.3200, -0.3280, -0.3300, -0.3308, -0.3332, -0.3340, -0.3388, -0.3448, -0.3476, -0.3488, -0.3416, -0.3400, -0.3400, -0.3472, -0.3516, -0.3496, -0.3504, -0.3580, -0.3580, -0.3568, -0.3564, -0.3560, -0.3588, -0.3652, -0.3672, -0.3648, -0.3600, -0.3624, -0.3632, -0.3624, -0.3556, -0.3524, -0.3508, -0.3476, -0.3460, -0.3444, -0.3412, -0.3372, -0.3348, -0.3344, -0.3328, -0.3324, -0.3316, -0.3324, -0.3332, -0.3324, -0.3312, -0.3308, -0.3324, -0.3352, -0.3392, -0.3436, -0.3440, -0.3452, -0.3464, -0.3476, -0.3492, -0.3512, -0.3544, -0.3576, -0.3608, -0.3652, -0.3724, -0.3768, -0.3792, -0.3816, -0.3840, -0.3840, -0.3856, -0.3880, -0.3924, -0.3972, -0.4004, -0.4028, -0.4068, -0.4140, -0.4212, -0.4300 };

tAnalysis * analysis_init(const tStatus * status) {
    tAnalysis * h = (tAnalysis *) malloc(sizeof(tAnalysis));

//    h->mass = mass_init(N, M);
    h->dist = (float *) calloc(N-M+1, sizeof(float));
    h->print_quat_and_accel = false;
    h->status = status;

    return h;
}
tAnalysisResult analysis_terminate(tAnalysis ** h) {
    free((*h)->dist); (*h)->dist = NULL;
    mass_terminate(&((*h)->mass)); (*h)->mass = NULL;

    return A_OK;
}
tAnalysisResult analysis_add(tAnalysis * h, long * quat, long * accel, long * gyro, float last_motion_s) {
    if(!h->print_quat_and_accel) return A_OK;

    h->logEntry.accel_x = accel[0];
    h->logEntry.accel_y = accel[1];
    h->logEntry.accel_z = accel[2];
    h->logEntry.gyro_x = gyro[0];
    h->logEntry.gyro_y = gyro[1];
    h->logEntry.gyro_z = gyro[2];
    h->logEntry.battery = h->status->battery_voltage;
    h->logEntry.last_motion = last_motion_s;

    storage_write_log(h->status, &h->logEntry);
    return A_OK;
}