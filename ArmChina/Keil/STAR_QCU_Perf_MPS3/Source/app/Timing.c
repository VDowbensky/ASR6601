/**************************************************************************//**
 * @file     Timing.c
 * @brief    CMSIS ArmChina STAR Timing Template C File for Cycle Measurement of Functions
 * @version  V1.0.0
 * @date     20. January 2022
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

#include "RTE_Components.h"
#include CMSIS_device_header
#include "Timing.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/* Macros declaration */
#define SYSTICK_INITIAL_VALUE       0xFFFFFF

volatile uint32_t SysTickCounter = 0;


/**
  \fn          void CycleCount_Init (void)
  \brief       Cycle counter initialization.
  \param[in]   none
  \return      none
*/
void CycleCount_Init(void)
{
    SysTick->LOAD = SYSTICK_INITIAL_VALUE;
    SysTick->VAL = 0;
    SysTick->CTRL = 0;
}

/**
  \fn          void CycleCount_Start (void)
  \brief       Start cycle counter and enable System Tick interrupt.
  \param[in]   none
  \return      none
*/
void CycleCount_Start(void)
{
    SysTick->CTRL = 0;
    SysTick->LOAD = SYSTICK_INITIAL_VALUE;
    SysTick->VAL = SYSTICK_INITIAL_VALUE;
    SysTickCounter = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}

/**
  \fn          void CycleCount_Stop (void)
  \brief       Stop cycle counter.
  \param[in]   none
  \return      none
*/
void CycleCount_Stop(void)
{
    SysTick->CTRL = 0;
    SysTick->LOAD = SYSTICK_INITIAL_VALUE;
}

/**
  \fn          uint32_t CycleCount_Get (void)
  \brief       Get cycle count.
  \param[in]   none
  \return      spent cycle count.
*/
uint32_t CycleCount_Get(void)
{
    uint32_t val = SysTick->VAL;
    uint32_t count;
    count = 0x00FFFFFFUL*(1+SysTickCounter) - val;
    return (count);
}

/**
  \fn          void SysTick_Handler (void)
  \brief       System Tick handler function for cycle count.
  \param[in]   none
  \return      none
*/
void SysTick_Handler(void) 
{
    SysTickCounter++;
}

#ifdef  __cplusplus
}
#endif
