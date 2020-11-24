#ifndef __ANALYSIS_H__
#define __ANALYSIS_H__

#include <stdlib.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <MASS.h>
#include "status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    tMass * mass;
    float * dist;
    bool print_quat_and_accel;
} tAnalysis;

typedef enum {
    A_OK,
    A_UNKNOWN
} tAnalysisResult;

tAnalysis * analysis_init(bool print_quat_and_accel);
tAnalysisResult analysis_terminate(tAnalysis ** h);
tAnalysisResult analysis_add(tAnalysis * h, long * quat, long * accel, long * gyro);

#ifdef __cplusplus
}
#endif

#endif /* __ANALYSIS_H__ */