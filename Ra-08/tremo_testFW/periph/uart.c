#include "uart.h"

void myuart_init(uint32_t br)
{
	gpio_set_iomux(TXD0_PORT, TXD0_PIN, 1);
	gpio_set_iomux(RXD0_PORT, RXD0_PIN, 1);
	
	rcc_set_uart0_clk_source(RCC_UART0_CLK_SOURCE_PCLK0);
	rcc_enable_peripheral_clk(RCC_PERIPHERAL_UART0,true);

	/* uart config struct init */
	uart_config_t uart_config;
	uart_config_init(&uart_config);

	uart_config.baudrate = UART_BAUDRATE_115200;
	uart_init(UART0, &uart_config);
	uart_cmd(UART0, ENABLE);
	
	uart_config_interrupt(UART0,UART_INTERRUPT_RX_DONE,true);
	
	NVIC_ClearPendingIRQ(UART0_IRQn);
	NVIC_EnableIRQ(UART0_IRQn);
}

void UART0_IRQHandler(void)
{
	cbRETARGET_Rx();
}