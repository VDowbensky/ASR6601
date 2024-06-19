/**************************************************************************//**
 * @file     Flash_api.h
 * @brief    CMSIS ArmChina STAR Flash API Header File
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

#ifndef FLASH_API_H_
#define FLASH_API_H_

#include "QCU_common.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
\brief Flash JEDEC-ID structure definition 
*/
typedef struct {
    uint8_t manu_id;  /* Manufacturer ID */
    uint8_t dev_t;    /* Device Type */
    uint8_t dev_id;   /* Device ID */
} JEDEC_ID;

/* Extern function declaration */
/**
  \fn          void Api_Init (void)
  \brief       Initialize the control register of QCU according to the device.
*/
extern void Api_Init(void);

/**
  \fn          uint8_t Api_ReadSR (bool xip_mode)
  \brief       Read the status register of flash device.
  \param[in]   qio_mode  Indicate whether using quad lines command mode.
  \return      \ref The value of the status register.
*/
extern uint8_t Api_ReadSR(bool qio_mode);

/**
  \fn          uint8_t Api_ReadCR (void)
  \brief       Read the configuration register of flash device.
  \return      \ref The value of the configuration register.
*/
extern uint8_t Api_ReadCR(void);

/**
  \fn          int32_t Api_ReadID (JEDEC_ID *id)
  \brief       Read the identification of flash device.
  \param[in]   id  Pointer to a struct storing the flash JEDEC-ID data.
  \return      \ref execution_status
*/
extern int32_t Api_ReadID(JEDEC_ID *id);

/**
  \fn          int32_t Api_ReadMemInd (uint32_t adr, uint32_t sz, uint8_t *buf)
  \brief       Read the flash device memory using Indirect read mode.
  \param[in]   adr  read memory address.
  \param[in]   sz  Length of data buffer.
  \param[in]   buf  Pointer to a buffer storing the read data.
  \return      \ref execution_status
*/
extern int32_t Api_ReadMemInd(uint32_t adr, uint32_t sz, uint8_t *buf);

/**
  \fn          int32_t Api_WriteEn (void)
  \brief       Configure the Write-Enable bit of flash to allow write operations to occur.
  \return      \ref execution_status
*/
extern int32_t Api_WriteEn(void);

/**
  \fn          int32_t Api_WaitBusy (bool qio_mode)
  \brief       Wait an internal Erase or Program operation of flash to complete.
  \param[in]   qio_mode  Indicate whether using quad lines command mode.
  \return      \ref execution_status
*/
extern int32_t Api_WaitBusy(bool qio_mode);

/**
  \fn          void Api_EraseChip (void)
  \brief       Erase complete flash.
*/
extern void Api_EraseChip(void);

/**
  \fn          void Api_EraseSector (uint32_t adr, bool qio_mode)
  \brief       Erase flash sector.
  \param[in]   adr  Sector address.
  \param[in]   qio_mode  Indicate whether using quad lines command mode.
*/
extern void Api_EraseSector(uint32_t adr, bool qio_mode);

/**
  \fn          void Api_ProgramPage (uint32_t adr, uint32_t sz, uint8_t *buf, bool qio_mode)
  \brief       Program up to 256 bytes of data in the flash memory.
  \param[in]   adr  Program address.
  \param[in]   sz  Length of data buffer.
  \param[in]   buf  Pointer to a buffer storing the program data.
  \param[in]   qio_mode  Indicate whether using quad lines command mode.
*/
extern void Api_ProgramPage(uint32_t adr, uint32_t sz, uint8_t *buf, bool qio_mode);

/**
  \fn          int32_t Api_EnSQIOR_XIP (void)
  \brief       Enter SPI Quad I/O Read with XIP mode.
  \return      \ref execution_status
*/
extern int32_t Api_EnSQIOR_XIP(void);

/**
  \fn          int32_t Api_RstSQIOR_XIP (void)
  \brief       Reset QCU in SPI Quad I/O Read with XIP mode to single line SPI mode.
  \return      \ref execution_status
*/
extern int32_t Api_RstSQIOR_XIP(void);

/**
  \fn          int32_t Api_EnHiSpd_XIP (void)
  \brief       Enter quad lines High-Speed Read with XIP mode.
  \return      \ref execution_status
*/
extern int32_t Api_EnHiSpd_XIP(void);

/**
  \fn          int32_t Api_RstHiSpd_XIP (void)
  \brief       Reset QCU in quad lines High-Speed Read with XIP mode to single line SPI mode.
  \return      \ref execution_status
  \note        Exit(RSTQIO) twice from quad lines High-Speed read with XIP. The first RSTQIO command will only
               return the device to a state where it can accept new command instruction. An additional RSTQIO
               is required to reset the device to SPI mode.
*/
extern int32_t Api_RstHiSpd_XIP(void);

#ifdef  __cplusplus
}
#endif

#endif /* FLASH_API_H_ */
