/**************************************************************************//**
 * @file     Flash_api.c
 * @brief    CMSIS ArmChina STAR Flash API Template C File
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

#include "QCU_driver.h"
#include "Flash_driver.h"
#include "Flash_api.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/* Macros declaration */
#define QCU_COMM_NO_ADDR 0x0
#define QCU_COMM_NO_DATA 0x0
#define FLASH_ID_NUM_MAX 0x3


/* Static functions declaration */
/**
  \fn          void Api_CfgFlash (uint8_t cmd, uint32_t length, void *data, bool xip_mode)
  \brief       Configure flash device API in QCU indirect mode.
  \param[in]   cmd  QCU indirect write command.
  \param[in]   length  Length of data buffer.
  \param[in]   data  Pointer to a buffer storing the communication data parameter.
  \param[in]   xip_mode  Indicate whether enter XIP mode.
*/
static void Api_CfgFlash(uint8_t cmd, uint32_t length, void *data, bool xip_mode);

/**
  \fn          void Api_WriteCR (uint8_t val)
  \brief       Write the new value to the configuration register of flash device.
  \param[in]   val  QCU direct read command.
  \note        Only apply to SST26VF064B for now.
*/
static void Api_WriteCR(uint8_t val);

/**
  \fn          int32_t Api_DirectRead (uint8_t cmd, bool xip_mode)
  \brief       Direct read configuration api.
  \param[in]   cmd  QCU direct read command.
  \param[in]   xip_mode  Indicate whether enter XIP mode.
  \return      \ref execution_status
*/
static int32_t Api_DirectRead(uint8_t cmd, bool xip_mode);


/* API functions definition */
/**
  \fn          void Api_Init (void)
  \brief       Initialize the control register of QCU according to the device.
*/
void Api_Init(void)
{
    QCU_Init(Flash_InitParameter);
}

/**
  \fn          uint8_t Api_ReadSR (bool xip_mode)
  \brief       Read the status register of flash device.
  \param[in]   qio_mode  Indicate whether using quad lines command mode.
  \return      \ref The value of the status register.
*/
uint8_t Api_ReadSR(bool qio_mode)
{
    QCU_CommData_Typedef comm_data;
    uint8_t cmd = 0;
    uint8_t val = 0;

    comm_data.addr = QCU_COMM_NO_ADDR;
    comm_data.length = 1;
    comm_data.data = &val;
    cmd = (False == qio_mode)? READ_SR1: READ_SR1_Q;
    QCU_Read_Indirect(cmd, &comm_data, Flash_GetParameter);
    return val;
}

/**
  \fn          uint8_t Api_ReadCR (void)
  \brief       Read the configuration register of flash device.
  \return      \ref The value of the configuration register.
*/
uint8_t Api_ReadCR(void)
{
    QCU_CommData_Typedef comm_data;
    uint8_t val = 0;

    comm_data.addr = QCU_COMM_NO_ADDR;
    comm_data.length = 1;
    comm_data.data = &val;
    QCU_Read_Indirect(READ_CR, &comm_data, Flash_GetParameter);
    return val;
}

/**
  \fn          int32_t Api_ReadID (JEDEC_ID *id)
  \brief       Read the identification of flash device.
  \param[in]   id  Pointer to a struct storing the flash JEDEC-ID data.
  \return      \ref execution_status
*/
int32_t Api_ReadID(JEDEC_ID *id)
{
    int32_t ret = ARM_DRIVER_OK;
    uint8_t buf[FLASH_ID_NUM_MAX] = {0};
    QCU_CommData_Typedef comm_data;

    if (NULL == id) {
        return ARM_DRIVER_ERROR;
    }

    comm_data.addr = QCU_COMM_NO_ADDR;
    comm_data.length = FLASH_ID_NUM_MAX;
    comm_data.data = buf;
    ret = QCU_Read_Indirect(READ_ID, &comm_data, Flash_GetParameter);
    if (ARM_DRIVER_OK != ret) {
        /* QCU read indirect configuration is failed */
        return ret;
    }
    id->manu_id = buf[0];
    id->dev_t = buf[1];
    id->dev_id = buf[2];

    return ARM_DRIVER_OK;
}

/**
  \fn          int32_t Api_ReadMemInd (uint32_t adr, uint32_t sz, uint8_t *buf)
  \brief       Read the flash device memory using Indirect read mode.
  \param[in]   adr  read memory address.
  \param[in]   sz  Length of data buffer.
  \param[in]   buf  Pointer to a buffer storing the read data.
  \return      \ref execution_status
*/
int32_t Api_ReadMemInd(uint32_t adr, uint32_t sz, uint8_t *buf)
{
    int32_t ret = ARM_DRIVER_OK;
    QCU_CommData_Typedef comm_data;

    if ((0 == sz) || (NULL == buf)) {
        return ARM_DRIVER_ERROR;
    }

    comm_data.addr = adr;
    comm_data.length = sz;
    comm_data.data = buf;
    ret = QCU_Read_Indirect(READ_IND, &comm_data, Flash_GetParameter);
    if (ARM_DRIVER_OK != ret) {
        /* QCU read indirect configuration is failed */
        return ret;
    }

    return ARM_DRIVER_OK;
}

/**
  \fn          int32_t Api_WriteEn (void)
  \brief       Configure the Write-Enable bit of flash to allow write operations to occur.
  \return      \ref execution_status
*/
int32_t Api_WriteEn(void)
{
    int32_t ret = ARM_DRIVER_OK;
    uint8_t val = 0;

    Api_CfgFlash(WRITE_ENABLE, QCU_COMM_NO_DATA, NULL, False);
    ret = Api_WaitBusy(False);
    if (ARM_DRIVER_OK != ret) {
        return ret;
    }
    val = Api_ReadSR(False);
    if (!(val & WEL_BIT_POS)) {
        /* Failed WREN configuration */
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

/**
  \fn          int32_t Api_WaitBusy (bool qio_mode)
  \brief       Wait an internal Erase or Program operation of flash to complete.
  \param[in]   qio_mode  Indicate whether using quad lines command mode.
  \return      \ref execution_status
*/
int32_t Api_WaitBusy(bool qio_mode)
{
    uint32_t timeout = 0;
    int32_t status;
    uint8_t val;

    do {
        val = Api_ReadSR(qio_mode);
        if ((val & BUSY_BIT_POS) == 0U) {
            break;
        }
        timeout++;
    } while (timeout < BUSY_TIMEOUT);

    if (timeout < BUSY_TIMEOUT) {
        status = ARM_DRIVER_OK;
    } else {
        status = ARM_DRIVER_ERROR_TIMEOUT;
    }

    return status;
}

/**
  \fn          void Api_EraseChip (void)
  \brief       Erase complete flash.
*/
void Api_EraseChip(void)
{
    QCU_CommData_Typedef comm_data;
    comm_data.addr = QCU_COMM_NO_ADDR;
    comm_data.length = QCU_COMM_NO_DATA;
    comm_data.data = NULL;
    QCU_Write(ERASE_CHIP, &comm_data, False, Flash_GetParameter);
}

/**
  \fn          void Api_EraseSector (uint32_t adr, bool qio_mode)
  \brief       Erase flash sector.
  \param[in]   adr  Sector address.
  \param[in]   qio_mode  Indicate whether using quad lines command mode.
*/
void Api_EraseSector(uint32_t adr, bool qio_mode)
{
    QCU_CommData_Typedef comm_data;
    uint8_t cmd;

    comm_data.addr = adr;
    comm_data.length = QCU_COMM_NO_DATA;
    comm_data.data = NULL;
    cmd = (False == qio_mode)? ERASE_SECTOR: ERASE_SECTOR_Q;
    QCU_Write(cmd, &comm_data, False, Flash_GetParameter);
}

/**
  \fn          void Api_ProgramPage (uint32_t adr, uint32_t sz, uint8_t *buf, bool qio_mode)
  \brief       Program up to 256 bytes of data in the flash memory.
  \param[in]   adr  Program address.
  \param[in]   sz  Length of data buffer.
  \param[in]   buf  Pointer to a buffer storing the program data.
  \param[in]   qio_mode  Indicate whether using quad lines command mode.
*/
void Api_ProgramPage(uint32_t adr, uint32_t sz, uint8_t *buf, bool qio_mode)
{
    QCU_CommData_Typedef comm_data;
    uint8_t cmd;

    comm_data.addr = adr;
    comm_data.length = sz;
    comm_data.data = buf;
    cmd = (False == qio_mode)? PROGRAM_PAGE: PROGRAM_PAGE_Q;
    QCU_Write(cmd, &comm_data, False, Flash_GetParameter);
}

/**
  \fn          int32_t Api_EnSQIOR_XIP (void)
  \brief       Enter SPI Quad I/O Read with XIP mode.
  \return      \ref execution_status
*/
int32_t Api_EnSQIOR_XIP(void)
{
    uint8_t val = 0;
    int32_t ret = ARM_DRIVER_OK;

    val = Api_ReadCR();
    val |= IOC_BIT_POS;
    Api_WriteCR(val);
    ret = Api_WaitBusy(False);
    if (ARM_DRIVER_OK != ret) {
        return ret;
    }
    val = Api_ReadCR();
    if (!(val & IOC_BIT_POS)) {
        return ARM_DRIVER_ERROR;
    }

    ret = Api_DirectRead(SQIOR_DIR, True);
    if (ARM_DRIVER_OK != ret) {
        return ret;
    }
    ret = QCU_SetOPMode(QCU_DIRECT_READ, True);
    if (ARM_DRIVER_OK != ret) {
        /* QCU set mode fail */
        return ret;
    }

    return ARM_DRIVER_OK;
}

/**
  \fn          int32_t Api_RstSQIOR_XIP (void)
  \brief       Reset QCU in SPI Quad I/O Read with XIP mode to single line SPI mode.
  \return      \ref execution_status
*/
int32_t Api_RstSQIOR_XIP(void)
{
    int32_t ret = ARM_DRIVER_OK;

    QCU_CommData_Typedef comm_data;
    comm_data.addr = QCU_COMM_NO_ADDR;
    comm_data.length = QCU_COMM_NO_DATA;
    comm_data.data = NULL;
    ret = QCU_Write(RST_QIO, &comm_data, False, Flash_GetParameter);
    if (ARM_DRIVER_OK != ret) {
        return ret;
    }
    ret = Api_DirectRead(READ_DIR, False);
    if (ARM_DRIVER_OK != ret) {
        return ret;
    }
    ret = QCU_SetOPMode(QCU_DIRECT_READ, False);
    if (ARM_DRIVER_OK != ret) {
        /* QCU set mode fail */
        return ret;
    }

    return ARM_DRIVER_OK;
}

/**
  \fn          int32_t Api_EnHiSpd_XIP (void)
  \brief       Enter quad lines High-Speed Read with XIP mode.
  \return      \ref execution_status
*/
int32_t Api_EnHiSpd_XIP(void)
{
    int32_t ret = ARM_DRIVER_OK;

    /* High speed read without XIP configuration */
    ret = Api_DirectRead(HSREAD_Q_DIR, False);
    if (ARM_DRIVER_OK != ret) {
        return ret;
    }
    /* Enable QIO */
    Api_CfgFlash(ENA_QIO, QCU_COMM_NO_DATA, NULL, False);
    /* Enter XIP mode */
    ret = Api_DirectRead(HSREAD_Q_DIR, True);
    if (ARM_DRIVER_OK != ret) {
        return ret;
    }
    ret = QCU_SetOPMode(QCU_DIRECT_READ, True);
    if (ARM_DRIVER_OK != ret) {
        /* QCU set mode fail */
        return ret;
    }

    return ARM_DRIVER_OK;
}

/**
  \fn          int32_t Api_RstHiSpd_XIP (void)
  \brief       Reset QCU in quad lines High-Speed Read with XIP mode to single line SPI mode.
  \return      \ref execution_status
  \note        Exit(RSTQIO) twice from quad lines High-Speed read with XIP. The first RSTQIO command will only
               return the device to a state where it can accept new command instruction. An additional RSTQIO
               is required to reset the device to SPI mode.
*/
int32_t Api_RstHiSpd_XIP(void)
{
    int32_t ret = ARM_DRIVER_OK;
    QCU_CommData_Typedef comm_data;
    comm_data.addr = QCU_COMM_NO_ADDR;
    comm_data.length = QCU_COMM_NO_DATA;
    comm_data.data = NULL;
    /* Exit Set Mode */
    ret = Api_DirectRead(HSREAD_Q_DIR, False);
    if (ARM_DRIVER_OK != ret) {
        return ret;
    }
    ret = QCU_Write(RST_QIO_Q, &comm_data, False, Flash_GetParameter);
    if (ARM_DRIVER_OK != ret) {
        return ret;
    }
    /* Return SPI Mode */
    ret = Api_DirectRead(READ_DIR, False);
    if (ARM_DRIVER_OK != ret) {
        return ret;
    }
    ret = QCU_Write(RST_QIO_Q, &comm_data, False, Flash_GetParameter);
    if (ARM_DRIVER_OK != ret) {
        return ret;
    }

    return ARM_DRIVER_OK;
}

/**
  \fn          void Api_CfgFlash (uint8_t cmd, uint32_t length, void *data, bool xip_mode)
  \brief       Configure flash device API in QCU indirect mode.
  \param[in]   cmd  QCU indirect write command.
  \param[in]   length  Length of data buffer.
  \param[in]   data  Pointer to a buffer storing the communication data parameter.
  \param[in]   xip_mode  Indicate whether enter XIP mode.
*/
static void Api_CfgFlash(uint8_t cmd, uint32_t length, void *data, bool xip_mode)
{
    QCU_CommData_Typedef comm_data;
    comm_data.addr = QCU_COMM_NO_ADDR;
    comm_data.length = length;
    comm_data.data = data;
    QCU_Write(cmd, &comm_data, xip_mode, Flash_GetParameter);
}

/**
  \fn          void Api_WriteCR (uint8_t val)
  \brief       Write the new value to the configuration register of flash device.
  \param[in]   val  QCU direct read command.
  \note        Only apply to SST26VF064B for now.
*/
static void Api_WriteCR(uint8_t val)
{
    uint8_t data[2] = {0};
    data[1] = val;
    Api_CfgFlash(WRITE_CR, 2, data, False);
}

/**
  \fn          int32_t Api_DirectRead (uint8_t cmd, bool xip_mode)
  \brief       Direct read configuration api.
  \param[in]   cmd  QCU direct read command.
  \param[in]   xip_mode  Indicate whether enter XIP mode.
  \return      \ref execution_status
*/
static int32_t Api_DirectRead(uint8_t cmd, bool xip_mode)
{
    int32_t ret = ARM_DRIVER_OK;

    ret = QCU_Read_Direct(cmd, xip_mode, Flash_GetParameter);
    if (ARM_DRIVER_OK != ret) {
        /* QCU read direct configuration is failed */
        return ret;
    }

    return ARM_DRIVER_OK;
}

#ifdef  __cplusplus
}
#endif
