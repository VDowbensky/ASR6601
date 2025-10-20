#include "flash.h"
#include "radio.h"

uint8_t flashbuffer[CONFIG_SIZE];

void readconfig(void)
{
	memcpy((void *)&radioconfig,(uint8_t*)(CONFIG_ADDR),sizeof(radioconfig));
}

void writeconfig(void)
{
	memcpy((void *)(&flashbuffer[0]),(uint8_t*)&radioconfig,sizeof(radioconfig));
	flash_erase_page(CONFIG_ADDR);
	flash_program_bytes(CONFIG_ADDR, flashbuffer, CONFIG_SIZE);
}