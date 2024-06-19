#ifndef _SX126XX_H_
#define _SX126XX_H_

//#include <stdint.h>
//#include <stdbool.h>
#include "bsp.h"
#include "lorac.h"
//#include "eeprom.h"
#define    FXO 32000000UL

#define OPMODE_SLEEP            0
#define OPMODE_STBYRC           1
#define OPMODE_STBYXOSC         2
#define OPMODE_FS               3
#define OPMODE_TX               4
#define OPMODE_RX               5
#define OPMODE_TXSTREAMCW       6
#define OPMODE_TXSTREAMPRE      7

#define MODEM_FSK		0
#define MODEM_LORA		1
#define MODEM_FHSS		3

//IRQ mask
#define    TXDONE_IRQMSK                          0x01 //All
#define    RXDONE_IRQMSK                          0x02 //All
#define    RPEDET_IRQMSK                          0x04 //All
#define    SYNCDET_IRQMSK                         0x08 //FSK
#define    HEADERDET_IRQMSK                       0x10 //LoRa
#define    HEADERERR_IRQMSK                       0x20 //LoRa
#define    CRCERR_IRQMSK                          0x40 //All
#define    CADDONE_IRQMSK                         0x80 //LoRa
#define    CADDET_IRQMSK                          0x100 //LoRa
#define    TIMEOUT_IRQMSK                         0x200 //All
#define    LRFHSSHOP_IRQMSK                       0x4000 //LR-FHSS
#define    ALL_IRQMSK                             0x7fff

//Radio events

//Commands
#define    CMD_GET_STATUS                         0xC0//GetStatus
#define    CMD_WRITE_REGISTER                     0x0D
#define    CMD_READ_REGISTER                      0x1D
#define    CMD_WRITE_BUFFER                       0x0E
#define    CMD_READ_BUFFER                        0x1E
#define    CMD_SET_SLEEP                          0x84//SetSleep
#define    CMD_SET_STANDBY                        0x80//SetStandby
#define    CMD_SET_FS                             0xC1//SetFs
#define    CMD_SET_TX                             0x83//SetTx
#define    CMD_SET_RX                             0x82//SetRx
#define    CMD_SET_RXDUTYCYCLE                    0x94//SetRxDutyCycle
#define    CMD_SET_CAD                            0xC5//SetCad
#define    CMD_SET_TXCONTINUOUSWAVE               0xD1//SetTxContinuousWave
#define    CMD_SET_TXCONTINUOUSPREAMBLE           0xD2//SetTxInfinitePreamble
#define    CMD_UPDATE_PRAM                        0xD9//Apply PRAM patch

#define    CMD_SET_PACKETTYPE                     0x8A//SetPacketType
#define    CMD_GET_PACKETTYPE                     0x11//GetPacketType
#define    CMD_SET_RFFREQUENCY                    0x86//SetRfFrequency
#define    CMD_SET_TXPARAMS                       0x8E//SetTxParams
#define    CMD_SET_PACONFIG                       0x95//SetPaConfig
#define    CMD_SET_CADPARAMS                      0x88
#define    CMD_SET_BUFFERBASEADDRESS              0x8F
#define    CMD_SET_MODULATIONPARAMS               0x8B//SetModulationParams
#define    CMD_SET_PACKETPARAMS                   0x8C
#define    CMD_GET_RXBUFFERSTATUS                 0x13//GetRxBufferStatus
#define    CMD_GET_PACKETSTATUS                   0x14//GetPacketStatus
#define    CMD_GET_RSSIINST                       0x15//GetRssiInst
#define    CMD_GET_STATS                          0x10//GetStats
#define    CMD_RESET_STATS                        0x00//ResetStats
#define    CMD_CFG_DIOIRQ                         0x08//SetDioIrqParams

#define    CMD_GET_IRQSTATUS                      0x12//GetIrqStatus
#define    CMD_CLR_IRQSTATUS                      0x02//ClearIrqStatus
#define    CMD_CALIBRATE                          0x89//Calibrate
#define    CMD_CALIBRATEIMAGE                     0x98//CalibrateImage
#define    CMD_SET_REGULATORMODE                  0x96//SetRegulatorMode
#define    CMD_GET_ERROR                          0x17//GetDeviceErrors
#define    CMD_CLR_ERROR                          0x07//ClearDeviceErrors
#define    CMD_SET_TCXOMODE                       0x97//SetDIO3AsTcxoCtrl
#define    CMD_SET_TXFALLBACKMODE                 0x93//SetRxTxFallbackMode
#define    CMD_SET_RFSWITCHMODE                   0x9D//SetDIO2AsRfSwitchCtrl
#define    CMD_SET_STOPRXTIMERONPREAMBLE          0x9F//StopTimerOnPreamble
#define    CMD_SET_LORASYMBTIMEOUT                0xA0

//Registers
#define REG_HoppingEnable               0x0385
#define REG_PacketLength                0x0386
#define REG_NbHoppingBlocks             0x0387

#define REG_NbSymbols0_H                0x0388
#define REG_NbSymbols0_L                0x0389
#define REG_Freq0_H                     0x038a
#define REG_Freq0_MH                    0x038b
#define REG_Freq0_ML                    0x038c
#define REG_Freq0_L                     0x038d

#define REG_NbSymbols1_H                0x038e
#define REG_NbSymbols1_L                0x038f
#define REG_Freq1_H                     0x0390
#define REG_Freq1_MH                    0x0391
#define REG_Freq1_ML                    0x0392
#define REG_Freq1_L                     0x0393

#define REG_NbSymbols2_H                0x0394
#define REG_NbSymbols2_L                0x0395
#define REG_Freq2_H                     0x0396
#define REG_Freq2_MH                    0x0397
#define REG_Freq2_ML                    0x0398
#define REG_Freq2_L                     0x0399

#define REG_NbSymbols3_H                0x039a
#define REG_NbSymbols3_L                0x039b
#define REG_Freq3_H                     0x039c
#define REG_Freq3_MH                    0x039d
#define REG_Freq3_ML                    0x039e
#define REG_Freq3_L                     0x039f

#define REG_NbSymbols4_H                0x03a0
#define REG_NbSymbols4_L                0x03a1
#define REG_Freq4_H                     0x03a2
#define REG_Freq4_MH                    0x03a3
#define REG_Freq4_ML                    0x03a4
#define REG_Freq4_L                     0x03a5

#define REG_NbSymbols5_H                0x03a6
#define REG_NbSymbols5_L                0x03a7
#define REG_Freq5_H                     0x03a8
#define REG_Freq5_MH                    0x03a9
#define REG_Freq5_ML                    0x03aa
#define REG_Freq5_L                     0x03ab

#define REG_NbSymbols6_H                0x03ac
#define REG_NbSymbols6_L                0x03ad
#define REG_Freq6_H                     0x03ae
#define REG_Freq6_MH                    0x03af
#define REG_Freq6_ML                    0x03b0
#define REG_Freq6_L                     0x03b1

#define REG_NbSymbols7_H                0x03b2
#define REG_NbSymbols7_L                0x03b3
#define REG_Freq7_H                     0x03b4
#define REG_Freq7_MH                    0x03b5
#define REG_Freq7_ML                    0x03b6
#define REG_Freq7_L                     0x03b7

#define REG_NbSymbols8_H                0x03b8
#define REG_NbSymbols8_L                0x03b9
#define REG_Freq8_H                     0x03ba
#define REG_Freq8_MH                    0x03bb
#define REG_Freq8_ML                    0x03bc
#define REG_Freq8_L                     0x03bd

#define REG_NbSymbols9_H                0x03be
#define REG_NbSymbols9_L                0x03bf
#define REG_Freq9_H                     0x03c0
#define REG_Freq9_MH                    0x03c1
#define REG_Freq9_ML                    0x03c2
#define REG_Freq9_L                     0x03c3

#define REG_NbSymbols10_H               0x03c4
#define REG_NbSymbols20_L               0x03c5
#define REG_Freq10_H                    0x03c6
#define REG_Freq10_MH                   0x03c7
#define REG_Freq10_ML                   0x03c8
#define REG_Freq10_L                    0x03c9

#define REG_NbSymbols11_H               0x03ca
#define REG_NbSymbols11_L               0x03cb
#define REG_Freq11_H                    0x03cc
#define REG_Freq11_MH                   0x03cd
#define REG_Freq11_ML                   0x03ce
#define REG_Freq11_L                    0x03cf

#define REG_NbSymbols12_H               0x03d0
#define REG_NbSymbols12_L               0x03d1
#define REG_Freq12_H                    0x03d2
#define REG_Freq12_MH                   0x03d3
#define REG_Freq12_ML                   0x03d4
#define REG_Freq12_L                    0x03d5

#define REG_NbSymbols13_H               0x03d6
#define REG_NbSymbols13_L               0x03d7
#define REG_Freq13_H                    0x03d8
#define REG_Freq13_MH                   0x03d9
#define REG_Freq13_ML                   0x03da
#define REG_Freq13_L                    0x03db

#define REG_NbSymbols14_H               0x03de
#define REG_NbSymbols14_L               0x03df
#define REG_Freq14_H                    0x03e0
#define REG_Freq14_MH                   0x03e1
#define REG_Freq14_ML                   0x03e2
#define REG_Freq14_L                    0x03e3

#define REG_NbSymbols15_H               0x03e4
#define REG_NbSymbols15_L               0x03e5
#define REG_Freq15_H                    0x03e6
#define REG_Freq15_MH                   0x03e7
#define REG_Freq15_ML                   0x03e8
#define REG_Freq15_L                    0x03e9

#define REG_DIOOUT_EN                   0x0580
#define REG_DIOIN_EN                    0x0583
#define REG_DIOPU_CTRL                  0x0584
#define REG_DIOPD_CTRL                  0x0585


#define REG_BITSYNC                     0x06ac
#define REG_WHITEINIT_H                 0x06b8
#define REG_WHITEINIT_L                 0x06b9

#define REG_CRCINIT_H                   0x06bc
#define REG_CRCINIT_L                   0x06bd
#define REG_CRCPOLY_H                   0x06be
#define REG_CRCPOLY_L                   0x06bf

#define REG_SYNC0                       0x06c0
#define REG_SYNC1                       0x06c1
#define REG_SYNC2                       0x06c2
#define REG_SYNC3                       0x06c3
#define REG_SYNC4                       0x06c4
#define REG_SYNC5                       0x06c5
#define REG_SYNC6                       0x06c6
#define REG_SYNC7                       0x06c7

#define REG_NODEADDR                    0x06cd
#define REG_BROADCASTADDR               0x06ce

#define REG_GAFC                        0x06d1

#define REG_LPLDLEN                     0x0702
#define REG_LSYNCTIMEOUT                0x0706

#define REG_IQPOL                       0x0736
#define REG_LRSYNC_H                    0x0740
#define REG_LRSYNC_L                    0x0741

#define REG_RND0                        0x0819
#define REG_RND1                        0x081a
#define REG_RND2                        0x081b
#define REG_RND3                        0x081c

#define REG_TXMOD                       0x0889
#define REG_PLLSTEP                     0x0889
#define REG_RXGAIN                      0x08ac

#define REG_TXCLAMPCONFIG               0x08d8
#define REG_OCPCONFIG                   0x08e7
#define REG_RTCCTRL                     0x0902
#define REG_XTATRIM                     0x0911
#define REG_XTBTRIM                     0x0912
#define REG_DIO3VOUTCTRL                0x0920
#define REG_EVENTMASK                   0x0944

#define LORA_BW_7p8             0
#define LORA_BW_10p4            0x08
#define LORA_BW_15p6            0x01
#define LORA_BW_20p8            0x09
#define LORA_BW_31p3            0x02
#define LORA_BW_41p7            0x0a
#define LORA_BW_62p5            0x03
#define LORA_BW_125             0x04
#define LORA_BW_250             0x05
#define LORA_BW_500             0x06

#define FSK_BW_4p8              0x1f
#define FSK_BW_5p8              0x17
#define FSK_BW_7p3              0x0f
#define FSK_BW_9p7              0x1e
#define FSK_BW_11p7             0x16
#define FSK_BW_14p6             0x0e
#define FSK_BW_19p5             0x1d
#define FSK_BW_23p4             0x15
#define FSK_BW_29p3             0x0d
#define FSK_BW_39               0x1c
#define FSK_BW_46p9             0x14
#define FSK_BW_58p6             0x0c
#define FSK_BW_78p2             0x1b
#define FSK_BW_93p8             0x13
#define FSK_BW_117p3            0x0b
#define FSK_BW_156p2            0x1a
#define FSK_BW_187p2            0x12
#define FSK_BW_234p3            0x0a
#define FSK_BW_312              0x19
#define FSK_BW_373p6            0x11
#define FSK_BW_467              0x09
#define FSK_BW_INVALID          0x00

#define FSK_BT0                 0
#define FSK_BT0p3               0x08
#define FSK_BT0p5               0x09
#define FSK_BT0p7               0x0a
#define FSK_BT1                 0x0b

#define FSK_CRC_OFF             1
#define FSK_CRC_1B              0
#define FSK_CRC_2B              2
#define FSK_CRC_1B_INV          4
#define FSK_CRC_2B_INV          6

#define LORA_CR_4_5             1
#define LORA_CR_4_6             2
#define LORA_CR_4_7             3
#define LORA_CR_4_8             4

#define RAMP_10U                0x00 //10
#define RAMP_20U                0x01 //20
#define RAMP_40U                0x02 //40
#define RAMP_80U                0x03 //80
#define RAMP_200U               0x04 //200
#define RAMP_800U               0x05 //800
#define RAMP_1700U              0x06 //1700
#define RAMP_3400U              0x07 //3400

int8_t SX126X_config(void);
int8_t SX126xx_LoRaConfig(void);
int8_t SX126xx_FskConfig(void);
int8_t SX126X_initconfigstructure(void);
void SX126X_writereg(uint16_t reg, uint8_t val);
void SX126X_writeregblock(uint16_t startaddr, uint8_t len, uint8_t *buf);
uint8_t SX126X_readreg(uint16_t reg);
void SX126X_readregblock(uint16_t startaddr, uint8_t len, uint8_t *buf);
void SX126X_writebuffer(uint8_t offset, uint8_t len, uint8_t *buf);
void SX126X_readbuffer(uint8_t offset, uint8_t len, uint8_t *buf);
uint8_t sx126x_GetStatus(void);
void SX126X_SetSleep(bool rtctimeout,bool warmstart);
void SX126X_SetStby(bool xosc);
void SX126X_SetFs(void);
void SX126X_SetTx(uint32_t timeout);
void SX126X_SetRx(uint32_t timeout);
void SX126X_StopTimerOnPreamble(bool stop);
void SX126X_SetRxDutyCycle(uint32_t rxperiod, uint32_t sleepperiod);
void SX126X_SetCAD(void);
void SX126X_SetCW(void);
void SX126X_SetTxInfinitePreamble(void);
void SX126X_SetRegulatorMode(bool dcdc);
void SX126X_Calibrate(bool rc64k, bool rc13M, bool pll, bool adc, bool adcbulkn, bool adcbulkp, bool image);
void SX126X_CalibrateImage(uint8_t f1, uint8_t f2);
void SX126X_SetPaConfig(uint8_t dutycycle, uint8_t hpmax, bool lp);
void SX126X_SetRxTxFallbackMode(uint8_t mode);
void SX126X_SetDioIrqParams(uint16_t IRQmsk, uint16_t DIO1msk, uint16_t DIO2msk, uint16_t DIO3msk);
uint16_t SX126X_GetIrqStatus(void);
void SX126X_ClearIrqStatus(uint16_t msk);
void SX126X_SetDIO2AsRfSwitchCtrl(bool enable);
void SX126X_SetDIO3AsTCXOCtrl(uint8_t voltage, uint32_t timeout);
void SX126X_SetRfFrequency(uint32_t freqcode);
//void SX126X_SetPacketType(bool loramode);
void SX126X_SetLoRaMode(bool loramode);
//bool SX126X_GetPacketType(void);
bool SX126X_GetLoRaMode(void);
void SX126X_SetTxParams(void);
void SX126X_SetFskModParams(uint32_t br, uint8_t shaping, uint8_t rxbw, uint32_t fdev);
void SX126X_SetLoRaModParams(uint8_t sf, uint8_t bw, uint8_t cr);
void SX126X_SetFskPacketParams(uint16_t prelen, uint8_t pdetlen, uint8_t synclen, uint8_t addrfilt, bool varlen, uint8_t paylen, uint8_t crctype, bool white);
void SX126X_SetLoRaPacketParams(uint16_t prelen, bool implheader, uint8_t paylen, bool crcon, bool invertIQ);
void SX126X_SetFskAddr(uint8_t node_addr,uint8_t br_addr);
void SX126X_SetFskSyncWord(uint8_t *sync);
void SX126X_SetFskCrcWhitening(uint16_t crcinit, uint16_t crcpoly, uint16_t whiteinit);
void SX126X_SetCadParams(uint8_t cadSymbolNum, uint8_t cadDetPeak, uint8_t cadDetMin, bool cadrx, uint32_t cadTimeout);
void SX126X_SetBufferBaseAddress(uint8_t txaddr, uint8_t rxaddr);
void SX126X_SetLoRaSymbNumTimeout(uint8_t SymbNum);
uint8_t SX126X_GetStatus(void);
void SX126X_GetRxBufferStatus(uint8_t *status, uint8_t *PayloadLengthRx, uint8_t *RxStartBufferPointer);
void SX126X_GetFskPacketStatus(uint8_t *Status, uint8_t *RxStatus, uint8_t *RssiSync, uint8_t *RssiAvg);
void SX126X_GetLoRaPacketStatus(uint8_t *Status, uint8_t *RssiPkt, int16_t *SnrPkt, uint8_t *SignalRssiPkt);
uint8_t SX126X_GetRssiInst(void);
void SX126X_FskGetStats(uint8_t *Status, uint16_t *NbPktReceived, uint16_t *NbPktCrcError, uint16_t *NbPktLengthError);
void SX126X_LoRaGetStats(uint8_t *Status, uint16_t *NbPktReceived, uint16_t *NbPktCrcError, uint16_t *NbPktHeaderErr);
void SX126X_ResetStats(void);
void SX126X_GetDeviceErrors(uint8_t *Status, uint16_t *OpError);
void SX126X_ClearDeviceErrors(void);


void SX126X_SetChannel(void);
void SX126X_setopmode(uint8_t mode);
void SX126X_LNAboost(bool boost);

void SX126X_CalibrateIR(void);

extern uint8_t opmode;
extern uint8_t prevopmode;

extern uint16_t irqflags;
extern uint8_t rfstatus;


#endif