#ifndef _UC1601_INTERFACE_H_
#define _UC1601_INTERFACE_H_

#include "bsp.h"
#include "i2c.h"

#define UC1601_DATA_ADDR          0x3D
#define UC1601_CMD_ADDR           0x3C

void UC1601_interface_init(void);
void UC1601DataWrite(uint8_t ucData);
void UC1601CmdWrite(uint8_t ucCmd);
void UC1601DoubleCmdWrite(uint8_t ucCmd, uint8_t ucData);

#endif
