#include "bsp.h"
#include "radio.h"
#include "app_cli.h"
#include "gui.h"
#include "test.h"

int main(void)
{
	init_power_clk();
	init_peripherals();
	delay_ms(100);
	printf("\r\nRA08 test\r\n");
	led_on();
	SSD1306_Init();
	SSD1306_Clear(0);
	radio_config();
	delay_ms(100);
	led_off();
	cli_init();
	//oled_test();
	//GUI_ShowString(0,0,"Hello, Lamer!",16,1);
	//GUI_ShowString(0,16,"RA08 test unit",16,1);
	//updatescreen();
	//GUI_DrawLine(64, 64, 64, 16,1);
	
	while(1)
	{
		radio_proc();
		cli_proc();
	}
}
