#include "bsp.h"
#include "lorac.h"
#include "sx126xx.h"

uint8_t lorac_TXbuffer[LORAC_TXBUFSIZE];
uint8_t lorac_RXbuffer[LORAC_RXBUFSIZE];

void lorac_init(void)
{
	LORAC->CR0 = 0x00000200;
  LORAC->SSP_CR0 = 0x07;
  LORAC->SSP_CPSR = 0x02;
  //wakeup lora 
  //avoid always waiting busy after main reset or soft reset
	if(LORAC->CR1 != 0x80)
	{
		delay_us(20);
		LORAC->NSS_CR = 0;
		delay_us(20);
		LORAC->NSS_CR = 1;
	}
	LORAC->SSP_CR1 = 0x02;
  NVIC_EnableIRQ(LORA_IRQn);
  //NVIC_SetPriority(LORAC_IRQn, 2);
	gpio_set_iomux(RFSW_TXRX_PORT, RFSW_TXRX_PIN, 3); //check
}

uint8_t lorac_transfer(uint8_t b)
{
  uint8_t data = 0;

  LORAC->SSP_DR = b;
	while(1) 
	{
		uint32_t status = LORAC->SSP_SR;
		if(((status & 0x01) == 0x01) && ((status & 0x10)==0)) break;
	}
	data = LORAC->SSP_DR & 0xFF;
  return data;
}

uint8_t lorac_transferblock(uint8_t len)
{
  uint8_t i;
	
	while(LORAC->SR & 0x100) {};
	LORAC->NSS_CR = 0;
	for(i = 0; i < len; i++) lorac_RXbuffer[i] = lorac_transfer(lorac_TXbuffer[i]);
	LORAC->NSS_CR = 1;
	while(LORAC->SR & 0x100) {};
  return i;
}

void lorac_reset(void)
{
	LORAC->CR1 &= ~(1<<5);  //nreset
	delay_us(100);
	LORAC->CR1 |= 1<<5;    //nreset release
	LORAC->CR1 &= ~(1<<7); //por release
	LORAC->CR0 |= 1<<5; //irq0
	LORAC->CR1 |= 0x1;  //tcxo
	while((LORAC->SR & 0x100));  
}

void SX126X_Wakeup(void)
{
    //BoardDisableIrq( );
    LORAC->NSS_CR = 0;
    delay_us(20);
    lorac_transfer(CMD_GET_STATUS);
    lorac_transfer(0x00);
    LORAC->NSS_CR = 1;
    // Wait for chip to be ready.
    lorac_wait_on_busy();
    //BoardEnableIrq( );
}

/*
void SX126X_Wakeup(void)
{
    lorac_TXbuffer[0] = CMD_GET_STATUS;
    lorac_TXbuffer[1] = 0;
    lorac_transferblock(2);
}
*/

void lorac_wait_on_busy(void)
{
	delay_us(10);
	while(LORAC->SR & 0x100);
}

void lorac_rfsw_on(void)
{
	gpio_init(RFSW_CTRL_PORT, RFSW_CTRL_PIN, GPIO_MODE_OUTPUT_PP_HIGH); //turn on RF switch power
}

void lorac_rfsw_off(void)
{
	gpio_init(RFSW_CTRL_PORT, RFSW_CTRL_PIN, GPIO_MODE_OUTPUT_PP_LOW); //turn off RF switch power
}
