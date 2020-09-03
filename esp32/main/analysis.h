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
    tStatus * status;
} tAnalysis;

typedef enum {
    A_OK,
    A_UNKNOWN
} tAnalysisResult;

void analysis_init(tStatus * status);
tAnalysisResult analysis_terminate();
tAnalysisResult analysis_add(long * quat, long * accel);

#ifdef __cplusplus
}
#endif

#endif /* __ANALYSIS_H__ */