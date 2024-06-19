/**************************************************************************//**
 * @file     Flash_driver.c
 * @brief    CMSIS ArmChina STAR Flash Driver Template C File
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

#ifdef  __cplusplus
extern "C"
{
#endif

/* Macros declaration */
/* Change macros and Instr_List[] according to your device.
Look out BIT_30 and BIT_29 for RMCR and OMCR in QCU_Comm_Typedef. */
#define FLASH_SST26VF064B

#if defined FLASH_SST26VF064B       /* For flash of STAR MPS3 board */
#define SCKSCALER_DIVIDE_8          (0x03U)
#define CSRHT_NINE_H_CYCLE          (0x07U)
#define FMSIZE_8M                   (0x16U)
static const QCU_CR_TypeDef CR_Default = {SCKSCALER_DIVIDE_8, CSRHT_NINE_H_CYCLE, FMSIZE_8M, XIPMODE_EXIT, SCKMODE_MODE0, DIRECT_READ_MODE};

#define NUMDC_2CLOCK                (0x02U)
#define NUMDC_4CLOCK                (0x04U)
#define NUMDC_8CLOCK                (0x08U)

#define CMD_WREN                    (0x06U)
#define CMD_CE                      (0xC7U)
#define CMD_SE                      (0x20U)
#define CMD_PP                      (0x02U)
#define CMD_WRDI                    (0x04U)
#define CMD_WRCR                    (0x01U)  /* Named WRSR in manual*/
#define CMD_EQIO                    (0x38U)
#define CMD_RSTQIO                  (0xFFU)
#define CMD_RDSR1                   (0x05U)
#define CMD_RDCR                    (0x35U)
#define CMD_RDID                    (0x9FU)
#define CMD_READ_INDIRECT           (0x03U)
#define CMD_SDOR_INDIRECT           (0x3BU)
#define CMD_SQIOR_INDIRECT          (0xEBU)
#define CMD_READ_DIRECT             (0x03U)
#define CMD_SDIOR_DIRECT            (0xBBU)
#define CMD_SQIOR_DIRECT            (0xEBU)
#define CMD_HSREAD_DIRECT           (0x0BU)

#define ALT_BYTE_DEFAULT            (0x00U)
#define ALT_BYTE                    (0xAAU)
static const QCU_Comm_Typedef Instr_List[]={
/* WREN */              {DDRMODE_DISABLE, IDMODE_WRITE, XIP_BIT_CLEAR, NUMDC_DEFAULT, RXDLY_DEFAULT, DSIZE_8BITS, DMODE_NO_DATA, \
                         ABSIZE_ALT_BYTES_8BITS, ABMODE_NONE, ADSIZE_8BITS, ADMODE_NONE, IMODE_SINGLE_LINE, CMD_WREN, ALT_BYTE_DEFAULT}, \
/* WREN_Q */            {DDRMODE_DISABLE, IDMODE_WRITE, XIP_BIT_CLEAR, NUMDC_DEFAULT, RXDLY_DEFAULT, DSIZE_8BITS, DMODE_NO_DATA, \
                         ABSIZE_ALT_BYTES_8BITS, ABMODE_NONE, ADSIZE_8BITS, ADMODE_NONE, IMODE_QUAD_LINES, CMD_WREN, ALT_BYTE_DEFAULT}, \
/* CE */                {DDRMODE_DISABLE, IDMODE_WRITE, XIP_BIT_CLEAR, NUMDC_DEFAULT, RXDLY_DEFAULT, DSIZE_8BITS, DMODE_NO_DATA, \
                         ABSIZE_ALT_BYTES_8BITS, ABMODE_NONE, ADSIZE_8BITS, ADMODE_NONE, IMODE_SINGLE_LINE, CMD_CE, ALT_BYTE_DEFAULT}, \
/* SE */                {DDRMODE_DISABLE, IDMODE_WRITE, XIP_BIT_CLEAR, NUMDC_DEFAULT, RXDLY_DEFAULT, DSIZE_8BITS, DMODE_NO_DATA, \
                         ABSIZE_ALT_BYTES_8BITS, ABMODE_NONE, ADSIZE_24BITS, ADMODE_SINGLE_LINE, IMODE_SINGLE_LINE, CMD_SE, ALT_BYTE_DEFAULT}, \
/* SE_Q */              {DDRMODE_DISABLE, IDMODE_WRITE, XIP_BIT_CLEAR, NUMDC_DEFAULT, RXDLY_DEFAULT, DSIZE_8BITS, DMODE_NO_DATA, \
                         ABSIZE_ALT_BYTES_8BITS, ABMODE_NONE, ADSIZE_24BITS, ADMODE_QUAD_LINES, IMODE_QUAD_LINES, CMD_SE, ALT_BYTE_DEFAULT}, \
/* PP */                {DDRMODE_DISABLE, IDMODE_WRITE, XIP_BIT_CLEAR, NUMDC_DEFAULT, RXDLY_DEFAULT, DSIZE_8BITS, DMODE_SINGLE_LINE_DATA, \
                         ABSIZE_ALT_BYTES_8BITS, ABMODE_NONE, ADSIZE_24BITS, ADMODE_SINGLE_LINE, IMODE_SINGLE_LINE, CMD_PP, ALT_BYTE_DEFAULT}, \
/* PP_Q */              {DDRMODE_DISABLE, IDMODE_WRITE, XIP_BIT_CLEAR, NUMDC_DEFAULT, RXDLY_DEFAULT, DSIZE_8BITS, DMODE_QUAD_LINE_DATA, \
                         ABSIZE_ALT_BYTES_8BITS, ABMODE_NONE, ADSIZE_24BITS, ADMODE_QUAD_LINES, IMODE_QUAD_LINES, CMD_PP, ALT_BYTE_DEFAULT}, \
/* WRDI */              {DDRMODE_DISABLE, IDMODE_WRITE, XIP_BIT_CLEAR, NUMDC_DEFAULT, RXDLY_DEFAULT, DSIZE_8BITS, DMODE_NO_DATA, \
                         ABSIZE_ALT_BYTES_8BITS, ABMODE_NONE, ADSIZE_8BITS, ADMODE_NONE, IMODE_SINGLE_LINE, CMD_WRDI, ALT_BYTE_DEFAULT}, \
/* WRCR */              {DDRMODE_DISABLE, IDMODE_WRITE, XIP_BIT_CLEAR, NUMDC_DEFAULT, RXDLY_DEFAULT, DSIZE_8BITS, DMODE_SINGLE_LINE_DATA, \
                         ABSIZE_ALT_BYTES_8BITS, ABMODE_NONE, ADSIZE_8BITS, ADMODE_NONE, IMODE_SINGLE_LINE, CMD_WRCR, ALT_BYTE_DEFAULT}, \
/* EQIO */              {DDRMODE_DISABLE, IDMODE_WRITE, XIP_BIT_CLEAR, NUMDC_DEFAULT, RXDLY_DEFAULT, DSIZE_8BITS, DMODE_NO_DATA, \
                         ABSIZE_ALT_BYTES_8BITS, ABMODE_NONE, ADSIZE_8BITS, ADMODE_NONE, IMODE_SINGLE_LINE, CMD_EQIO, ALT_BYTE_DEFAULT}, \
/* RSTQIO */            {DDRMODE_DISABLE, IDMODE_WRITE, XIP_BIT_CLEAR, NUMDC_DEFAULT, RXDLY_DEFAULT, DSIZE_8BITS, DMODE_NO_DATA, \
                         ABSIZE_ALT_BYTES_8BITS, ABMODE_NONE, ADSIZE_8BITS, ADMODE_NONE, IMODE_SINGLE_LINE, CMD_RSTQIO, ALT_BYTE_DEFAULT}, \
/* RSTQIO_Q */          {DDRMODE_DISABLE, IDMODE_WRITE, XIP_BIT_CLEAR, NUMDC_DEFAULT, RXDLY_DEFAULT, DSIZE_8BITS, DMODE_NO_DATA, \
                         ABSIZE_ALT_BYTES_8BITS, ABMODE_NONE, ADSIZE_8BITS, ADMODE_NONE, IMODE_QUAD_LINES, CMD_RSTQIO, ALT_BYTE_DEFAULT}, \
/* RDSR1 */             {DDRMODE_DISABLE, IDMODE_READ, XIP_BIT_CLEAR, NUMDC_DEFAULT, RXDLY_DEFAULT, DSIZE_8BITS, DMODE_SINGLE_LINE_DATA, \
                         ABSIZE_ALT_BYTES_8BITS, ABMODE_NONE, ADSIZE_24BITS, ADMODE_NONE, IMODE_SINGLE_LINE, CMD_RDSR1, ALT_BYTE_DEFAULT}, \
/* RDSR1_Q */           {DDRMODE_DISABLE, IDMODE_READ, XIP_BIT_CLEAR, NUMDC_2CLOCK, RXDLY_DEFAULT, DSIZE_8BITS, DMODE_QUAD_LINE_DATA, \
                         ABSIZE_ALT_BYTES_8BITS, ABMODE_NONE, ADSIZE_24BITS, ADMODE_NONE, IMODE_QUAD_LINES, CMD_RDSR1, ALT_BYTE_DEFAULT}, \
/* RDCR */              {DDRMODE_DISABLE, IDMODE_READ, XIP_BIT_CLEAR, NUMDC_DEFAULT, RXDLY_DEFAULT, DSIZE_8BITS, DMODE_SINGLE_LINE_DATA, \
                         ABSIZE_ALT_BYTES_8BITS, ABMODE_NONE, ADSIZE_24BITS, ADMODE_NONE, IMODE_SINGLE_LINE, CMD_RDCR, ALT_BYTE_DEFAULT}, \
/* RDID */              {DDRMODE_DISABLE, IDMODE_READ, XIP_BIT_CLEAR, NUMDC_DEFAULT, RXDLY_DEFAULT, DSIZE_8BITS, DMODE_SINGLE_LINE_DATA, \
                         ABSIZE_ALT_BYTES_8BITS, ABMODE_NONE, ADSIZE_24BITS, ADMODE_NONE, IMODE_SINGLE_LINE, CMD_RDID, ALT_BYTE_DEFAULT}, \
/* READ_IND */          {DDRMODE_DISABLE, IDMODE_READ, XIP_BIT_CLEAR, NUMDC_DEFAULT, RXDLY_DEFAULT, DSIZE_8BITS, DMODE_SINGLE_LINE_DATA, \
                         ABSIZE_ALT_BYTES_8BITS, ABMODE_NONE, ADSIZE_24BITS, ADMODE_SINGLE_LINE, IMODE_SINGLE_LINE, CMD_READ_INDIRECT, ALT_BYTE_DEFAULT}, \
/* SDOR_IND */          {DDRMODE_DISABLE, IDMODE_READ, XIP_BIT_CLEAR, NUMDC_8CLOCK, RXDLY_DEFAULT, DSIZE_8BITS, DMODE_DUAL_LINE_DATA, \
                         ABSIZE_ALT_BYTES_8BITS, ABMODE_NONE, ADSIZE_24BITS, ADMODE_SINGLE_LINE, IMODE_SINGLE_LINE, CMD_SDOR_INDIRECT, ALT_BYTE_DEFAULT}, \
/* SQIOR_IND */         {DDRMODE_DISABLE, IDMODE_READ, XIP_BIT_CLEAR, NUMDC_4CLOCK, RXDLY_DEFAULT, DSIZE_8BITS, DMODE_QUAD_LINE_DATA, \
                         ABSIZE_ALT_BYTES_8BITS, ABMODE_QUAD_LINES, ADSIZE_24BITS, ADMODE_QUAD_LINES, IMODE_SINGLE_LINE, CMD_SQIOR_INDIRECT, ALT_BYTE_DEFAULT}, \
/* READ_DIR */          {DDRMODE_DISABLE, NOWRAP_DEFAULT, WRAPMODE_DEFAULT, NUMDC_DEFAULT, RXDLY_DEFAULT, DSIZE_8BITS, DMODE_SINGLE_LINE_DATA, \
                         ABSIZE_ALT_BYTES_8BITS, ABMODE_NONE, ADSIZE_24BITS, ADMODE_SINGLE_LINE, IMODE_SINGLE_LINE, CMD_READ_DIRECT, ALT_BYTE_DEFAULT}, \
/* SDIOR_DIR */         {DDRMODE_DISABLE, NOWRAP_DEFAULT, WRAPMODE_DEFAULT, NUMDC_DEFAULT, RXDLY_DEFAULT, DSIZE_8BITS, DMODE_DUAL_LINE_DATA, \
                         ABSIZE_ALT_BYTES_8BITS, ABMODE_DUAL_LINES, ADSIZE_24BITS, ADMODE_DUAL_LINES, IMODE_SINGLE_LINE, CMD_SDIOR_DIRECT, ALT_BYTE_DEFAULT}, \
/* SQIOR_DIR */         {DDRMODE_DISABLE, NOWRAP_DEFAULT, WRAPMODE_DEFAULT, NUMDC_4CLOCK, RXDLY_DEFAULT, DSIZE_8BITS, DMODE_QUAD_LINE_DATA, \
                         ABSIZE_ALT_BYTES_8BITS, ABMODE_QUAD_LINES, ADSIZE_24BITS, ADMODE_QUAD_LINES, IMODE_SINGLE_LINE, CMD_SQIOR_DIRECT, ALT_BYTE_DEFAULT}, \
/* High-Speed R_Q */    {DDRMODE_DISABLE, NOWRAP_DEFAULT, WRAPMODE_DEFAULT, NUMDC_4CLOCK, RXDLY_DEFAULT, DSIZE_8BITS, DMODE_QUAD_LINE_DATA, \
                         ABSIZE_ALT_BYTES_8BITS, ABMODE_QUAD_LINES, ADSIZE_24BITS, ADMODE_QUAD_LINES, IMODE_QUAD_LINES, CMD_HSREAD_DIRECT, ALT_BYTE_DEFAULT}
};
#else
#error "please prepare instruction list of flash in your board!"
#endif


/**
  \fn          void Flash_InitParameter (QCU_Init_TypeDef *param)
  \brief       Get initialization parameters of QCU and command max value according to flash device.
  \param[in]   param  Pointer to QCU initialization parameters.
*/
void Flash_InitParameter(QCU_Init_TypeDef *param)
{
    param->CR.SCKSCALER = CR_Default.SCKSCALER;
    param->CR.CSRHT = CR_Default.CSRHT;
    param->CR.FSize = CR_Default.FSize;
    param->CR.XIPMODE = CR_Default.XIPMODE;
    param->CR.SCKMODE = CR_Default.SCKMODE;
    param->CR.OPMODE = CR_Default.OPMODE;
    param->CMD.Indiret_Write = INDIRECT_WRITE_CMD_MAX;
    param->CMD.Indiret_Read = INDIRECT_READ_CMD_MAX;
    param->CMD.Direct_Read = DIRECT_READ_CMD_MAX;
}

/**
  \fn          void Flash_GetParameter (uint8_t cmd, bool xip_mode, QCU_Comm_Typedef *instr)
  \brief       Get communication parameters of QCU and flash device.
  \param[in]   cmd  Flash operation command.
  \param[in]   xip_mode  Whether enter XIP mode.
  \param[in]   instr  Pointer to QCU communication instruction.
*/
void Flash_GetParameter(uint8_t cmd, bool xip_mode, QCU_Comm_Typedef *instr)
{
    instr->DDRMODE = Instr_List[cmd].DDRMODE;
    instr->BIT_30 = Instr_List[cmd].BIT_30;
    instr->BIT_29 = Instr_List[cmd].BIT_29;
    instr->NUMDC = Instr_List[cmd].NUMDC;
    instr->RXDLY = Instr_List[cmd].RXDLY;
    instr->DSIZE = Instr_List[cmd].DSIZE;
    instr->DMODE = Instr_List[cmd].DMODE;
    instr->ABSIZE = Instr_List[cmd].ABSIZE;
    instr->ABMODE = Instr_List[cmd].ABMODE;
    instr->ADSIZE = Instr_List[cmd].ADSIZE;
    instr->ADMODE = Instr_List[cmd].ADMODE;
    instr->IMODE = Instr_List[cmd].IMODE;
    instr->INSTRUCTION = Instr_List[cmd].INSTRUCTION;
    instr->ALT = (False == xip_mode)? Instr_List[cmd].ALT: ALT_BYTE;
}

#ifdef  __cplusplus
}
#endif
