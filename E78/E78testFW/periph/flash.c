#include "flash.h"
#include "radio_proc.h"

uint8_t flashbuffer[CONFIG_SIZE];

void readconfig(void)
{
	memcpy((void *)&radioConfig,(uint8_t*)(CONFIG_ADDR),sizeof(radioConfig));
}

void writeconfig(void)
{
	memcpy((void *)(&flashbuffer[0]),(uint8_t*)&radioConfig,sizeof(radioConfig));
	flash_erase_page(CONFIG_ADDR);
	flash_program_bytes(CONFIG_ADDR, flashbuffer, CONFIG_SIZE);
}