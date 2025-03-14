#include "bsp.h"

#include "gpio.h"
#include "uart.h"
#include "adc.h"
#include "i2c.h"
#include "ustimer.h"
#include "retargetserial.h"

void init_power_clk(void)
{
	rcc_enable_oscillator(RCC_OSC_XO32M, true);
	rcc_set_sys_clk_source(RCC_SYS_CLK_SOURCE_XO32M);
	rcc_set_hclk_div(RCC_HCLK_DIV_1);
	rcc_set_pclk_div(RCC_PCLK0_DIV_1,RCC_PCLK1_DIV_1);
	rcc_set_systick_source(RCC_SYSTICK_SOURCE_HCLK);
	rcc_enable_peripheral_clk(RCC_PERIPHERAL_SYSCFG,true);
	//rcc_enable_peripheral_clk(RCC_PERIPHERAL_PWR, true);
	
	delay_init();
}

void init_peripherals(void)
{
	mygpio_init();
	myadc_init();
	RETARGET_SerialInit();
	i2c0_init();
	//auxuart_init(UART1_BR);
	ustimer_init();
}

void led_on(void)
{
	//gpio_write(LED_PORT, LED_PIN,GPIO_LEVEL_LOW); //this function not working!
	gpio_init(LED_PORT, LED_PIN, GPIO_MODE_OUTPUT_PP_LOW);
}

void led_off(void)
{
	//gpio_write(LED_PORT, LED_PIN,GPIO_LEVEL_HIGH);
	gpio_init(LED_PORT, LED_PIN, GPIO_MODE_OUTPUT_PP_HIGH);
}

