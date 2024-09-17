#include "sx128x_interface.h"

uint8_t radiospi_transfer(uint8_t b);

void SX128X_reset(void)
{
	delay_ms(20);	
	gpio_init(SX128X_RESET_PORT, SX128X_RESET_PIN, GPIO_MODE_OUTPUT_PP_LOW);
	delay_ms(50);		
	gpio_init(SX128X_RESET_PORT, SX128X_RESET_PIN, GPIO_MODE_OUTPUT_PP_HIGH);
	delay_ms(20);		
}

void SX128X_select(void)
{
	gpio_init(SX128X_NSS_PORT, SX128X_NSS_PIN, GPIO_MODE_OUTPUT_PP_LOW);
}

void SX128X_deselect(void)
{
	gpio_init(SX128X_NSS_PORT, SX128X_NSS_PIN, GPIO_MODE_OUTPUT_PP_HIGH);
}

void radiospi_init(void)
{
	//NSS
	gpio_init(SX128X_NSS_PORT, SX128X_NSS_PIN, GPIO_MODE_OUTPUT_PP_HIGH);
	//SPI pins
	gpio_set_iomux(SX128X_SCK_PORT, SX128X_SCK_PIN, 4);
	gpio_set_iomux(SX128X_MOSI_PORT, SX128X_MOSI_PIN, 4);
	gpio_set_iomux(SX128X_MISO_PORT, SX128X_MISO_PIN, 4);
	//RESET
	gpio_init(SX128X_RESET_PORT, SX128X_RESET_PIN, GPIO_MODE_OUTPUT_PP_HIGH);
	//TCXO
	gpio_init(SX128X_TCXO_PORT, SX128X_TCXO_PIN, GPIO_MODE_OUTPUT_PP_HIGH);
	gpio_config_drive_capability(SX128X_TCXO_PORT, SX128X_TCXO_PIN, GPIO_DRIVE_CAPABILITY_8MA);
	//BUSY
	gpio_init(SX128X_BUSY_PORT, SX128X_BUSY_PIN, GPIO_MODE_INPUT_PULL_DOWN);
	//DIO1
	gpio_init(SX128X_DIO1_PORT, SX128X_DIO1_PIN, GPIO_MODE_INPUT_PULL_DOWN);
	//DIO2
	//gpio_init(SX128X_DIO2_PORT, SX128X_DIO2_PIN, GPIO_MODE_INPUT_PULL_DOWN);
	//DIO3
	//gpio_init(SX128X_DIO3_PORT, SX128X_DIO3_PIN, GPIO_MODE_INPUT_PULL_DOWN);
	//configure DIO1 interrupt
	gpio_config_interrupt(SX128X_DIO1_PORT, SX128X_DIO1_PIN, GPIO_INTR_RISING_EDGE);
	NVIC_ClearPendingIRQ(GPIO_IRQn);
	NVIC_EnableIRQ(GPIO_IRQn);
	
	//init SPI module
	ssp_init_t init_struct;
	rcc_enable_peripheral_clk(RCC_PERIPHERAL_SSP1, true);
	//ssp_init_struct(&init_struct);
	init_struct.ssp_sclk      = 4000000; // 4M
	init_struct.ssp_role      = SSP_ROLE_MASTER;
	init_struct.ssp_format    = SSP_FRAME_FORMAT_SPI;
	init_struct.ssp_data_size = SSP_DATA_SIZE_8BIT;
	init_struct.ssp_clk_pol   = SPI_CLK_POLARITY_HIGH;
	init_struct.ssp_clk_phase = SPI_CLK_PHASE_2EDGE;
	init_struct.ssp_dma_tx_en = DISABLE;
	init_struct.ssp_dma_rx_en = DISABLE;
	ssp_init(SSP1, &init_struct);
	ssp_cmd(SSP1, ENABLE);
}

uint8_t radiospi_transfer(uint8_t b)
{
	uint8_t retval = 0;
	
  SSP0->DR = b;
	while(1) 
	{
		uint32_t status = SSP1->SR;
		if(((status & 0x01) == 0x01) && ((status & 0x10)==0)) break;
	}
	retval = SSP1->DR & 0xFF;
  return retval;
}

bool SX128X_checkBusy(void)
{
	uint32_t attempts = SX128X_BUSY_TIMEOUT;
	
	while(attempts--)
	{
		if(gpio_read(SX128X_BUSY_PORT, SX128X_BUSY_PIN) == GPIO_LEVEL_LOW) return true;
	}
	return false;
}

//DIO1 interrupt
void GPIO_IRQHandler(void)
{
	if(gpio_get_interrupt_status(SX128X_DIO1_PORT, SX128X_DIO1_PIN) == SET) 
	{
		gpio_clear_interrupt(SX128X_DIO1_PORT, SX128X_DIO1_PIN);
  //uint16_t irqstatus;
  //read SX128x status
  //irqstatus = SX128X_GetIrqStatus();
  //SX128X_ClearIrqStatus(SX126X_ALL_IRQMSK);
  //if(irqstatus & SX126X_TXDONE_IRQMSK) packet_sent = true;
  //if(irqstatus & SX126X_RXDONE_IRQMSK) packet_received = true;
  //if(irqstatus & SX126X_CRCERR_IRQMSK) crc_error = true;
	}
}

void SX128X_writeCmd(uint8_t cmd, uint8_t *buffer, uint16_t size)
{
	uint8_t i;
	
	SX128X_checkBusy();
	SX128X_select();
	radiospi_transfer(cmd);
	for(i = 0;i < size;i++) radiospi_transfer(buffer[i]);
	SX128X_deselect();
	if(cmd != SX128X_SET_SLEEP) SX128X_checkBusy();
}

void SX128X_readCmd(uint8_t cmd, uint8_t *buffer, uint16_t size)
{
	uint8_t i;
	
	SX128X_checkBusy();
	SX128X_select();
	radiospi_transfer(cmd);
	for(i = 0;i < size;i++) buffer[i] = radiospi_transfer(0xff);
	SX128X_deselect();
	SX128X_checkBusy();
}

void SX128X_writeRegs(uint16_t reg, uint8_t *buffer, uint16_t size)
{
  uint8_t addr_l,addr_h;
	uint8_t i;
	
	addr_l = reg & 0xff;
	addr_h = reg >> 8;
  SX128X_checkBusy();
  SX128X_select();
	radiospi_transfer(SX128X_WRITE_REGISTER);
	radiospi_transfer(addr_h);//MSB
	radiospi_transfer(addr_l);//LSB
	for(i = 0;i < size; i++) radiospi_transfer(buffer[i]);
	SX128X_deselect();
	SX128X_checkBusy();
}

void SX128X_readRegs(uint16_t reg, uint8_t *buffer, uint16_t size)
{
  uint16_t i;
	uint8_t addr_l,addr_h;
	
	addr_h = reg >> 8;
	addr_l = reg & 0x00FF;
	
  SX128X_checkBusy();
  SX128X_select();
	radiospi_transfer(SX128X_READ_REGISTER);
	radiospi_transfer(addr_h);//MSB
	radiospi_transfer(addr_l);//LSB
  radiospi_transfer(0xFF);
	for(i = 0; i < size; i++) buffer[i] = radiospi_transfer(0xff);
	SX128X_deselect();
	SX128X_checkBusy();
}

void SX128X_writeReg(uint16_t reg, uint8_t value)
{
	SX128X_writeRegs(reg,&value,1);
}

uint8_t SX128X_readReg(uint16_t reg)
{
	uint8_t value;
	
	SX128X_readRegs(reg,&value,1);
	return value;
}


void SX128X_writeBuffer(uint8_t offset, uint8_t *data, uint8_t length)
{
	uint16_t i;
	
	SX128X_checkBusy();
  SX128X_select();
	radiospi_transfer(SX128X_WRITE_BUFFER);
	radiospi_transfer(offset);
	for(i = 0; i < length;i++) radiospi_transfer(data[i]);
	SX128X_deselect();
	SX128X_checkBusy();
}

void SX128X_readBuffer(uint8_t offset, uint8_t *data, uint8_t length)
{
	uint16_t i;
	
	SX128X_checkBusy();
  SX128X_select();

	radiospi_transfer(SX128X_READ_BUFFER);
	radiospi_transfer(offset);
	radiospi_transfer(0xFF);
	for(i = 0;i < length; i++) data[i] = radiospi_transfer(0xff);
	SX128X_deselect();
	SX128X_checkBusy();
}




