#ifndef _FLASH_H_
#define _FLASH_H_

#include "bsp.h"

#define CONFIG_ADDR			0x0800f000
#define CONFIG_SIZE			0x1000

void readconfig(void);
void writeconfig(void);


#endif
