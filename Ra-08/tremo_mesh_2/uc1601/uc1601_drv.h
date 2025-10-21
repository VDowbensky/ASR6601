#ifndef _UC1601_H_
#define _UC1601_H_

#include "uc1601_interface.h"

//! Write Data Byte to Memory  CD=1,WR=0
#define UC1601_WRITE_DATA       0x00 
//! Read Data Byte from Memory  CD=1,WR=1
#define UC1601_READ_DATA        0x00 
//! Get Status  CD=0,WR=1
#define UC1601_GET_STATUS       0x00
//! Set Column Address LSB  CD=0,WR=0
#define UC1601_SET_CA_LSB       0x00  
//! Set Column Address MSB  CD=0,WR=0
#define UC1601_SET_CA_MSB       0x10 
//! Set Column Address MSB or LSB MASK
#define UC1601_SET_CA_MASK      0x0F 
//! Set Temp. Compensation  CD=0,WR=0
#define UC1601_SET_TC           0x24
#define UC1601_SET_TC_MASK      0x03
//! Set Power Control  CD=0,WR=0
#define UC1601_SET_PC           0x28
#define UC1601_SET_PC_MASK      0x07
//! Set Scroll Line  CD=0,WR=0
#define UC1601_SET_SL           0x40
#define UC1601_SET_SL_MASK      0x1F
//! Set Page Address  CD=0,WR=0
#define UC1601_SET_PA           0xB0
#define UC1601_SET_PA_MASK      0x0F
//! Set VBIAS Potentiometer(double-byte command)  CD=0,WR=0
#define UC1601_SET_PM           0x81
#define UC1601_SET_PM_MASK      0xFF
//! Set Partial Display Control  CD=0,WR=0
#define UC1601_SET_PD_EN        0x85
#define UC1601_SET_PD_DIS       0x84
//! Set RAM Address Control  CD=0,WR=0
#define UC1601_SET_AC           0x88
#define UC1601_SET_AC_MASK      0x07
//! Automatic column/page wrap around.
//! 0: CA or PA (depends on AC[1]= 0 or 1) will stop increasing after reaching
//! boundary
//! 1: CA or PA (depends on AC[1]= 0 or 1) will restart, and CA or PA will 
//! increase by one.
#define UC1601_SET_AC_0         0x01
//! Auto-Increment order
//! 0 : column (CA) increasing (+1) first until CA reach CA boundary, then PA
//! will increase by (+/-1).
//! 1 : page (PA) increasing (+/-1) first until PA reach PA boundary, then CA 
//! will increase by (+1).
#define UC1601_SET_AC_1         0x02
//! PID, page address (PA) auto increment direction ( 0/1 = +/- 1 )
//! When WA=1 and CA reaches CA boundary, PID controls whether page address 
//! will be adjusted by +1 or -1.
#define UC1601_SET_AC_2         0x04
//! Set Frame Rate  CD=0,WR=0
#define UC1601_SET_LC3          0xA0
//! Frame Rate IS 80 frame-per-second
#define UC1601_SET_FR_80        0xA0
//! Frame Rate is 100 frame-per-second
#define UC1601_SET_FR_100       0xA1
//! Set All Pixel ON  CD=0,WR=0
//! Set DC[1] to force all SEG drivers to output ON signals.
#define UC1601_SET_DC1          0xA4
#define UC1601_SET_DC1_EN       0xA5
//! Set Inverse Display  CD=0,WR=0
//! Set DC[0] to force all SEG drivers to output the inverse of the data 
//! (bit-wise) stored in display RAM.
#define UC1601_SET_DC0          0xA6
#define UC1601_SET_DC0_EN       0xA7
//! Set Display Enable  CD=0,WR=0
//! This command is for programming register DC[2]. When DC[2] is set to 1, 
//! UC1601s will first exit from sleep
//! mode, restore the power and then turn on COM drivers and SEG drivers.
#define UC1601_SET_DC2          0xAE
#define UC1601_SET_DC2_EN       0xAF
//! Set LCD Mapping Control  CD=0,WR=0
//! Set LC[2:1] for COM (row) mirror (MY), SEG (column) mirror (MX).
#define UC1601_SET_LC21         0xC0
#define UC1601_SET_LC21_MASK    0x06
//! System Reset  CD=0,WR=0
//! Control register values will be reset to their default values.
#define UC1601_SET_SC           0xE2
//! No Operation  CD=0,WR=0
#define UC1601_SET_NOP          0xE3
//! Set LCD Bias Ratio  CD=0,WR=0
#define UC1601_SET_BR           0xE8
#define UC1601_SET_BR_6         0xE8
#define UC1601_SET_BR_7         0xE9
#define UC1601_SET_BR_8         0xEA
#define UC1601_SET_BR_9         0xEB
//! Set COM End(Double-byte command)  CD=0,WR=0
#define UC1601_SET_CEN          0xF1
#define UC1601_SET_CEN_MASK     0x7F
//! Set Partial Display Start(Double-byte command)  CD=0,WR=0
#define UC1601_SET_DST          0xF2
#define UC1601_SET_DST_MASK     0x7F
//! Set Partial Display End(Double-byte command)  CD=0,WR=0
#define UC1601_SET_DEN          0xF3
#define UC1601_SET_DEN_MASK     0x7F
//! Set Partial Display End(Double-byte command)  CD=0,WR=0
#define UC1601_SET_DEN          0xF3
#define UC1601_SET_DEN_MASK     0x7F
//! Read Data Byte from Memory(Double-byte command)  CD=1,WR=1
#define UC1601_READ_DATA_CMD    0xFF
#define UC1601_READ_DATA_MASK   0xFF
//! Get Status(Double-byte command)  CD=0,WR=1
#define UC1601_GET_STATUS_CMD   0xFE
#define UC1601_READ_DATA_MASK   0xFF

void UC1601Init(void);
void UC1601AddressSet(uint8_t ucPA, uint8_t ucCA);
void UC1601Display(uint8_t ucLine, uint8_t ucRow, uint8_t ucAsciiWord);
void UC1601InverseDispaly(uint8_t ucLine, uint8_t ucRow, uint8_t ucAsciiWord);
void UC1601CharDispaly(uint8_t ucLine, uint8_t ucRow, char *pcChar);
void UC1601ChineseDispaly(uint8_t ucLine, uint8_t ucRow, uint8_t ucLength, char *pcChar);
void HD44780DisplayN(uint8_t ucLine, uint8_t ucRow, uint32_t n);
void UC1601Clear(void);
void UC1601InverseEnable(void);
void UC1601InverseDisable(void);
void UC1601AllPixelOnEnable(void);
void UC1601AllPixelOnDisable(void);
void UC1601DisplayOn(void);
void UC1601DisplayOff(void);
void UC1601ScrollLineSet(uint8_t ucLine);
void UC1601PMSet(uint8_t ucPM);
void UC1601CENSet(uint8_t ucCEN);
void UC1601DSTSet(uint8_t ucDST);
void UC1601DENSet(uint8_t ucDEN);

#endif

/****************************EOF****************************/
