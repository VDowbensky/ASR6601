#ifndef _LORAC_H_
#define _LORAC_H_

#include "bsp.h"

#define LORAC_TXBUFSIZE 256
#define LORAC_RXBUFSIZE 256


void lorac_init(void);
uint8_t lorac_transfer(uint8_t b);
void lorac_reset(void);
void lorac_wait_on_busy(void);
void lorac_rfsw_rx(void);
void lorac_rfsw_tx(void);

//extern uint8_t lorac_TXbuffer[];
//extern uint8_t lorac_RXbuffer[];

#endif
