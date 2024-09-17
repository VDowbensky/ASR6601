#ifndef _I2C_H_
#define _I2C_H_

#include "bsp.h"
#include "tremo_i2c.h"

#define I2C_TIMEOUT 100000L

int32_t i2c0_init(void);
int32_t i2c0_write(uint8_t addr, uint8_t *data, uint16_t len);
int32_t i2c0_read(uint8_t addr, uint8_t *data, uint16_t len);
int32_t i2c0_waitforflag(i2c_flag_t flag);

#endif
