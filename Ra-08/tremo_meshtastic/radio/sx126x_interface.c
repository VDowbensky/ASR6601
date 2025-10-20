#include "sx126x_interface.h"

void SX126X_reset(uint8_t sx)
{
    lorac_reset();
}

void SX126X_Wakeup(uint8_t sx)
{
    SX126X_select(sx);
    delay_us(20);
    SX126X_spi_transfer(sx,SX126X_GET_STATUS);
    SX126X_spi_transfer(sx,0x00);
    SX126X_deselect(sx);
    // Wait for chip to be ready.
    SX126X_checkBusy(sx);
}

bool SX126X_checkBusy(uint8_t sx)
{ 
  uint32_t attempts = SX126X_BUSY_TIMEOUT;
    while(attempts--)
    {
      if(!(LORAC->SR & 0x100)) return true;
    }
		return false;
}

void SX126X_select(uint8_t sx)
{
    LORAC->NSS_CR = 0;
}

void SX126X_deselect(uint8_t sx)
{
    LORAC->NSS_CR = 1;
}

void SX126X_writeCmd(uint8_t sx,uint8_t cmd, uint8_t *buffer, uint16_t size)
{
  uint8_t i;
  SX126X_checkBusy(sx);
  SX126X_select(sx);
  SX126X_spi_transfer(sx,cmd);
  for(i = 0;i < size;i++) SX126X_spi_transfer(sx,buffer[i]);
  SX126X_deselect(sx);
  if(cmd != SX126X_SET_SLEEP) SX126X_checkBusy(sx);
}

void SX126X_readCmd(uint8_t sx,uint8_t cmd, uint8_t *buffer, uint16_t size)
{
  uint8_t i;
  
  SX126X_checkBusy(sx);
  SX126X_select(sx);
  SX126X_spi_transfer(sx,cmd);
  for(i = 0;i < size;i++) buffer[i] = SX126X_spi_transfer(sx,0xff);
  SX126X_deselect(sx);
  SX126X_checkBusy(sx);
}

void SX126X_writeRegs(uint8_t sx,uint16_t reg, uint8_t *buffer, uint16_t size)
{
  uint8_t addr_l,addr_h;
  uint8_t i;
  
  addr_l = reg & 0xff;
  addr_h = reg >> 8;
  SX126X_checkBusy(sx);
  SX126X_select(sx);
  SX126X_spi_transfer(sx,SX126X_WRITE_REGISTER);
  SX126X_spi_transfer(sx,addr_h);//MSB
  SX126X_spi_transfer(sx,addr_l);//LSB
  for(i = 0;i < size; i++) SX126X_spi_transfer(sx,buffer[i]);
  SX126X_deselect(sx);
  SX126X_checkBusy(sx);
}

void SX126X_readRegs(uint8_t sx,uint16_t reg, uint8_t *buffer, uint16_t size)
{
  uint16_t i;
  uint8_t addr_l,addr_h;
  
  addr_h = reg >> 8;
  addr_l = reg & 0x00FF;
  
  SX126X_checkBusy(sx);
  SX126X_select(sx);
  SX126X_spi_transfer(sx,SX126X_READ_REGISTER);
  SX126X_spi_transfer(sx,addr_h);//MSB
  SX126X_spi_transfer(sx,addr_l);//LSB
  SX126X_spi_transfer(sx,0xff);
  for(i = 0; i < size; i++) buffer[i] = SX126X_spi_transfer(sx,0xff);
  SX126X_deselect(sx);
  SX126X_checkBusy(sx);
}

void SX126X_writeReg(uint8_t sx,uint16_t reg, uint8_t value)
{
  SX126X_writeRegs(sx,reg,&value,1);
}

uint8_t SX126X_readReg(uint8_t sx,uint16_t reg)
{
  uint8_t value;
  
  SX126X_readRegs(sx,reg,&value,1);
  return value;
}

void SX126X_writeBuffer(uint8_t sx,uint8_t offset, uint8_t *data, uint8_t length)
{
  uint16_t i;
  
  SX126X_checkBusy(sx);
  SX126X_select(sx);
  SX126X_spi_transfer(sx,SX126X_WRITE_BUFFER);
  SX126X_spi_transfer(sx,offset);
  for(i = 0; i < length;i++) SX126X_spi_transfer(sx,data[i]);
  SX126X_deselect(sx);
  SX126X_checkBusy(sx);
}

void SX126X_readBuffer(uint8_t sx,uint8_t offset, uint8_t *data, uint8_t length)
{
  uint16_t i;
  
  SX126X_checkBusy(sx);
  SX126X_select(sx);
  SX126X_spi_transfer(sx,SX126X_READ_BUFFER);
  SX126X_spi_transfer(sx,offset);
  SX126X_spi_transfer(sx,0xff);
  for(i = 0;i < length; i++) data[i] = SX126X_spi_transfer(sx,0xff);
  SX126X_deselect(sx);
  SX126X_checkBusy(sx);
}

void SX126X_rfsw_tx(uint8_t sx)
{
  switch(sx)
  {
    case 0:
    lorac_rfsw_tx();
    break;
    
    default:
    break;
  }
}

void SX126X_rfsw_rx(uint8_t sx)
{
  switch(sx)
  {
    case 0:
    lorac_rfsw_rx();
    break;
    
    default:
    break;
  }
}

uint8_t SX126X_spi_transfer(uint8_t sx,uint8_t b)
{
  return lorac_transfer(b);
}




