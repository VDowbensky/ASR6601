#ifndef _SSD1306_INTERFACE_H_
#define _SSD1306_INTERFACE_H_
#include "bsp.h"

#if(SSD1306_INTERFACE_I2C)
#include "i2c.h"
#define SSD1306_ADDR0			0x3c
#define SSD1306_ADDR1			0x3d
#define SSD1306_ADDR			SSD1306_ADDR0
#elif SSD1306_INTERFACE_HARD_SPI
#include "spi.h"
//#else //soft SPI
//#include "softspi.h"
#endif

void SSD1306_interface_init(void);
void SSD1306_Select(void);
void SSD1306_Deselect(void);
void SSD1306_WriteByte(uint8_t b);
void SSD1306_Command(bool cmd);
void SSD1306_Reset(void);
void SSD1306_WR_Byte(uint8_t dat,uint8_t cmd); 

extern char strbuffer[];

#endif
