/**************************************************************************//**
 * @file     Timing.h
 * @brief    CMSIS ArmChina STAR Timing Header File
 * @version  V1.0.0: The initial version to support CMSIS ArmChina STAR examples.
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

#ifndef __TIMING_H_
#define __TIMING_H_

#include <stdint.h>

#ifdef  __cplusplus
extern "C"
{
#endif

/* Extern function declaration */
/**
  \fn          void CycleCount_Init (void)
  \brief       Cycle counter initialization.
  \param[in]   none
  \return      none
*/
extern void CycleCount_Init();

/**
  \fn          void CycleCount_Start (void)
  \brief       Start cycle counter and enable System Tick interrupt.
  \param[in]   none
  \return      none
*/
extern void CycleCount_Start();

/**
  \fn          void CycleCount_Stop (void)
  \brief       Stop cycle counter.
  \param[in]   none
  \return      none
*/
extern void CycleCount_Stop();

/**
  \fn          uint32_t CycleCount_Get (void)
  \brief       Get cycle count.
  \param[in]   none
  \return      spent cycle count.
*/
extern uint32_t CycleCount_Get();

/* Macro definition */
#define TIMING_INIT()   CycleCount_Init()
#define TIMING_START()  CycleCount_Start()
#define TIMING_GET()    CycleCount_Get()
#define TIMING_STOP()   CycleCount_Stop()

#ifdef  __cplusplus
}
#endif

#endif /* __TIMING_H_ */

/** End of Files **/
