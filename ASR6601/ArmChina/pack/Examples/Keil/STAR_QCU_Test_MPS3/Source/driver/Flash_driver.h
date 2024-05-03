/**************************************************************************//**
 * @file     Flash_driver.h
 * @brief    CMSIS ArmChina STAR Flash Driver Header File
 * @version  V1.0.0: The initial version to support STAR work with Flash on MPS3.
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

#ifndef FLASH_DRIVER_H_
#define FLASH_DRIVER_H_

#include "QCU_common.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/* Macros declaration */
/* Change macros according to your device */
#define BUSY_BIT_POS 0x01
#define WEL_BIT_POS  0x02
#define IOC_BIT_POS  0x02
#define BUSY_TIMEOUT 0xFFFF
#define FLASH_ADDR_MAX 0x800000

/* The order of enum must be the same as Instr_List */
typedef enum {
    WRITE_ENABLE = 0,
    WRITE_ENABLE_Q,      /* _Q means using QIO mode */
    ERASE_CHIP,
    ERASE_SECTOR,
    ERASE_SECTOR_Q,
    PROGRAM_PAGE,
    PROGRAM_PAGE_Q,
    WRITE_DISABLE,
    WRITE_CR,
    ENA_QIO,
    RST_QIO,
    RST_QIO_Q,

    INDIRECT_WRITE_CMD_MAX
} Indirect_Write_CMD;

typedef enum {
    READ_SR1 = INDIRECT_WRITE_CMD_MAX,
    READ_SR1_Q,
    READ_CR,
    READ_ID,
    READ_IND,
    SDOR_IND,
    SQIOR_IND,

    INDIRECT_READ_CMD_MAX
} Indirect_Read_CMD;

typedef enum {
    READ_DIR = INDIRECT_READ_CMD_MAX,
    SDIOR_DIR,
    SQIOR_DIR,
    HSREAD_Q_DIR,

    DIRECT_READ_CMD_MAX
} Direct_Read_CMD;
/* The order of enum must be the same as Instr_List */

/* Extern function declaration */
/**
  \fn          void Flash_InitParameter (QCU_Init_TypeDef *param)
  \brief       Get initialization parameters of QCU and command max value according to flash device.
  \param[in]   param  Pointer to QCU initialization parameters.
*/
extern void Flash_InitParameter(QCU_Init_TypeDef *param);

/**
  \fn          void Flash_GetParameter (uint8_t cmd, bool xip_mode, QCU_Comm_Typedef *instr)
  \brief       Get communication parameters of QCU and flash device.
  \param[in]   cmd  Flash operation command.
  \param[in]   xip_mode  Whether enter XIP mode.
  \param[in]   instr  Pointer to QCU communication instruction.
*/
extern void Flash_GetParameter(uint8_t cmd, bool xip_mode, QCU_Comm_Typedef *instr);


#ifdef  __cplusplus
}
#endif

#endif /* FLASH_DRIVER_H_ */
