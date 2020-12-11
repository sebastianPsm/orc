#ifndef __ANALYSIS_H__
#define __ANALYSIS_H__

#include <stdlib.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
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
    const tStatus * status;
    tLogEntry logEntry;
} tAnalysis;

typedef enum {
    A_OK,
    A_UNKNOWN
} tAnalysisResult;

tAnalysis * analysis_init(const tStatus * status);
tAnalysisResult analysis_terminate(tAnalysis ** h);
tAnalysisResult analysis_add(tAnalysis * h, long * quat, long * accel, long * gyro, float last_motion_s);

#ifdef __cplusplus
}
#endif

#endif /* __ANALYSIS_H__ */