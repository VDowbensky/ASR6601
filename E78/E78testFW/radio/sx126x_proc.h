#ifndef _SX126X_PROC_H_
#define _SX126X_PROC_H_

#include "bsp.h"
#include "flash.h"
#include "sx126x_interface.h"
#include "radio_proc.h"

int8_t SX126X_config(void);
int8_t SX126X_LoRaConfig(void);
int8_t SX126X_FskConfig(void);
int8_t SX126X_initconfigstructure(void);


void SX126X_SetChannel(void);
void SX126X_setopmode(uint8_t mode);
void SX126X_LNAboost(bool boost);

void SX126X_CalibrateIR(void);

extern uint8_t opmode;
extern uint8_t prevopmode;

extern uint16_t irqflags;
extern uint8_t rfstatus;

#endif

