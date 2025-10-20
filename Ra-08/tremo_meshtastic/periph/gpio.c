#include "gpio.h"

void mygpio_init(void)
{	
	rcc_enable_peripheral_clk(RCC_PERIPHERAL_GPIOA, true);
	rcc_enable_peripheral_clk(RCC_PERIPHERAL_GPIOB, true);
	rcc_enable_peripheral_clk(RCC_PERIPHERAL_GPIOC, true);
	rcc_enable_peripheral_clk(RCC_PERIPHERAL_GPIOD, true);
	
	//Keys
	gpio_init(K0_PORT, K0_PIN, GPIO_MODE_INPUT_PULL_UP);
	gpio_init(K1_PORT, K1_PIN, GPIO_MODE_INPUT_PULL_UP);
	//LED
	gpio_init(LED_PORT, LED_PIN, GPIO_MODE_OUTPUT_PP_HIGH);
	gpio_config_drive_capability(LED_PORT, LED_PIN,GPIO_DRIVE_CAPABILITY_8MA);
}