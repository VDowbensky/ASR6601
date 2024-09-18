#ifndef _ADC_H_
#define _ADC_H_

#include "bsp.h"

void myadc_init(void);
void kickADC(void);

extern float Vcc;
extern float T;

#endif
