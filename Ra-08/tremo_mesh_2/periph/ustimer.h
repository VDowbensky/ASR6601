#ifndef _USTIMER_H_
#define _USTIMER_H_

#include "bsp.h"

void ustimer_init(void);
void ustimer_start(void);
void ustimer_stop(void);
void ustimer_setinterval(uint16_t interval);

#endif
