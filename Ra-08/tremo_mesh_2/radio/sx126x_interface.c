#include "sx126x_interface.h"

void SX126X_reset(void)
{
	lorac_reset();
}

void SX126X_Wakeup(void)
{
    //BoardDisableIrq( );
    LORAC->NSS_CR = 0;
    delay_us(20);
    lorac_transfer(SX126X_GET_STATUS);
    lorac_transfer(0x00);
    LORAC->NSS_CR = 1;
    // Wait for chip to be ready.
    lorac_wait_on_busy();
    //BoardEnableIrq( );
}

bool SX126X_checkBusy(void)
{	
	uint32_t attempts = SX126X_BUSY_TIMEOUT;
	
	while(attempts--)
	{
		if(!(LORAC->SR & 0x100)) return true;
	}
	return false;
}

void SX126X_select(void)
{
	LORAC->NSS_CR = 0;
}

void SX126X_deselect(void)
{
	LORAC->NSS_CR = 1;
}

void SX126X_writeCmd(uint8_t cmd, uint8_t *buffer, uint16_t size)
{
	uint8_t i;
	
	SX126X_checkBusy();
	SX126X_select();
	lorac_transfer(cmd);
	for(i = 0;i < size;i++) lorac_transfer(buffer[i]);
	SX126X_deselect();
	if(cmd != SX126X_SET_SLEEP) SX126X_checkBusy();
}

void SX126X_readCmd(uint8_t cmd, uint8_t *buffer, uint16_t size)
{
	uint8_t i;
	
	SX126X_checkBusy();
	SX126X_select();
	lorac_transfer(cmd);
	for(i = 0;i < size;i++) buffer[i] = lorac_transfer(0xff);
	SX126X_deselect();
	SX126X_checkBusy();
}

void SX126X_writeRegs(uint16_t reg, uint8_t *buffer, uint16_t size)
{
  uint8_t addr_l,addr_h;
	uint8_t i;
	
	addr_l = reg & 0xff;
	addr_h = reg >> 8;
  SX126X_checkBusy();
  SX126X_select();
	lorac_transfer(SX126X_WRITE_REGISTER);
	lorac_transfer(addr_h);//MSB
	lorac_transfer(addr_l);//LSB
	for(i = 0;i < size; i++) lorac_transfer(buffer[i]);
	SX126X_deselect();
	SX126X_checkBusy();
}

void SX126X_readRegs(uint16_t reg, uint8_t *buffer, uint16_t size)
{
  uint16_t i;
	uint8_t addr_l,addr_h;
	
	addr_h = reg >> 8;
	addr_l = reg & 0x00FF;
	
  SX126X_checkBusy();
  SX126X_select();
	lorac_transfer(SX126X_READ_REGISTER);
	lorac_transfer(addr_h);//MSB
	lorac_transfer(addr_l);//LSB
  lorac_transfer(0xFF);
	for(i = 0; i < size; i++) buffer[i] = lorac_transfer(0xff);
	SX126X_deselect();
	SX126X_checkBusy();
}

void SX126X_writeReg(uint16_t reg, uint8_t value)
{
	SX126X_writeRegs(reg,&value,1);
}

uint8_t SX126X_readReg(uint16_t reg)
{
	uint8_t value;
	
	SX126X_readRegs(reg,&value,1);
	return value;
}

void SX126X_writeBuffer(uint8_t offset, uint8_t *data, uint8_t length)
{
	uint16_t i;
	
	SX126X_checkBusy();
  SX126X_select();
	lorac_transfer(SX126X_WRITE_BUFFER);
	lorac_transfer(offset);
	for(i = 0; i < length;i++) lorac_transfer(data[i]);
	SX126X_deselect();
	SX126X_checkBusy();
}

void SX126X_readBuffer(uint8_t offset, uint8_t *data, uint8_t length)
{
	uint16_t i;
	
	SX126X_checkBusy();
  SX126X_select();

	lorac_transfer(SX126X_READ_BUFFER);
	lorac_transfer(offset);
	lorac_transfer(0xFF);
	for(i = 0;i < length; i++) data[i] = lorac_transfer(0xff);
	SX126X_deselect();
	SX126X_checkBusy();
}

void SX126X_rfsw_tx(void)
{
	lorac_rfsw_tx();
}

void SX126X_rfsw_rx(void)
{
	lorac_rfsw_rx();
}



