#ifndef __ANALYSIS_H__
#define __ANALYSIS_H__

#include <stdlib.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <MASS.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    unsigned accel_buffer_idx;
    float accel_buffer_a[1024];
    float accel_buffer_b[1024];

    tMass * mass;
    float * dist;
} tAnalysis;

typedef enum {
    A_OK,
    A_UNKNOWN
} tAnalysisResult;

tAnalysis * analysis_init();
tAnalysisResult analysis_terminate(tAnalysis ** h);

tAnalysisResult analysis_accel_buffer_append(tAnalysis * h, float * x, float * y, float * z);

#ifdef __cplusplus
}
#endif

#endif /* __ANALYSIS_H__ */