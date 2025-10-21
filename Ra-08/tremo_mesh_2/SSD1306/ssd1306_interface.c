#include "ssd1306_interface.h"

char strbuffer[64];

#if SSD1306_INTERFACE_SOFT_SPI

void SSD1306_interface_init(void)
{
	gpio_init(SSD1306_CS_PORT, SSD1306_CS_PIN, GPIO_MODE_OUTPUT_PP_HIGH);
	gpio_init(SSD1306_SCK_PORT, SSD1306_SCK_PIN, GPIO_MODE_OUTPUT_PP_LOW);
	gpio_init(SSD1306_MOSI_PORT, SSD1306_MOSI_PIN, GPIO_MODE_OUTPUT_PP_LOW);
	gpio_init(SSD1306_RST_PORT, SSD1306_RST_PIN, GPIO_MODE_OUTPUT_PP_HIGH);
	gpio_init(SSD1306_DC_PORT, SSD1306_DC_PIN, GPIO_MODE_OUTPUT_PP_LOW);
}

void SSD1306_Select(void)
{
	gpio_write(SSD1306_CS_PORT, SSD1306_CS_PIN,GPIO_LEVEL_LOW);
}

void SSD1306_Deselect(void)
{
	gpio_write(SSD1306_CS_PORT, SSD1306_CS_PIN,GPIO_LEVEL_HIGH);
}

void SSD1306_WriteByte(uint8_t b)
{
	uint8_t i;
	
	for(i = 8;i > 0;i--)
	{
		if(b & 0x80) gpio_write(SSD1306_MOSI_PORT, SSD1306_MOSI_PIN,GPIO_LEVEL_HIGH);
		else gpio_write(SSD1306_MOSI_PORT, SSD1306_MOSI_PIN,GPIO_LEVEL_LOW);
		gpio_write(SSD1306_SCK_PORT, SSD1306_SCK_PIN,GPIO_LEVEL_LOW);
		gpio_write(SSD1306_SCK_PORT, SSD1306_SCK_PIN,GPIO_LEVEL_HIGH);
    b <<= 1; 
	}
}

void SSD1306_Command(bool cmd)
{
	if(cmd) gpio_write(SSD1306_DC_PORT, SSD1306_DC_PIN,GPIO_LEVEL_HIGH);
	else gpio_write(SSD1306_DC_PORT, SSD1306_DC_PIN,GPIO_LEVEL_LOW);
}

void SSD1306_Reset(void)
{
	gpio_write(SSD1306_RST_PORT, SSD1306_RST_PIN,GPIO_LEVEL_HIGH);
	delay_ms(100);
	gpio_write(SSD1306_RST_PORT, SSD1306_RST_PIN,GPIO_LEVEL_LOW);
	delay_ms(100);
	gpio_write(SSD1306_RST_PORT, SSD1306_RST_PIN,GPIO_LEVEL_HIGH);	
}

void SSD1306_WR_Byte(uint8_t dat,uint8_t cmd)
{
	if(cmd) SSD1306_Command(true);
	else SSD1306_Command(false);
	SSD1306_Select();
	SSD1306_WriteByte(dat);
	SSD1306_Deselect();
}

#elif SSD1306_INTERFACE_HARD_SPI

#include "spi.h"

void SSD1306_interface_init(void)
{
	gpio_init(SSD1306_CS_PORT, SSD1306_CS_PIN, GPIO_MODE_OUTPUT_PP_HIGH);
	gpio_init(SSD1306_RST_PORT, SSD1306_RST_PIN, GPIO_MODE_OUTPUT_PP_HIGH);
	gpio_init(SSD1306_DC_PORT, SSD1306_DC_PIN, GPIO_MODE_OUTPUT_PP_LOW);
	gpio_set_iomux(SSD1306_SCK_PORT, SSD1306_SCK_PIN, 4);
	gpio_set_iomux(SSD1306_MOSI_PORT, SSD1306_MOSI_PIN, 4);
	//init SPI module
	ssp_init_t init_struct;
	rcc_enable_peripheral_clk(RCC_PERIPHERAL_SSP1, true);
	//ssp_init_struct(&init_struct);
	init_struct.ssp_sclk      = 2000000; // max. 3.3 MHz
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

void SSD1306_Select(void)
{
	gpio_write(SSD1306_CS_PORT, SSD1306_CS_PIN,GPIO_LEVEL_LOW);
}

void SSD1306_Deselect(void)
{
	gpio_write(SSD1306_CS_PORT, SSD1306_CS_PIN,GPIO_LEVEL_HIGH);
}

void SSD1306_WriteByte(uint8_t b)
{
	spi_transfer(b);
}

void SSD1306_Command(bool cmd)
{
	if(cmd) gpio_write(SSD1306_DC_PORT, SSD1306_DC_PIN,GPIO_LEVEL_HIGH);
	else gpio_write(SSD1306_DC_PORT, SSD1306_DC_PIN,GPIO_LEVEL_LOW);
}

void SSD1306_Reset(void)
{
	gpio_write(SSD1306_RST_PORT, SSD1306_RST_PIN,GPIO_LEVEL_HIGH);
	delay_ms(100);
	gpio_write(SSD1306_RST_PORT, SSD1306_RST_PIN,GPIO_LEVEL_LOW);
	delay_ms(100);
	gpio_write(SSD1306_RST_PORT, SSD1306_RST_PIN,GPIO_LEVEL_HIGH);	
}
#else //I2c

#include "i2c.h"

void SSD1306_interface_init(void)
{
	i2c0_init();
}

void SSD1306_Select(void)
{
	//nothing to do in I2C mode
}

void SSD1306_Deselect(void)
{
	//nothing to do in I2C mode
}

void SSD1306_WriteByte(uint8_t b)
{
	//not used in I2C mode
}

void SSD1306_Command(bool cmd)
{
	//nothing to do in I2C mode
}

void SSD1306_Reset(void)
{
	//nothing to do in I2C mode
}

void SSD1306_WR_Byte(uint8_t dat,uint8_t cmd)
{
	uint8_t buf[2];
	if(cmd) buf[0] = 0;
	else buf[0] = 0x40;
	buf[1] = dat;
	i2c0_write(SSD1306_ADDR, buf, 2);
}

#endif