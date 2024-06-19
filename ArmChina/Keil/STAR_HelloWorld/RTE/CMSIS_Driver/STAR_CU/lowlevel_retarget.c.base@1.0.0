/**************************************************************************//**
 * @file     lowlevel_retarget.c
 * @brief    CMSIS ArmChina STAR low level retarget interface with standard IO, peripherals
 * @version  V1.0.0
 * @date     10. March 2021
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

//#include <stdio.h>
#include "RTE_Components.h"
#include "UART_APB.h"


void UartStdOutInit(void);

#ifdef RTE_Compiler_IO_STDOUT_User     /* Compiler I/O: STDOUT User */
int stdout_putchar (int ch);
#endif

#ifdef RTE_Compiler_IO_STDIN_User      /* Compiler I/O: STDIN User */
int stdin_getchar (void);
#endif

int MyLowLevelPutchar(int x);

int _write(int file, char *ptr, int len);


void UartStdOutInit (void)
{	
    APB_UART_Initialize();
}

#ifdef RTE_Compiler_IO_STDOUT_User     /* Compiler I/O: STDOUT User */
int stdout_putchar (int ch) 
{ 
    return ((int) APB_UART_PutChar((char) ch));
}
#endif

#ifdef RTE_Compiler_IO_STDIN_User      /* Compiler I/O: STDIN User */
int stdin_getchar (void)
{
    return ((int) APB_UART_GetChar());
}
#endif

int MyLowLevelPutchar(int x)
{
    return ((int) APB_UART_PutChar((unsigned char) x)); 
} 

int _write(int file, char *ptr, int len)
{
    int i;
    (void) file;
    for (i = 0; i < len; i++)
    {
        APB_UART_PutChar(*ptr++);
    }
    return len;
}

