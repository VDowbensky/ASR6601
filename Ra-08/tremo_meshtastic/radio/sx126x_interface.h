#ifndef _SX126X_INTERFACE_H_
#define _SX126X_INTERFACE_H_

#include "bsp.h"
#include "sx126x_defs.h"
//SPI interfaces
#include "lorac.h"

#define SX126X_BUSY_TIMEOUT 1000000UL

void SX128X_select(uint8_t sx);
void SX126X_reset(uint8_t sx);
void SX126X_Wakeup(uint8_t sx);
bool SX126X_checkBusy(uint8_t sx);
void SX126X_select(uint8_t sx);
void SX126X_deselect(uint8_t sx);
void SX126X_writeCmd(uint8_t sx,uint8_t cmd, uint8_t *buffer, uint16_t size);
void SX126X_readCmd(uint8_t sx,uint8_t cmd, uint8_t *buffer, uint16_t size);
void SX126X_writeRegs(uint8_t sx,uint16_t reg, uint8_t *buffer, uint16_t size);
void SX126X_readRegs(uint8_t sx,uint16_t reg, uint8_t *buffer, uint16_t size);
void SX126X_writeReg(uint8_t sx,uint16_t reg, uint8_t value);
uint8_t SX126X_readReg(uint8_t sx,uint16_t reg);
void SX126X_writeBuffer(uint8_t sx,uint8_t offset, uint8_t *data, uint8_t length);
void SX126X_readBuffer(uint8_t sx,uint8_t offset, uint8_t *data, uint8_t length);
void SX126X_rfsw_tx(uint8_t sx);
void SX126X_rfsw_rx(uint8_t sx);
uint8_t SX126X_spi_transfer(uint8_t sx,uint8_t b);
  
#endif