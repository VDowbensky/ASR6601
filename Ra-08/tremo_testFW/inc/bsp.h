#ifndef _BSP_H_
#define _BSP_H_

#include <stdio.h>
#include <string.h>
#include "tremo_system.h"
#include "tremo_rcc.h"
#include "tremo_flash.h"
#include "tremo_gpio.h"
#include "tremo_uart.h"
#include "tremo_adc.h"
#include "tremo_delay.h"
#include "retargetserial.h"
#include "aes_proc.h"

#define HW_VERSION    1
#define FW_VERSION		0
#define FW_REVISION		1

#define TXD0_PORT			GPIOB
#define TXD0_PIN			GPIO_PIN_1
#define RXD0_PORT			GPIOB
#define RXD0_PIN			GPIO_PIN_0

#define LPRXD_PORT		GPIOD //probably for GPS
#define LPRXD_PIN			GPIO_PIN_12

#define TXD1_PORT			GPIOA
#define TXD1_PIN			GPIO_PIN_5
#define RXD1_PORT			GPIOA
#define RXD1_PIN			GPIO_PIN_4

#define TXD2_PORT			GPIOA
#define TXD2_PIN			GPIO_PIN_9
#define RXD2_PORT			GPIOA
#define RXD2_PIN			GPIO_PIN_8

#define SCL0_PORT			GPIOA
#define SCL0_PIN			GPIO_PIN_14
#define SDA0_PORT			GPIOA
#define SDA0_PIN			GPIO_PIN_15

#define LED_PORT			GPIOA
#define LED_PIN				GPIO_PIN_8

#define K0_PORT				GPIOA
#define K0_PIN				GPIO_PIN_11
#define K1_PORT				GPIOA
#define K1_PIN				GPIO_PIN_9

#define RFSW_CTRL_PORT	GPIOA
#define RFSW_CTRL_PIN		GPIO_PIN_10 //GPIO10
#define RFSW_TXRX_PORT	GPIOD
#define RFSW_TXRX_PIN		GPIO_PIN_11 //GPIO59

#define UART0_BR			115200
#define AES_MODE			AES_ECB_MODE
//#define AES_MODE    	AES_CBC_MODE
//#define AES_MODE			AES_OFB_MODE
//#define AES_MODE			AES_CTR_MODE
//#define AES_KEYLEN		AES_128
//#define AES_KEYLEN		AES_192
#define AES_KEYLEN		AES_256

void init_power_clk(void);
void init_peripherals(void);
void led_on(void);
void led_off(void);

#endif
