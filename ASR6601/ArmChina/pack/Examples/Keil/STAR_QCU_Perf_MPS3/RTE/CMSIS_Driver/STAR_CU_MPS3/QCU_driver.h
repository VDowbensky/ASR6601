/**************************************************************************//**
 * @file     QCU_driver.h
 * @brief    CMSIS ArmChina STAR QCU Driver Header File
 * @version  V1.0.0: The initial version to support STAR work with QCU on MPS3.
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

#ifndef QCU_DRIVER_H_
#define QCU_DRIVER_H_

#include "QCU_common.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
\brief QCU communication data structure definition 
*/
typedef struct {
    uint32_t addr;
    uint32_t length;
    void     *data;
} QCU_CommData_Typedef;

typedef void (*GetInitParameter_t)(QCU_Init_TypeDef *init_param);
typedef void (*GetParameter_t)(uint8_t cmd, bool xip_mode, QCU_Comm_Typedef *instr);

/* Extern function declaration */
/**
  \fn          int32_t QCU_Init (GetInitParameter_t GetInitParameter)
  \brief       Initialize the control register of QCU.
  \param[in]   GetInitParameter  Pointer to a function can get initialization parameters.
  \return      \ref execution_status
*/
extern int32_t QCU_Init(GetInitParameter_t GetInitParameter);

/**
  \fn          int32_t QCU_Write (uint8_t cmd, uint32_t addr, uint32_t length, void *data, bool xip_mode, GetParameter_t GetParameter)
  \brief       QCU indirect write mode driver(QCU write only in indirect mode).
  \param[in]   cmd  QCU indirect write command.
  \param[in]   comm_data  Pointer to a struct storing the communication data parameter.
  \param[in]   xip_mode  Indicate whether enter XIP mode.
  \param[in]   GetParameter  Pointer to a function can get parameters.
  \return      \ref execution_status
*/
extern int32_t QCU_Write(uint8_t cmd, QCU_CommData_Typedef *comm_data, bool xip_mode, GetParameter_t GetParameter);

/**
  \fn          int32_t QCU_Read_Indirect (uint8_t cmd, QCU_CommData_Typedef *comm_data, GetParameter_t GetParameter)
  \brief       QCU indirect read mode driver.
  \param[in]   cmd  QCU indirect read command.
  \param[in]   comm_data  Pointer to a struct storing the communication data parameter.
  \param[in]   GetParameter  Pointer to a function can get parameters.
  \return      \ref execution_status
*/
extern int32_t QCU_Read_Indirect(uint8_t cmd, QCU_CommData_Typedef *comm_data, GetParameter_t GetParameter);

/**
  \fn          int32_t QCU_Read_Direct (uint8_t cmd, bool xip_mode, GetParameter_t GetParameter)
  \brief       QCU direct read mode driver.
  \param[in]   cmd  QCU direct read command.
  \param[in]   xip_mode  Indicate whether enter XIP mode.
  \param[in]   GetParameter  Pointer to a function can get parameters.
  \return      \ref execution_status
*/
extern int32_t QCU_Read_Direct(uint8_t cmd, bool xip_mode, GetParameter_t GetParameter);

/**
  \fn          int32_t QCU_SetOPMode (uint8_t op_mode, bool xip_mode)
  \brief       QCU set operation mode.
  \param[in]   op_mode  QCU operation mode.
  \param[in]   xip_mode  Indicate whether enter XIP mode.
  \return      \ref execution_status
  \note        Please make sure parameters of registers(OMCR/RMCR...) are configured correctly before calling this function.
*/
extern int32_t QCU_SetOPMode(uint8_t op_mode, bool xip_mode);

/**
  \fn          uint8_t QCU_GetOPMode (void)
  \brief       Get current operation mode of QCU.
*/
extern uint8_t QCU_GetOPMode(void);


#ifdef  __cplusplus
}
#endif

#endif /* QCU_DRIVER_H_ */
