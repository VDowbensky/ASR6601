#ifndef _SX126X_INTERFACE_H_
#define _SX126X_INTERFACE_H_

#include "bsp.h"
#include "lorac.h"
#include "sx126x.h"

#define SX126X_BUSY_TIMEOUT	1000000UL

void SX128X_select(void);
void SX126X_reset(void);
void SX126X_Wakeup(void);
bool SX126X_checkBusy(void);
void SX126X_select(void);
void SX126X_deselect(void);
void SX126X_writeCmd(uint8_t cmd, uint8_t *buffer, uint16_t size);
void SX126X_readCmd(uint8_t cmd, uint8_t *buffer, uint16_t size);
void SX126X_writeRegs(uint16_t reg, uint8_t *buffer, uint16_t size);
void SX126X_readRegs(uint16_t reg, uint8_t *buffer, uint16_t size);
void SX126X_writeReg(uint16_t reg, uint8_t value);
uint8_t SX126X_readReg(uint16_t reg);
void SX126X_writeBuffer(uint8_t offset, uint8_t *data, uint8_t length);
void SX126X_readBuffer(uint8_t offset, uint8_t *data, uint8_t length);
void SX126X_rfsw_tx(void);
void SX126X_rfsw_rx(void);
	
#endif