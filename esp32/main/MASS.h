#ifndef __MASS_H__
#define __MASS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <fft.h>

struct tMassStruct;
typedef struct tMassStruct tMass;

tMass * mass_init(unsigned n, unsigned m);
void mass_terminate(tMass ** h);
void mass_findNN(tMass * h, float * x, float * y, float * dist);

#ifdef __cplusplus
}
#endif

#endif /* __MASS_H__ */