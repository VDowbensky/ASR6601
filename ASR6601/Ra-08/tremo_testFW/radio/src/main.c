#include <stdio.h>
#include <string.h>
#include "delay.h"
#include "timer.h"
#include "radio.h"
#include "tremo_uart.h"
#include "tremo_lpuart.h"
#include "tremo_gpio.h"
#include "tremo_rcc.h"
#include "tremo_pwr.h"
#include "tremo_delay.h"
#include "rtc-board.h"
#include "lora_config.h"

extern int tc_lora_test(void);


void uart_log_init()
{
    lpuart_init_t lpuart_init_cofig;
    uart_config_t uart_config;

    // set iomux
    gpio_set_iomux(GPIOD, GPIO_PIN_12, 2); // LPUART_RX:GP60
    gpio_set_iomux(GPIOB, GPIO_PIN_1, 1);  // UART0_TX:GP17

    // lpuart init
    lpuart_deinit(LPUART);
    rcc_enable_peripheral_clk(RCC_PERIPHERAL_LPUART, true);
    lpuart_init_cofig.baudrate         = 9600;
    lpuart_init_cofig.data_width       = LPUART_DATA_8BIT;
    lpuart_init_cofig.parity           = LPUART_PARITY_NONE;
    lpuart_init_cofig.stop_bits        = LPUART_STOP_1BIT;
    lpuart_init_cofig.low_level_wakeup = false;
    lpuart_init_cofig.start_wakeup     = false;
    lpuart_init_cofig.rx_done_wakeup   = true;
    lpuart_init(LPUART, &lpuart_init_cofig);

    lpuart_config_interrupt(LPUART, LPUART_CR1_RX_DONE_INT, ENABLE);
    lpuart_config_tx(LPUART, false);
    lpuart_config_rx(LPUART, true);

    NVIC_SetPriority(LPUART_IRQn, 2);
    // NVIC_EnableIRQ(LPUART_IRQn);

    // uart init
    uart_config_init(&uart_config);
    uart_config.fifo_mode = ENABLE;
    uart_config.mode     = UART_MODE_TX;
    uart_config.baudrate = 9600;
    uart_init(CONFIG_DEBUG_UART, &uart_config);

    uart_cmd(CONFIG_DEBUG_UART, ENABLE);
}

void board_init()
{
    rcc_enable_oscillator(RCC_OSC_XO32K, true);

    rcc_enable_peripheral_clk(RCC_PERIPHERAL_UART0, true);
    rcc_enable_peripheral_clk(RCC_PERIPHERAL_LPUART, true);
    rcc_enable_peripheral_clk(RCC_PERIPHERAL_GPIOA, true);
    rcc_enable_peripheral_clk(RCC_PERIPHERAL_GPIOB, true);
	rcc_enable_peripheral_clk(RCC_PERIPHERAL_GPIOC, true);
    rcc_enable_peripheral_clk(RCC_PERIPHERAL_GPIOD, true);
    rcc_enable_peripheral_clk(RCC_PERIPHERAL_PWR, true);
    rcc_enable_peripheral_clk(RCC_PERIPHERAL_RTC, true);
    rcc_enable_peripheral_clk(RCC_PERIPHERAL_LORA, true);
    rcc_enable_peripheral_clk(RCC_PERIPHERAL_SAC, true);
    
    delay_ms(100);
    pwr_xo32k_lpm_cmd(true);
    
    

    RtcInit();
	gpio_set_iomux(GPIOA,GPIO_PIN_6,0);
	gpio_set_iomux(GPIOA,GPIO_PIN_7,0);
    uart_log_init();
}

int main(void)
{
    // Target board initialization
    board_init();
		printf("\r\n"
            "*************************************************************************\r\n"
            "*************************************************************************\r\n"
            "                         ASR6601 LoRa Test\r\n"
            "          fix RF switch with ANT_SW_CTRL and IO47(C15)\r\n"
            "  version:%s(%s-%s)\r\n"
            "  Available commands are:\r\n"
            "  AT+CTX=<freq>,<data_rate>,<bandwidth>,<code_rate>,<pwr>,<iqconverted>\r\n"
            "  AT+CRX=<freq>,<data_rate>,<bandwidth>,<code_rate>,<iqconverted>\r\n"
            "  AT+CRXS=<freq>,<data_rate>,<bandwidth>,<code_rate>,<iqconverted>\r\n"
            "  AT+CADDRSET=<local_addr>\r\n"
            "  AT+CTXADDRSET=<target_addr>\r\n"
            "  AT+CSLEEP=<sleep_mode>\r\n"
            "*************************************************************************\r\n"
            "*************************************************************************\r\n",LORA_TEST_FIRMWARE_VERSION,__DATE__,__TIME__);
    tc_lora_test();
}

#ifdef USE_FULL_ASSERT
void assert_failed(void* file, uint32_t line)
{
    (void)file;
    (void)line;

    while (1) { }
}
#endif