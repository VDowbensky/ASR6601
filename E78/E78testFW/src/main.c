#include "bsp.h"
#include "radio_proc.h"
#include "app_cli.h"

int main(void)
{
	init_power_clk();
	init_peripherals();
	delay_ms(100);
	printf("\r\nRA08 test\r\n");
	led_on();
	radio_init();
	delay_ms(100);
	led_off();
	cli_init();
	
	while(1)
	{
		radio_proc();
		cli_proc();
	}
}
