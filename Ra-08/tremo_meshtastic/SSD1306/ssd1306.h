#ifndef _SSD1306_H_
#define _SSD1306_H_

#include "bsp.h"
#include "ssd1306_interface.h"

//--------------OLED parameters definition---------------------
#define PAGE_SIZE    	8
#define XLevelL				0x00
#define XLevelH				0x10
#define YLevel       	0xB0
#define	Brightness	 	0xFF 
#define WIDTH 	     	128
#define HEIGHT 	     	64	

//-------------Write command and data definition-------------------
#define SSD1306_CMD     0	//Write Command
#define SSD1306_DATA    1	//Write Data
   						  
//-----------------OLED operation definitions---------------- 

void SSD1306_Display_On(void);
void SSD1306_Display_Off(void);
void SSD1306_Set_Pos(uint8_t x, uint8_t y);
void SSD1306_Init_GPIO(void);	   							   		    
void SSD1306_Init(void);
void SSD1306_Set_Pixel(uint8_t x, uint8_t y,uint8_t color);
void SSD1306_Display(void);
void SSD1306_Clear(uint8_t dat);  

#endif
