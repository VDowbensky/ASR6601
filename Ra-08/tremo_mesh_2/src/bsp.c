#include "bsp.h"

#include "gpio.h"
#include "uart.h"
#include "adc.h"
#include "i2c.h"
#include "ustimer.h"
#include "retargetserial.h"
#include "ssd1306.h"
#include "test.h"

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
	//auxuart_init(UART1_BR);
	//radiospi_init();
	ustimer_init();
}

void led_on(void)
{
	gpio_write(LED_PORT, LED_PIN,GPIO_LEVEL_LOW);
}

void led_off(void)
{
	gpio_write(LED_PORT, LED_PIN,GPIO_LEVEL_HIGH);
}

void oled_test(void)
{
	delay_ms(1000);
	TEST_MainPage();         //主界面显示测试
	SSD1306_Clear(0); 
	Test_Color();            //刷屏测试
	SSD1306_Clear(0); 
	Test_Rectangular();      //矩形绘制测试
	SSD1306_Clear(0); 
	Test_Circle();           //圆形绘制测试
	SSD1306_Clear(0); 
	Test_Triangle();         //三角形绘制测试
	SSD1306_Clear(0);  
	TEST_English();          //英文显示测试
	SSD1306_Clear(0); 
	TEST_Number_Character(); //数字和符号显示测试
	SSD1306_Clear(0); 
	TEST_BMP();              //BMP单色图片显示测试
	SSD1306_Clear(0); 
	TEST_Menu1();            //菜单1显示测试
	SSD1306_Clear(0); 
	TEST_Menu2();            //菜单2显示测试
	SSD1306_Clear(0); 
	printf("LCD test done.\r\n");
}

