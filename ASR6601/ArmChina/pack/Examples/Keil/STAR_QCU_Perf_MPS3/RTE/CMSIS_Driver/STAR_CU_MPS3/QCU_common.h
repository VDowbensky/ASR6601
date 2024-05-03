/**************************************************************************//**
 * @file     QCU_common.h
 * @brief    CMSIS ArmChina STAR QCU Common Header File
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

#ifndef QCU_COMMON_H_
#define QCU_COMMON_H_

#include <stdint.h>
#include <stdio.h>

#ifdef  __cplusplus
extern "C"
{
#endif

typedef enum {
    False = 0,
    True  
} bool;


/* General return codes */
#define ARM_DRIVER_OK                 0 ///< Operation succeeded 
#define ARM_DRIVER_ERROR             -1 ///< Unspecified error
#define ARM_DRIVER_ERROR_BUSY        -2 ///< Driver is busy
#define ARM_DRIVER_ERROR_TIMEOUT     -3 ///< Timeout occurred
#define ARM_DRIVER_ERROR_UNSUPPORTED -4 ///< Operation not supported
#define ARM_DRIVER_ERROR_PARAMETER   -5 ///< Parameter error
#define ARM_DRIVER_ERROR_SPECIFIC    -6 ///< Start of driver specific errors 


/**
\brief Type define of QCU register
*/
typedef struct {
    volatile uint32_t CR;       /*!< QCU control register,                                      Address offset: 0x00 */
    volatile uint32_t SR;       /*!< QCU status register,                                       Address offset: 0x04 */
    volatile uint32_t RMCR;     /*!< QCU Direct Read Access Mode Control register,              Address offset: 0x08 */
    volatile uint32_t OMCR;     /*!< QCU Operation Mode Control register,                       Address offset: 0x0C */
    volatile uint32_t RABR;     /*!< QCU Direct Read Mode Alternate Bytes register,             Address offset: 0x10 */
    volatile uint32_t OABR;     /*!< QCU direct program/indirect mode alternate bytes register, Address offset: 0x14 */
    volatile uint32_t IMAR;     /*!< QCU Indirect mode Address register,                        Address offset: 0x18 */
    volatile uint32_t FDR;      /*!< QCU FIFO DATA register,                                    Address offset: 0x1C */
    volatile uint32_t DLR;      /*!< QCU Data Length register,                                  Address offset: 0x20 */
    volatile uint32_t WCNT;     /*!< Wait counter for response direct read access ,             Address offset: 0x24 */
} QCU_TypeDef;

/* QCU description */
#define QCU_BASE        (0xE0044000ul)                      /*QCU base address */
#define QUADSPI         ((QCU_TypeDef*)QCU_BASE)

/**
\brief QCU control structure definition 
*/
typedef struct {
    uint32_t  SCKSCALER;    /* Clock prescaler.
                               This field defines the scaler factor for generating SCK based on the clock 2*(value+1) */
    uint32_t  CSRHT;        /* CSRHT+1 defines the minimum number of CLK cycles which the chip select (CSn) must remain high between */
    uint32_t  FSize;        /* Flash memory size.
                               This field defines the size of external memory using the following formula:
                               Number of bytes in Flash memory = 2**[FMSIZE+1] */
    uint32_t  XIPMODE;      /* XIP mode select in direct read access mode.
                               This bit indicates that the flash device memory supports the XIP mode */
    uint32_t  SCKMODE;      /* SCK MODE select, Mode0/Mode3 */
    uint32_t  OPMODE;       /* Operation mode select */
} QCU_CR_TypeDef;

/**
\brief QCU command structure definition 
*/
typedef struct {
    uint8_t  Indiret_Write;
    uint8_t  Indiret_Read;
    uint8_t  Direct_Read;
} CMD_MAX_TypeDef;

/**
\brief QCU Init structure definition 
*/
typedef struct {
    QCU_CR_TypeDef  CR;
    CMD_MAX_TypeDef CMD;
} QCU_Init_TypeDef;

/**
\brief QCU communication structure definition 
*/
typedef struct {
    uint8_t  DDRMODE;
    uint8_t  BIT_30;        /* IDMODE for OMCR or NOWRAP for RMCR */
    uint8_t  BIT_29;        /* XIPIMMENTER for OMCR or WRAPMODE for RMCR */
    uint8_t  NUMDC;

    uint8_t  RXDLY;
    uint8_t  DSIZE;
    uint8_t  DMODE;
    uint8_t  ABSIZE;

    uint8_t  ABMODE;
    uint8_t  ADSIZE;
    uint8_t  ADMODE;
    uint8_t  IMODE;

    uint8_t  INSTRUCTION;

    uint8_t  ALT;           /* Alternate bytes */
} QCU_Comm_Typedef;

/* bits definition of QCU control register (QCU_CR) */
#define SCKSCALER_POS   24
#define SCKSCALER_MSK   (0x3Ful << SCKSCALER_POS)          /*Clock prescaler.
                                                            This field defines the scaler factor for generating SCK based on the clock 2*(value+1).
                                                            0: Fsck = F/2
                                                            1: Fsck = F/4
                                                            2: Fsck = F/6
                                                            3: Fsck = F/8
                                                            …
                                                            63: Fsck = F/128
                                                            SCK’s duty cycle is always 50%. This field can be modified only when QCU is in inactive mode.
                                                            */
#define CSRHT_POS       16
#define CSRHT_MSK       (0x3ul << CSRHT_POS)                /* CSRHT+1 defines the minimum number of CLK cycles which the chip select (CSn) must remain high 
                                                            between commands issued to the Flash device memory.
                                                            0: CSn stays high for at least 1 cycle between Flash memory commands
                                                            1: CSn stays high for at least 2 cycle between Flash memory commands
                                                            …
                                                            7: CSn stays high for at least 8 cycle between Flash memory commands
                                                            */
#define FMSIZE_POS      8
#define FMSIZE_MSK      (0x1Ful << FMSIZE_POS)              /*Flash memory size
                                                            This field defines the size of external memory using the following formula:
                                                            Number of bytes in Flash memory = 2**[FMSIZE+1]
                                                            FMSIZE+1 is effectively the number of address bits required to address the Flash memory. 
                                                            The Flash memory capacity can be up to 4GB (addressed using 32bits), 
                                                            but in the direct Read/Program mode is limited to 512MB, since the C-AHB memory spaces are 512MB.
                                                            default value of these fields is 28(2**29, 512MB). These fields should be set based on the actual 
                                                            memory sizes after power-on by software. 
                                                            If direct read/write address overhead this region, AHB bus will response error to indicate transaction fail.
                                                            */
#define XIPMODE_POS     5
#define XIPMODE_MSK     (0x1ul << XIPMODE_POS)              /*XIP mode select in direct read access mode.
                                                            This bit indicates that the flash device memory supports the XIP mode. 
                                                            The read instruction only needs to transfer once. Usually the flash memory needs to configurate to XIP mode first.
                                                            0: The flash device memory doesn’t in XIP mode
                                                            1: The flash device memory in XIP mode.
                                                            This field can be modified only when the operation mode enters inactive state and BUSY = 0.
                                                            This field is valid only in Direct read access mode.
                                                            */
#define SCKMODE_POS     4
#define SCKMODE_MSK     (0x1ul << SCKMODE_POS)              /*SCK MODE select, Mode0/Mode3
                                                            This bit indicates the level that SCK takes between instructions when CSn inactive
                                                            0: SCK must stay low while CSn is inactive. This is referred to as mode 0.
                                                            1: SCK must stay high while CSn is inactive. This is referred to as mode 3.
                                                            This field can be modified only when the FSM enters inactive state.
                                                            */
#define OPMODE_POS      0
#define OPMODE_MSK      (0x3ul << OPMODE_POS)               /*Operation mode select.
                                                            2’b00: Direct read access mode.
                                                            2’b01: Direct program access mode, when a block data is program it will 
                                                                   come back to last operation mode automatically (Direct read access mode or inactive state).
                                                            2’b10: Indirect mode, when an instruction is done, it will come back to last operation mode automatically 
                                                                   (Direct read access mode or inactive state).
                                                            2’b11: Inactive mode, usually use to consist configuring the control register.
                                                            When last operation mode which is indirect mode or direct program mode is on going transfer, 
                                                            writing this field to inactive mode, and if the indirect mode transaction is not finished, 
                                                            there will be generate a transaction abortion pending (TEF).
                                                            This field could be modified anytime.
                                                            when write to 2’b00, QCU_RABR/QCU_RMCR would be updated to control new transfer.
                                                            when write to 2’b01, QCU_OMCR/QCU_OABR/QCU_DLR would be updated to control new transfer.
                                                            when write to 2’b10, QCU_OMCR/QCU_OABR/QCU_IMAR/QCU_DLR would be updated to control new transfer.
                                                            when write to 2’b11, no shadow registers would be updated.
                                                            */

/* bits definition of QCU status register (QCU_SR) */
#define FIFOLEVEL_POS   12
#define FIFOLEVEL_MSK   (0xFul << FIFOLEVEL_POS)            /*This field gives the number of valid bytes which are being held in the FIFO.
                                                            FIFOLEVEL = 0 when the FIFO is empty, and FIFOLEVEL = FIFO_DEPTH when the FIFO is full. 
                                                            In read mode, it means there are FIFOLEVEL data in the FIFO needs to be read back while in write mode,
                                                            it means there are FIFOLEVEL data in the FIFO needs to send.
                                                            These bits are used in indirect mode
                                                            */
#define FIFOFULL_POS    9
#define FIFOFULL_MSK    (0x1ul << FIFOFULL_POS)             /*
                                                            This bit is set when the FIFO is full. This bit usually uses in indirect mode.
                                                            */
#define FIFOEMPTY_POS   8
#define FIFOEMPTY_MSK   (0x1ul << FIFOEMPTY_POS)            /*
                                                            This bit is set when the FIFO is empty. This bit usually uses in indirect mode.
                                                            */
#define XIPSTATUS_POS   6
#define XIPSTATUS_MSK   (0x1ul << XIPSTATUS_POS)            /*
                                                            This bit is indicated that core is in XIP mode.
                                                            when XPR function is enable, and QCU enters into XPR mode, this bit will change 
                                                            to 1 to indicate QCU is in XIP mode and instruction phase will be discarded.
                                                            When QCU_OMCR.XIPIMMENTER is set, and FSM comes back from indirect access mode, 
                                                            this bit will be set to 1. Otherwise this bit will be clear to 0. This bit will be 
                                                            set/clear with TCF set at the same time.
                                                            RO
                                                            */
#define BUSY_POS        5
#define BUSY_MSK        (0x1ul << BUSY_POS)                 /*
                                                            This bit is set when an operation on QSPI-Bus is on going. This bit clears automatically 
                                                            when the operation on QSPI-Bus is finished and the FIFO is empty in indirect mode
                                                            */
#define TCF_POS         4
#define TCF_MSK         (0x1ul << TCF_POS)                  /*
                                                            Transfer complete flag
                                                            This bit is set in indirect mode when the programmed number of data has been 
                                                            transferred or in direct program mode when the transfer has been aborted. It is cleared by writing 1 to TCF.
                                                            */
#define OPCRCF_POS      2
#define OPCRCF_MSK      (0x1ul << OPCRCF_POS)               /*
                                                            Operation mode change request complete flag
                                                            It indicates that latest operation mode change request has been granted.
                                                            When writing QCU_CR register, this bit will clear to 0 automatically. When operation mode has been changed, this bit will set to 1.
                                                            RO
                                                            */
#define CUR_OP_POS      0
#define CUR_OP_MSK      (0x3ul << CUR_OP_POS)               /*current operation mode.
                                                            2’b00: direct read mode
                                                            2’b01: Reserved
                                                            2’b10: indirect access mode
                                                            2’b11: inactive mode
                                                            RO
                                                            */

/* bits definition of QCU Direct Read Access Mode Control register(QCU_RMCR) and QCU Indirect Mode Control register(QCU_OMCR) */
#define DDRMODE_POS     31
#define DDRMODE_MSK     (0x1ul << DDRMODE_POS)              /*
                                                            This bit sets the DDR mode for the address, alternate byte and data phase:
                                                            0: DDR mode disabled
                                                            1: DDR mode enabled
                                                            */
#define NOWRAP_POS      30
#define NOWRAP_MSK      (0x1ul << NOWRAP_POS)               /*
                                                            QCU not support WRAP in direct read mode
                                                            This bit indicates that whether QCU should support wrap mode.
                                                            0: QCU should follow the AHB transaction, and critical word reads back first.
                                                            1: QCU doesn’t follow the AHB transaction always, 
                                                            and it could be read in 8-words boundary, and the critical word may not be read back first.
                                                            This bit could improve read efficiency in some scenario.
                                                            */
#define WRAPMODE_POS    29
#define WRAPMODE_MSK    (0x1ul << WRAPMODE_POS)             /*
                                                            Flash memory device supports wrap transaction mode select in direct read access mode.
                                                            This bit indicates that the flash device memory supports wrap transaction mode. 
                                                            The wrapping bursts read transaction would wrap at 32-bytes address boundaries.
                                                            0: The flash device memory doesn’t in wrap mode
                                                            1: The flash device memory in wrap mode.
                                                            This field can be modified only when the operation mode enters inactive state and BUSY = 0.
                                                            This field is valid only in Direct read access mode.
                                                            The burst wrap type only supports 8-words to suit the cache line length.
                                                            */
#define NUMDC_POS       24
#define NUMDC_MSK       (0x1Ful << NUMDC_POS)               /*
                                                            Number of dummy cycles
                                                            This field defines the duration of the dummy phase. 
                                                            In both SDR and DDR modes, it specifies a number of SCK cycles (0~31).
                                                            */
#define RXDLY_POS       22
#define RXDLY_MSK       (0x3ul << RXDLY_POS)                /*
                                                            RX delay
                                                            This field determines the timing for the sampling of the input serial data. 
                                                            It specifies the number of delay steps for capturing the serial data inputs. 
                                                            Each delay step is equal to one core bus clock cycle. If set to 0, input serial 
                                                            data is captured with the rising edge of the generated SPI clock. Supported values range from 0 to 3.
                                                            00: 0 AHB bus clock delay
                                                            01: 1 AHB bus clock delay
                                                            10: 2 AHB bus clock delay
                                                            11: 3 AHB bus clock delay
                                                            */
#define DSIZE_POS       20
#define DSIZE_MSK       (0x3ul << DSIZE_POS)                /*
                                                            Data size
                                                            This field defines the data size:
                                                            00: Reserved
                                                            01: Reserved
                                                            10: Reserved
                                                            11: 32 bits data
                                                            Only supports 32 bits data. if direct read mode accepts a non-32 bits transaction, it will response error
                                                            */
#define DMODE_POS       18
#define DMODE_MSK       (0x3ul << DMODE_POS)                /*
                                                            Data mode
                                                            This field defines the data phase’s mode of operation:
                                                            00: Reserve
                                                            01: Data on a single line
                                                            10: Data on two lines
                                                            11: Data on quad lines
                                                            This field also determines the dummy phase mode of operation.
                                                            */
#define ABSIZE_POS      16
#define ABSIZE_MSK      (0x3ul << ABSIZE_POS)               /*
                                                            Alternate bytes size
                                                            This field defines the address size:
                                                            00: 8-bit alternate bytes
                                                            01: 16-bit alternate bytes
                                                            10: 24-bit alternate bytes
                                                            11: 32-bit alternate bytes
                                                            */
#define ABMODE_POS      14
#define ABMODE_MSK      (0x3ul << ABMODE_POS)               /*
                                                            Alternate bytes mode
                                                            This field defines the address phase’s mode of operation:
                                                            00: No alternate bytes
                                                            01: Alternate bytes on a single line
                                                            10: Alternate bytes on two lines
                                                            11: Alternate bytes on quad lines
                                                            */
#define ADSIZE_POS      12
#define ADSIZE_MSK      (0x3ul << ADSIZE_POS)               /*
                                                            Address size
                                                            This field defines the address size:
                                                            00: Reserved
                                                            01: Reserved
                                                            10: 24-bit address
                                                            11: 32-bit address
                                                            */
#define ADMODE_POS      10
#define ADMODE_MSK      (0x3ul << ADMODE_POS)               /*
                                                            Address mode
                                                            This field defines the address phase’s mode of operation:
                                                            00: No address bytes
                                                            01: Address on a single line
                                                            10: Address on two lines
                                                            11: Address on quad lines
                                                            */
#define IMODE_POS       8
#define IMODE_MSK       (0x3ul << IMODE_POS)                /*
                                                            Instruction mode
                                                            This field defines the instruction phase mode of operation:
                                                            00: No instruction bytes
                                                            01: Instruction on a single line
                                                            10: Instruction on two lines
                                                            11: Instruction on quad lines
                                                            */
#define INSTRUCTION_POS 0
#define INSTRUCTION_MSK (0xFFul << INSTRUCTION_POS)         /*
                                                            Direct read access mode instruction send to SPI device
                                                            Arbitrary instruction to be send to the external flash device memory in any access mode.
                                                            */
#define IDMODE_POS      30
#define IDMODE_MSK      (0x1ul  << IDMODE_POS)              /*Indirect access mode data phase direction
                                                            0: Read data, RX FIFO enable
                                                            1: Write data, TX FIFO enable
                                                            This bit is valued only when OPMODE_SEL = 2’b10. There is no effect in other operation mode.
                                                            This field can be modified only when the FSM enters inactive state.
                                                            This field also enables RX/TX FIFO.
                                                            When there is data in the RX FIFO, APB Bus should read empty the RX FIFO, then set the bit to 1 to enable TX FIFO.
                                                            When there is data in the TX FIFO, QCU should read empty the TX FIFO, then set the bit to 0 to enable RX FIFO
                                                            */
#define XIPIMMENTER_POS 29
#define XIPIMMENTER_MSK (0x1ul << XIPIMMENTER_POS)

/* bits definition of QCU direct read mode alternate bytes register (QCU_RABR) and QCU indirect mode alternate bytes register (QCU_OABR) */
#define ALT_POS         0
#define ALT_MSK         (0xFFFFFFFFul << ALT_POS)           /*
                                                            Alternate bytes.
                                                            Optional data to be send to the external SPI device after the address phase. 
                                                            QCU will be according to ABMODE & ABSIZE in QCU_DMCR to send LSB 0/8/16/24/32 bits to flash. 
                                                            This field could be used in Direct/Indirect access mode
                                                            */

/* bits definition of QCU Indirect mode Address register (QCU_IMAR) */
#define IMADDR_POS      0
#define IMADDR_MSK      (0xFFFFFFFFul << IMADDR_POS)        /*
                                                            Address to be sent to the external SPI device. 
                                                            This field is only used in indirect access mode. 
                                                            It is recommended to write to these fields in inactive mode and BUSY =0.
                                                            */

/* bits definition of QCU FIFO DATA register (QCU_FDR) */
#define FIFODATA_POS    0
#define FIFODATA_MSK    (0xFFFFFFFFul << FIFODATA_POS)      /*
                                                            Data to be sent/received to/from the external SPI device. This field is only used in indirect access mode. 
                                                            In indirect write mode, data write to this register is stored on the FIFO before it is sent to the flash 
                                                            memory during the data phase. If the FIFO is full, a write operation is stalled until the FIFO is not full.
                                                            In indirect read mode, reading this register gives the data which is received from the flash memory. 
                                                            If the FIFO is empty, the read operation is hold for next data.
                                                            FIFO will hold the APB bus transaction to hid the hardware operation. 
                                                            If RX FIFO is empty while CU reads and TX FIFO is full while CU writes, 
                                                            the pready signal would be pull low to hold the transaction.
                                                            Word, halfword and byte accesses to this register are supported. 
                                                            But halfword and byte accesses are organized in word. For example, 3 bytes need APB to read 3 times in byte mode. 
                                                            4 bytes need APB to read 2 times in halfword mode.
                                                            */

/* bits definition of QCU Data Length register (QCU_DLR) */
#define DL_POS          0
#define DL_MSK          (0xFFFFFul << DL_POS)               /*
                                                            Transfer data length
                                                            Number of data to be retrieved (DL + 1) in indirect read/write mode and direct program mode.
                                                            0: 1 byte is to be transferred
                                                            1: 2 bytes is to be transferred
                                                            2: 3 bytes is to be transferred
                                                            …
                                                            0xfffff: 1,048,576(1M) bytes is to be transferred
                                                            This field has no effect when in direct read mode.
                                                            This field can be written only in inactive mode.
                                                            */

/* QCU_CR configuration */
#define XIPMODE_EXIT                0x0
#define XIPMODE_ENTER               0x1
#define SCKMODE_MODE0               0x0
#define SCKMODE_MODE1               0x1
#define DIRECT_READ_MODE            0x00
#define INDIRECT_MODE               0x02
#define INACTIVE_MODE               0x03

/* QCU_RMCR and QCU_OMCR configuration */
#define DDRMODE_DISABLE             0x0
#define DDRMODE_ENABLE              0x1

/* RMCR-NOWRAP */
#define NOWRAP_DEFAULT              0x0
/* RMCR-WRAPMODE */
#define WRAPMODE_DEFAULT            0x0

/* OMCR-IDMODE */
#define IDMODE_READ                 0x0
#define IDMODE_WRITE                0x1
/* OMCR-XIPIMMENTER */
#define XIP_BIT_CLEAR               0x0
#define XIP_BIT_SET                 0x1

#define NUMDC_DEFAULT               0x0
#define RXDLY_DEFAULT               0x0

#define DSIZE_8BITS                 0x0
#define DSIZE_16BITS                0x1
#define DSIZE_32BITS                0x3

#define DMODE_NO_DATA               0x0
#define DMODE_SINGLE_LINE_DATA      0x1
#define DMODE_DUAL_LINE_DATA        0x2
#define DMODE_QUAD_LINE_DATA        0x3

#define ABSIZE_ALT_BYTES_8BITS      0x0
#define ABSIZE_ALT_BYTES_16BITS     0x1
#define ABSIZE_ALT_BYTES_24BITS     0x2
#define ABSIZE_ALT_BYTES_32BITS     0x3

#define ABMODE_NONE                 0x0
#define ABMODE_SINGLE_LINE          0x1
#define ABMODE_DUAL_LINES           0x2
#define ABMODE_QUAD_LINES           0x3

#define ADSIZE_8BITS                0x0
#define ADSIZE_16BITS               0x1
#define ADSIZE_24BITS               0x2
#define ADSIZE_32BITS               0x3

#define ADMODE_NONE                 0x0
#define ADMODE_SINGLE_LINE          0x1
#define ADMODE_DUAL_LINES           0x2
#define ADMODE_QUAD_LINES           0x3

#define IMODE_NONE                  0x0
#define IMODE_SINGLE_LINE           0x1
#define IMODE_DUAL_LINES            0x2
#define IMODE_QUAD_LINES            0x3


#define QCU_DIRECT_READ             0x0
#define QCU_INDIRECT_READ           0x1
#define QCU_INDIRECT_WRITE          0x2
#define QCU_INACTIVE                0x3

#define TCF_TIMEOUT                 0xFFFF
#define OPCRCF_TIMEOUT              0xFFFF
#define FIFO_TIMEOUT                1000
#define FIFOFULL_FLAG               0x1


#ifdef  __cplusplus
}
#endif

#endif /* QCU_COMMON_H_ */
