/**************************************************************************//**
 * @file     Flash_app.h
 * @brief    CMSIS ArmChina STAR Flash Application Header File
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

#ifndef FLASH_APP_H_
#define FLASH_APP_H_

#include "Flash_api.h"
#include "QCU_common.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
\brief Flash information structure definition 
*/
typedef struct {
    uint8_t status_reg;  /* Flash status register */
    uint8_t config_reg;  /* Flash configuration register */
    JEDEC_ID jedec_id;   /* Flash JEDEC ID */
} FLASH_INFORMATION;

/* Extern function declaration */
/**
  \fn          void App_Init (void)
  \brief       Application initialization.
*/
extern void App_Init(void);

/**
  \fn          int32_t App_UnInit (void)
  \brief       Application reset, enter SQI mode with XIP.
  \return      \ref execution_status
*/
extern int32_t App_UnInit(void);

/**
  \fn          int32_t App_EraseChip (void)
  \brief       Erase complete flash.
  \return      \ref execution_status
*/
extern int32_t App_EraseChip(void);

/**
  \fn          int32_t App_EraseSector (uint32_t adr)
  \brief       Erase flash sector.
  \param[in]   adr  Sector address.
  \return      \ref execution_status
*/
extern int32_t App_EraseSector(uint32_t adr);

/**
  \fn          int32_t App_ProgramPage (uint32_t adr, uint32_t sz, uint8_t *buf)
  \brief       Page program data in the flash memory.
  \param[in]   adr  Program address.
  \param[in]   sz  Length of data buffer.
  \param[in]   buf  Pointer to a buffer storing the program data.
  \return      \ref execution_status
*/
extern int32_t App_ProgramPage(uint32_t adr, uint32_t sz, uint8_t *buf);

/**
  \fn          int32_t App_CheckFlash (FLASH_INFORMATION *infor)
  \brief       Get status, configuration register, and identification of flash.
  \param[in]   infor  Pointer to a struct storing the flash information data.
  \return      \ref execution_status
*/
extern int32_t App_CheckFlash(FLASH_INFORMATION *infor);

/**
  \fn          int32_t App_TestIndRd (uint32_t adr, uint32_t sz, uint8_t *buf)
  \brief       Using indirect read mode of QCU to read the flash device memory.
  \param[in]   adr  Read address.
  \param[in]   sz  Length of data buffer.
  \param[in]   buf  Pointer to a buffer storing the read data.
  \return      \ref execution_status
*/
extern int32_t App_TestIndRd(uint32_t adr, uint32_t sz, uint8_t *buf);

#ifdef  __cplusplus
}
#endif

#endif /* FLASH_APP_H_ */
