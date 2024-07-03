#include "ssd1306.h"

unsigned char SSD1306_framebuffer[1024];

int32_t ssd1306_Init(void)
{
  uint8_t initialization[] = {
    /* Enable charge pump regulator (RESET = ) */
    0x8d,
    0x14,
    /* Display On (RESET = ) */
    0xaf,
    /* Set Memory Addressing Mode to Horizontal Addressing Mode (RESET = Page Addressing Mode) */
    0x20,
    0x0,
    /* Reset Column Address (for horizontal addressing) */
    0x21,
    0,
    127,
    /* Reset Page Address (for horizontal addressing) */
    0x22,
    0,
    7
  };
	return ssd1306_WriteCommandBurst(initialization, sizeof(initialization));
	
}

/* Horizontal addressing mode maps to linear framebuffer */
void SSD1306_MINIMAL_setPixel(unsigned int x, unsigned int y) {
  x &= 0x7f;
  y &= 0x3f;
  SSD1306_framebuffer[((y & 0xf8) << 4) + x] |= 1 << (y & 7);
}




/*****************Low level functions***************************/

// Send a byte to the command register
int32_t ssd1306_WriteCommand(uint8_t command) 
{
	return ssd1306_Write(SSD1306_COMMAND, &command,1);
}

int32_t ssd1306_WriteCommandBurst(uint8_t *command, uint16_t len) 
{
	return ssd1306_Write(SSD1306_COMMAND_BURST,command,len);
}

// Send data
int32_t ssd1306_WriteData(uint8_t dat) 
{
  return ssd1306_Write(SSD1306_DATA,&dat,1);
}

int32_t ssd1306_WriteDataBurst(uint8_t *dat, uint16_t len) 
{
	return ssd1306_Write(SSD1306_DATA_BURST,dat,len);
}

int32_t ssd1306_Write(uint8_t method, uint8_t *buf, uint16_t len)
{
	int32_t retval = 0;
	
	i2c_master_send_start(I2C0, SSD1306_ADDR, I2C_WRITE);
  i2c_clear_flag_status(I2C0, I2C_FLAG_TRANS_EMPTY);
	//while (i2c_get_flag_status(I2C0, I2C_FLAG_TRANS_EMPTY) != SET);
	retval = i2c0_waitforflag(I2C_FLAG_TRANS_EMPTY);
	if(retval != 0) goto stop;
	i2c_send_data(I2C0,method);
	retval = i2c0_waitforflag(I2C_FLAG_TRANS_EMPTY);
	if(retval != 0) goto stop;
	// send data
	for(uint16_t i=0; i<len; i++) 
	{
		i2c_send_data(I2C0, buf[i]);
		i2c_clear_flag_status(I2C0, I2C_FLAG_TRANS_EMPTY);
		//while(i2c_get_flag_status(I2C0, I2C_FLAG_TRANS_EMPTY) != SET);
		retval = i2c0_waitforflag(I2C_FLAG_TRANS_EMPTY);
		if(retval != 0) goto stop;
	}
stop:
	// stop
	i2c_master_send_stop(I2C0);
	return retval;
}