#include "bsp.h"
#include "radio_proc.h"

int main(void)
{
	init_power_clk();
	init_peripherals();
	printf("RA08 test\r\n");
	led_on();
	radio_init();
	delay_ms(100);
	led_off();
	printf("Radio init OK\r\n");
	while(1)
	{
		radio_proc();
	}
}
