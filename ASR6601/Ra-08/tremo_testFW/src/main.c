#include "bsp.h"
#include "radio_proc.h"

int main(void)
{
	init_power_clk();
	init_peripherals();
	radio_init();
	while(1)
	{
		radio_proc();
	}
}
