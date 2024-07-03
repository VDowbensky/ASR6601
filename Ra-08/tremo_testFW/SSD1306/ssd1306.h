#ifndef _SSD1306_H_
#define _SSD1306_H_

#include "bsp.h"
#include "i2c.h"

#define SSD1306_ADDR0				0x3c //DC=0
#define SSD1306_ADDR1				0x3d //DC=1

#define SSD1306_ADDR 				SSD1306_ADDR0

//#define SSD1306_COMMAND           0x80  // Continuation bit=1, D/C=0; 1000 0000
//#define SSD1306_COMMAND_STREAM    0x00  // Continuation bit=0, D/C=0; 0000 0000
//#define SSD1306_DATA              0xC0  // Continuation bit=1, D/C=1; 1100 0000
//#define SSD1306_DATA_STREAM       0x40  // Continuation bit=0, D/C=1; 0100 0000

#define SSD1306_COMMAND						0x00
#define SSD1306_COMMAND_BURST			0x80
#define SSD1306_DATA							0x40
#define SSD1306_DATA_BURST				0xC0

int32_t ssd1306_Init(void);

int32_t ssd1306_Write(uint8_t method, uint8_t *buf, uint16_t len);
int32_t ssd1306_WriteCommand(uint8_t command);
int32_t ssd1306_WriteCommandBurst(uint8_t *command, uint16_t len);
int32_t ssd1306_WriteDataBurst(uint8_t *dat, uint16_t len);

#endif
