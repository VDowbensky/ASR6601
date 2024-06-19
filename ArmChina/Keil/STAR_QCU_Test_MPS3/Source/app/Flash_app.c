/**************************************************************************//**
 * @file     Flash_app.c
 * @brief    CMSIS ArmChina STAR Flash Application Template C File
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

#include "Flash_driver.h"
#include "Flash_app.h"

/*** Application for flash programming algorithm using in Keil-MDK ***/
/**
  \fn          void App_Init (void)
  \brief       Application initialization.
*/
void App_Init(void)
{
    Api_Init();
    Api_RstSQIOR_XIP();
}

/**
  \fn          int32_t App_UnInit (void)
  \brief       Application reset, enter SQI mode with XIP.
  \return      \ref execution_status
*/
int32_t App_UnInit(void)
{
    int32_t ret = ARM_DRIVER_OK;

    ret = Api_WriteEn();
    if (ARM_DRIVER_OK != ret) {
        return ARM_DRIVER_ERROR;
    }
    Api_EnSQIOR_XIP();

    return ARM_DRIVER_OK;
}

/**
  \fn          int32_t App_EraseChip (void)
  \brief       Erase complete flash.
  \return      \ref execution_status
*/
int32_t App_EraseChip(void)
{
    int32_t ret = ARM_DRIVER_OK;
    ret = Api_WriteEn();
    if (ARM_DRIVER_OK != ret) {
        return ARM_DRIVER_ERROR;
    }
    Api_EraseChip();
    ret = Api_WaitBusy(False);
    if (ARM_DRIVER_OK != ret) {
        return ARM_DRIVER_ERROR_TIMEOUT;
    }

    return ARM_DRIVER_OK;
}

/**
  \fn          int32_t App_EraseSector (uint32_t adr)
  \brief       Erase flash sector.
  \param[in]   adr  Sector address.
  \return      \ref execution_status
*/
int32_t App_EraseSector(uint32_t adr)
{
    int32_t ret = ARM_DRIVER_OK;
    ret = Api_WriteEn();
    if (ARM_DRIVER_OK != ret) {
        return ARM_DRIVER_ERROR;
    }
    Api_EraseSector(adr, False);
    ret = Api_WaitBusy(False);
    if (ARM_DRIVER_OK != ret) {
        return ARM_DRIVER_ERROR_TIMEOUT;
    }

    return ARM_DRIVER_OK;
}

/**
  \fn          int32_t App_ProgramPage (uint32_t adr, uint32_t sz, uint8_t *buf)
  \brief       Page program data in the flash memory.
  \param[in]   adr  Program address.
  \param[in]   sz  Length of data buffer.
  \param[in]   buf  Pointer to a buffer storing the program data.
  \return      \ref execution_status
*/
int32_t App_ProgramPage(uint32_t adr, uint32_t sz, uint8_t *buf)
{
    int32_t ret = ARM_DRIVER_OK;
    ret = Api_WriteEn();
    if (ARM_DRIVER_OK != ret) {
        return ARM_DRIVER_ERROR;
    }
    Api_ProgramPage(adr, sz, buf, False);
    ret = Api_WaitBusy(False);
    if (ARM_DRIVER_OK != ret) {
        return ARM_DRIVER_ERROR_TIMEOUT;
    }

    return ARM_DRIVER_OK;
}

/**
  \fn          int32_t App_CheckFlash (FLASH_INFORMATION *infor)
  \brief       Get status, configuration register, and identification of flash.
  \param[in]   infor  Pointer to a struct storing the flash information data.
  \return      \ref execution_status
*/
int32_t App_CheckFlash(FLASH_INFORMATION *infor)
{
    int32_t ret = ARM_DRIVER_OK;
    JEDEC_ID buf = {0};

    if (NULL == infor) {
        return ARM_DRIVER_ERROR;
    }

    infor->status_reg = Api_ReadSR(False);

    infor->config_reg = Api_ReadCR();

    ret = Api_ReadID(&buf);
    if (ARM_DRIVER_OK != ret) {
        return ret;
    }
    infor->jedec_id.manu_id = buf.manu_id;
    infor->jedec_id.dev_t = buf.dev_t;
    infor->jedec_id.dev_id = buf.dev_id;

    return ARM_DRIVER_OK;
}

/**
  \fn          int32_t App_TestIndRd (uint32_t adr, uint32_t sz, uint8_t *buf)
  \brief       Using indirect read mode of QCU to read the flash device memory.
  \param[in]   adr  Read address.
  \param[in]   sz  Length of data buffer.
  \param[in]   buf  Pointer to a buffer storing the read data.
  \return      \ref execution_status
*/
int32_t App_TestIndRd(uint32_t adr, uint32_t sz, uint8_t *buf)
{
    int32_t ret = ARM_DRIVER_OK;
    ret = Api_ReadMemInd(adr, sz, buf);
    if (ARM_DRIVER_OK != ret) {
        return ARM_DRIVER_ERROR_TIMEOUT;
    }

    return ARM_DRIVER_OK;
}
