/**************************************************************************//**
 * @file     UART_APB.c
 * @brief    CMSIS ArmChina STAR APB UART Template C File
 * @version  V1.0.0
 * @date     28. January 2021
 ******************************************************************************/
/*
 * Copyright (c) 2018-2021 ArmChina. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>

#include "UART_APB.h"


/* Initialize the UART. 
*  Set Baud rate and enable TX and RX
*  Execute the Initialization before using UART
*/
void APB_UART_Initialize(void)
{
    WR_REG32(APB_UART_CTRL, 0);
	WR_REG32(APB_UART_BAUDIV, BAUD_DIV);
	WR_REG32(APB_UART_CTRL, APB_UART_CTRL_TXEN | APB_UART_CTRL_RXEN);
}


/* The UART in the STAR FPGA reference design provides Transmit Holding Register. 
*  Use this INLINE function to check the status of this register before TX a char
*/
static inline void APB_UART_WaitForSpace(void)
{
    while((RD_REG32(APB_UART_STATUS)) & APB_UART_STATUS_TXFULL);
}


/* Use this INLINE function to check the status of the Transmit Holding Register if data received.   
*/
static inline void APB_UART_WaitForData(void)
{
    while(((RD_REG32(APB_UART_STATUS)) & APB_UART_STATUS_RXEMPTY) == 0);
}


/* Output a char via the UART. 
*  To work with toolchain to implement printf fuction, 
*  note that the type of the return argument should match that of the lowlevel function of printf in the library
*/
unsigned char APB_UART_PutChar(unsigned char out_char)
{
 
  APB_UART_WaitForSpace(); // Wait if Transmit Holding register is full

  if (out_char == '\n')
  {
	  WR_REG32(APB_UART_DATA, '\r');
	  APB_UART_WaitForSpace(); // Wait if Transmit Holding register is full
  }

  WR_REG32(APB_UART_DATA, out_char); // write to transmit holding register

  return out_char;
}


/* Reveive a char via the UART. 
*/
unsigned char APB_UART_GetChar(void)
{
  unsigned char in_char;

  APB_UART_WaitForData();

  in_char = (unsigned char) (RD_REG32(APB_UART_DATA));

  //Convert CR to LF
  if(in_char == '\r')
  {
	  in_char = '\n';
  }
  return in_char;
}


