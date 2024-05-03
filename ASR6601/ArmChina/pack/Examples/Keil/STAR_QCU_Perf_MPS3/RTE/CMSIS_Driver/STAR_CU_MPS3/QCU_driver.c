/**************************************************************************//**
 * @file     QCU_driver.c
 * @brief    CMSIS ArmChina STAR QCU Driver Template C File
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

#ifdef  __cplusplus
extern "C"
{
#endif

/* For check the max value of commands in different operation mode */
static CMD_MAX_TypeDef cmd_max = {0, 0, 0};

/* Static function declaration */
/**
  \fn          int32_t QCU_SendData (const void *data, uint32_t num, uint8_t width)
  \brief       QCU send data interface.
  \param[in]   data  Pointer to a buffer storing the data to send.
  \param[in]   num  Number of data items to send.
  \param[in]   width  Width of data items to send.
  \return      \ref execution_status
*/
static int32_t QCU_SendData(const void *data, uint32_t num, uint8_t width);

/**
  \fn          int32_t QCU_ReceiveData (void *data, uint32_t num, uint8_t width)
  \brief       QCU receive data interface.
  \param[in]   data  Pointer to a buffer storing the data to receive.
  \param[in]   num  Number of data items to receive.
  \param[in]   width  Width of data items to receive.
  \return      \ref execution_status
*/
static int32_t QCU_ReceiveData(void *data, uint32_t num, uint8_t width);

/**
  \fn          void QCU_CheckTCF (void)
  \brief       Check transfer complete flag of QCU.
*/
static uint8_t QCU_CheckTCF(void);

/**
  \fn          void QCU_ClearTCF (void)
  \brief       Clear transfer complete flag of QCU.
*/
static void QCU_ClearTCF(void);

/**
  \fn          uint8_t QCU_CheckOPCRCF (void)
  \brief       Check operation mode change request complete flag of QCU.
*/
static uint8_t QCU_CheckOPCRCF(void);


/* Driver functions definition */
/**
  \fn          int32_t QCU_Init (GetInitParameter_t GetInitParameter)
  \brief       Initialize the control register of QCU.
  \param[in]   GetInitParameter  Pointer to a function can get initialization parameters.
  \return      \ref execution_status
*/
int32_t QCU_Init(GetInitParameter_t GetInitParameter)
{
    QCU_Init_TypeDef param;
    uint32_t value = 0;

    /* Check the input parameters */
    if (NULL == GetInitParameter) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    GetInitParameter(&param);

    /* Check the parameters */
    if ((param.CR.SCKSCALER> 0x3f) || (param.CR.CSRHT > 0x7) ||(param.CR.FSize <= 0x4) \
    ||  (param.CR.FSize >= 0x1f) || (param.CR.XIPMODE > 0x1) || (param.CR.SCKMODE > 0x1) \
    ||  (param.CR.OPMODE > 0x3)) {
        /* QCU CR init parameter error */
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    /* Prepare parameters according to flash type and hardware configuration */
    value = (param.CR.SCKSCALER << SCKSCALER_POS) | (param.CR.CSRHT << CSRHT_POS) \
          | (param.CR.FSize << FMSIZE_POS) | (param.CR.XIPMODE << XIPMODE_POS) \
          | (param.CR.SCKMODE << SCKMODE_POS) | (param.CR.OPMODE << OPMODE_POS);

    /* Update the control register of QCU */
    QUADSPI->CR = value;

    /* Set the max value of different mode commands */
    cmd_max.Indiret_Write = param.CMD.Indiret_Write;
    cmd_max.Indiret_Read = param.CMD.Indiret_Read;
    cmd_max.Direct_Read = param.CMD.Direct_Read;

    return ARM_DRIVER_OK;
}

/**
  \fn          int32_t QCU_Write (uint8_t cmd, uint32_t addr, uint32_t length, void *data, bool xip_mode, GetParameter_t GetParameter)
  \brief       QCU indirect write mode driver(QCU write only in indirect mode).
  \param[in]   cmd  QCU indirect write command.
  \param[in]   comm_data  Pointer to a struct storing the communication data parameter.
  \param[in]   xip_mode  Indicate whether enter XIP mode.
  \param[in]   GetParameter  Pointer to a function can get parameters.
  \return      \ref execution_status
*/
//TODO: entering xip mode in indirect mode has not yet test.
int32_t QCU_Write(uint8_t cmd, QCU_CommData_Typedef *comm_data, bool xip_mode, GetParameter_t GetParameter)
{
    QCU_Comm_Typedef param;
    uint32_t value;
    uint32_t timeout = 0;

    /* Check the input parameters */
    if ((cmd >= cmd_max.Indiret_Write) || (NULL == comm_data) || (NULL == GetParameter)) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    GetParameter(cmd, xip_mode, &param);
    /* Prepare parameters according to flash type */
    if ((param.DDRMODE > 1) || (param.BIT_30 > 1) ||(param.BIT_29 > 1) || (param.NUMDC > 0x1F) \
    ||  (param.RXDLY > 0x3) || (param.DSIZE > 0x3) || (param.DMODE > 0x3) || (param.ABSIZE > 0x3) \
    ||  (param.ABMODE > 0x3) || (param.ADSIZE > 0x3) || (param.ADMODE > 0x3) || (param.IMODE > 0x3) \
    ||  (param.INSTRUCTION > 0xFF) || (cmd >= cmd_max.Indiret_Write)) {
        /* Indirect write mode parameter error */
        return ARM_DRIVER_ERROR_PARAMETER;
    } else {
        value = (param.DDRMODE << DDRMODE_POS) |(param.BIT_30 << NOWRAP_POS) | (param.BIT_29 << WRAPMODE_POS) \
              | (param.NUMDC << NUMDC_POS) | (param.RXDLY << RXDLY_POS) | (param.DSIZE << DSIZE_POS) \
              | (param.DMODE << DMODE_POS) | (param.ABSIZE << ABSIZE_POS) | (param.ABMODE << ABMODE_POS) \
              | (param.ADSIZE << ADSIZE_POS) | (param.ADMODE << ADMODE_POS) | (param.IMODE << IMODE_POS) \
              | (param.INSTRUCTION << INSTRUCTION_POS);

        /* Update indirect mode control registers of QCU */
        QUADSPI->OMCR = value;
        QUADSPI->OABR = param.ALT;
        QUADSPI->IMAR = comm_data->addr & 0xFFFFFF;
        if ((DMODE_NO_DATA != param.DMODE) && (0 != comm_data->length)) {
            QUADSPI->DLR = comm_data->length-1;
        } else {
            QUADSPI->DLR = 0;
        }

        /* Set QCU operation mode */
        QCU_SetOPMode(QCU_INDIRECT_WRITE, xip_mode);

        /* Send data if request */
        if ((NULL != comm_data->data) && (0 != comm_data->length)) {
            QCU_SendData(comm_data->data, comm_data->length, param.DSIZE);

            while ((!QCU_CheckTCF()) && (timeout < TCF_TIMEOUT)) {
                timeout++;
            }

            if (timeout < TCF_TIMEOUT) {
                QCU_ClearTCF();
            } else {
                return ARM_DRIVER_ERROR_TIMEOUT;
            }
        }
    }

    return ARM_DRIVER_OK;
}

/**
  \fn          int32_t QCU_Read_Indirect (uint8_t cmd, QCU_CommData_Typedef *comm_data, GetParameter_t GetParameter)
  \brief       QCU indirect read mode driver.
  \param[in]   cmd  QCU indirect read command.
  \param[in]   comm_data  Pointer to a struct storing the communication data parameter.
  \param[in]   GetParameter  Pointer to a function can get parameters.
  \return      \ref execution_status
*/
int32_t QCU_Read_Indirect(uint8_t cmd, QCU_CommData_Typedef *comm_data, GetParameter_t GetParameter)
{
    QCU_Comm_Typedef param;
    uint32_t value;
    uint32_t timeout = 0;

    /* Check the input parameters */
    if ((cmd < cmd_max.Indiret_Write) || (cmd >= cmd_max.Indiret_Read) || (NULL == comm_data) || (NULL == GetParameter)) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    GetParameter(cmd, False, &param);
    /* Prepare parameters according to flash type */
    if ((param.DDRMODE > 1) || (param.BIT_30 > 1) ||(param.BIT_29 > 1) || (param.NUMDC > 0x1F) \
    ||  (param.RXDLY > 0x3) || (param.DSIZE > 0x3) || (param.DMODE > 0x3) || (param.ABSIZE > 0x3) \
    ||  (param.ABMODE > 0x3) || (param.ADSIZE > 0x3) || (param.ADMODE > 0x3) || (param.IMODE > 0x3) \
    ||  (param.INSTRUCTION > 0xFF) ) {
        /* Indirect read mode parameter error */
        return ARM_DRIVER_ERROR_PARAMETER;
    } else {
        value = (param.DDRMODE << DDRMODE_POS) |(param.BIT_30 << NOWRAP_POS) | (param.BIT_29 << WRAPMODE_POS) \
              | (param.NUMDC << NUMDC_POS) | (param.RXDLY << RXDLY_POS) | (param.DSIZE << DSIZE_POS) \
              | (param.DMODE << DMODE_POS) | (param.ABSIZE << ABSIZE_POS) | (param.ABMODE << ABMODE_POS) \
              | (param.ADSIZE << ADSIZE_POS) | (param.ADMODE << ADMODE_POS) | (param.IMODE << IMODE_POS) \
              | (param.INSTRUCTION << INSTRUCTION_POS);

        /* Update indirect mode control registers of QCU */
        QUADSPI->OMCR = value;
        QUADSPI->OABR = param.ALT;
        QUADSPI->IMAR = comm_data->addr & 0xFFFFFF;
        if ((DMODE_NO_DATA != param.DMODE) && (0 != comm_data->length)) {
            QUADSPI->DLR = comm_data->length-1;
        } else {
            QUADSPI->DLR = 0;
        }

        /* Set QCU operation mode */
        QCU_SetOPMode(QCU_INDIRECT_READ, False);

        /* Receive data if any */
        if ((NULL != comm_data->data) && (0 != comm_data->length)) {
            QCU_ReceiveData(comm_data->data, comm_data->length, param.DSIZE);

            while ((!QCU_CheckTCF()) && (timeout < TCF_TIMEOUT)) {
                timeout++;
            }

            if (timeout < TCF_TIMEOUT) {
                QCU_ClearTCF();
            } else {
                return ARM_DRIVER_ERROR_TIMEOUT;
            }
        }
    }

    return ARM_DRIVER_OK;
}

/**
  \fn          int32_t QCU_Read_Direct (uint8_t cmd, bool xip_mode, GetParameter_t GetParameter)
  \brief       QCU direct read mode driver.
  \param[in]   cmd  QCU direct read command.
  \param[in]   xip_mode  Indicate whether enter XIP mode.
  \param[in]   GetParameter  Pointer to a function can get parameters.
  \return      \ref execution_status
*/
int32_t QCU_Read_Direct(uint8_t cmd, bool xip_mode, GetParameter_t GetParameter)
{
    QCU_Comm_Typedef param;
    uint32_t value;
    //int32_t ret = ARM_DRIVER_OK;

    /* Check the input parameters */
    if ((cmd < cmd_max.Indiret_Read) || (cmd >= cmd_max.Direct_Read) || (NULL == GetParameter)) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    GetParameter(cmd, xip_mode, &param);
    /* Prepare parameters according to flash type */
    if ((param.DDRMODE > 1) || (param.BIT_30 > 1) ||(param.BIT_29 > 1) || (param.NUMDC > 0x1F) \
    ||  (param.RXDLY > 0x3) || (param.DSIZE > 0x3) || (param.DMODE > 0x3) || (param.ABSIZE > 0x3) \
    ||  (param.ABMODE > 0x3) || (param.ADSIZE > 0x3) || (param.ADMODE > 0x3) || (param.IMODE > 0x3) \
    ||  (param.INSTRUCTION > 0xFF) ) {
        /* Direct read mode parameter error */
        return ARM_DRIVER_ERROR_PARAMETER;
    } else {
        value = (param.DDRMODE << DDRMODE_POS) |(param.BIT_30 << NOWRAP_POS) | (param.BIT_29 << WRAPMODE_POS) \
              | (param.NUMDC << NUMDC_POS) | (param.RXDLY << RXDLY_POS) | (param.DSIZE << DSIZE_POS) \
              | (param.DMODE << DMODE_POS) | (param.ABSIZE << ABSIZE_POS) | (param.ABMODE << ABMODE_POS) \
              | (param.ADSIZE << ADSIZE_POS) | (param.ADMODE << ADMODE_POS) | (param.IMODE << IMODE_POS) \
              | (param.INSTRUCTION << INSTRUCTION_POS);

        /* Update direct mode control registers of QCU */
        QUADSPI->RMCR = value;
        QUADSPI->RABR = param.ALT;
#if 0
        /* Set QCU operation mode */
        ret = QCU_SetOPMode(QCU_DIRECT_READ, xip_mode);
        if (ARM_DRIVER_OK != ret) {
            /* QCU set mode fail */
            return ret;
        }
#endif
    }

    return ARM_DRIVER_OK;
}

/**
  \fn          int32_t QCU_SetOPMode (uint8_t op_mode, bool xip_mode)
  \brief       QCU set operation mode.
  \param[in]   op_mode  QCU operation mode.
  \param[in]   xip_mode  Indicate whether enter XIP mode.
  \return      \ref execution_status
  \note        Please make sure parameters of registers(OMCR/RMCR...) are configured correctly before calling this function.
*/
int32_t QCU_SetOPMode(uint8_t op_mode, bool xip_mode)
{
    uint32_t value;
    uint32_t timeout = 0;

    if (op_mode > QCU_INACTIVE) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    if (QCU_GetOPMode() == INDIRECT_MODE) {
        /* QCU_Set_OPMODE: can`t set the CR register in Indirect mode */
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    if (op_mode == QCU_DIRECT_READ) {
        value = QUADSPI->CR & ~(OPMODE_MSK | XIPMODE_MSK);
        if (xip_mode) {
            if ((QCU_GetOPMode() != QCU_DIRECT_READ)) {
                return ARM_DRIVER_ERROR_UNSUPPORTED;
            }
            value |= ((DIRECT_READ_MODE) | (XIPMODE_ENTER << XIPMODE_POS));
        } else {
            value |= DIRECT_READ_MODE;
        }
        QUADSPI->CR = value;
    } else if (op_mode == QCU_INDIRECT_READ) {
        /* Empty FIFO */
        QUADSPI->SR |= FIFOEMPTY_MSK;
        if (xip_mode) {
            return ARM_DRIVER_ERROR_PARAMETER;
        }

        value = QUADSPI->CR & ~(OPMODE_MSK);
        value |= (INDIRECT_MODE << OPMODE_POS);  
        QUADSPI->CR = value;
    } else if (op_mode == QCU_INDIRECT_WRITE) {
        /* Empty FIFO */
        QUADSPI->SR |= FIFOEMPTY_MSK;
        value = QUADSPI->CR & ~OPMODE_MSK;
        value |= (INDIRECT_MODE << OPMODE_POS);
        QUADSPI->CR = value;
    } else {
        /* Inactive mode */
        value = QUADSPI->CR & ~(OPMODE_MSK);
        value |= (INACTIVE_MODE << OPMODE_POS);
        QUADSPI->CR = value;
    }

    while ((!QCU_CheckOPCRCF()) && (timeout < OPCRCF_TIMEOUT)) {
        timeout++;
    }
    if (timeout >= OPCRCF_TIMEOUT) {
        return ARM_DRIVER_ERROR_TIMEOUT;
    }

    return ARM_DRIVER_OK;
}

/**
  \fn          uint8_t QCU_GetOPMode (void)
  \brief       Get current operation mode of QCU.
*/
uint8_t QCU_GetOPMode(void)
{
    return (QUADSPI->SR & CUR_OP_MSK);
}

/**
  \fn          int32_t QCU_SendData (const void *data, uint32_t num, uint8_t width)
  \brief       QCU send data interface.
  \param[in]   data  Pointer to a buffer storing the data to send.
  \param[in]   num  Number of data items to send.
  \param[in]   width  Width of data items to send.
  \return      \ref execution_status
*/
static int32_t QCU_SendData(const void *data, uint32_t num, uint8_t width)
{
    uint8_t *temp8 = (uint8_t*)data;
    uint16_t *temp16 = (uint16_t*)data;
    uint32_t *temp32 = (uint32_t*)data;
    uint32_t length = num;
    uint8_t interval = 0;
    uint32_t timeout= 0;

    if ((NULL == data) || (0 == num)) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    if (DSIZE_8BITS == width) {
        interval = 1;
    } else if (DSIZE_16BITS == width) {
        interval = 2;
    } else if (DSIZE_32BITS == width) {
        interval = 4;
    } else {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    if (((QUADSPI->CR & OPMODE_MSK) != INDIRECT_MODE )\
    || (((QUADSPI->OMCR & IDMODE_MSK) >> IDMODE_POS) != IDMODE_WRITE)) {
        /* The QCU mode is not correct, need TX_FIFO_enable and INDIRECT_MODE */
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    while (length && (timeout < FIFO_TIMEOUT)) {
        if (((QUADSPI->SR & FIFOFULL_MSK) >> FIFOFULL_POS) != FIFOFULL_FLAG) {

            if (DSIZE_8BITS == width) {
                QUADSPI->FDR = ((uint32_t)*temp8) & 0xFF;
                temp8++;
            } else if (DSIZE_16BITS == width) {
                QUADSPI->FDR = ((uint32_t)*temp16) & 0xFFFF;
                temp16++;
            } else {
                QUADSPI->FDR = (*temp32);
                temp32++;
            }

            timeout = 0;
            if (length > interval) {
                length -= interval;
            } else {
                break;
            }
        } else {
            timeout++;
        }
    }

    if (0 != timeout) {
        return ARM_DRIVER_ERROR_TIMEOUT;
    }

    return ARM_DRIVER_OK;
}

/**
  \fn          int32_t QCU_ReceiveData (void *data, uint32_t num, uint8_t width)
  \brief       QCU receive data interface.
  \param[in]   data  Pointer to a buffer storing the data to receive.
  \param[in]   num  Number of data items to receive.
  \param[in]   width  Width of data items to receive.
  \return      \ref execution_status
*/
static int32_t QCU_ReceiveData(void *data, uint32_t num, uint8_t width)
{
    uint8_t *temp8 = (uint8_t*)data;
    uint16_t *temp16 = (uint16_t*)data;
    uint32_t *temp32 = (uint32_t*)data;
    uint32_t length = num;
    uint8_t interval = 0;
    uint32_t timeout = 0;

    if ((NULL == data) || (0 == num)) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    if (DSIZE_8BITS == width) {
        interval = 1;
    } else if (DSIZE_16BITS == width) {
        interval = 2;
    } else if (DSIZE_32BITS == width) {
        interval = 4;
    } else {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    if (((QUADSPI->CR  & OPMODE_MSK) != INDIRECT_MODE )\
    || (((QUADSPI->OMCR & IDMODE_MSK) >> IDMODE_POS) != IDMODE_READ)) {
        /* The QCU mode is not correct, need RX_FIFO_enable and INDIRECT_MODE */
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    while (length && (timeout < FIFO_TIMEOUT)) {
        if ((QUADSPI->SR & FIFOLEVEL_MSK) >> FIFOLEVEL_POS) {

            if (width == DSIZE_8BITS) {
                *temp8 = (uint8_t)QUADSPI->FDR;
                temp8++;
            } else if (width == DSIZE_16BITS) {
                *temp16 = (uint16_t)QUADSPI->FDR;
                temp16++;
            } else {
                *temp32 = (uint32_t)QUADSPI->FDR;
                temp32++;
            }

            timeout = 0;
            if (length > interval) {
                length -= interval;
            } else {
                break;
            }
        } else {
            timeout++;
        }
    }

    if (0 != timeout) {
        return ARM_DRIVER_ERROR_TIMEOUT;
    }

    return ARM_DRIVER_OK;
}

/**
  \fn          void QCU_CheckTCF (void)
  \brief       Check transfer complete flag of QCU.
*/
static uint8_t QCU_CheckTCF(void)
{
    return (QUADSPI->SR & TCF_MSK);
}

/**
  \fn          void QCU_ClearTCF (void)
  \brief       Clear transfer complete flag of QCU.
*/
static void QCU_ClearTCF(void)
{
    QUADSPI->SR |= TCF_MSK;
}

/**
  \fn          uint8_t QCU_CheckOPCRCF (void)
  \brief       Check operation mode change request complete flag of QCU.
*/
static uint8_t QCU_CheckOPCRCF(void)
{
    return (QUADSPI->SR & OPCRCF_MSK);
}

#ifdef  __cplusplus
}
#endif
