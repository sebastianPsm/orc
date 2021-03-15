#ifndef __ANALYSIS_H__
#define __ANALYSIS_H__

#include <stdlib.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <dsps_fft2r.h>
#include <MASS.h>
#include "status.h"
#include "storage.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    tMass * mass;
    float * dist;
    bool print_quat_and_accel;
    tStatus * status;
    tLogEntry logEntry;
    int16_t * fftdata;
    int fftdataidx;
    float tLast;
    float freq;
} tAnalysis;

typedef enum {
    A_OK,
    A_INIT_FFT,
    A_UNKNOWN
} tAnalysisResult;

tAnalysis * analysis_init(tStatus * status);
tAnalysisResult analysis_terminate(tAnalysis ** h);
tAnalysisResult analysis_add(tAnalysis * h, long * quat, long * accel, long * gyro, float last_motion_s);

#ifdef __cplusplus
}
#endif

#endif /* __ANALYSIS_H__ */