#ifndef _SX126XX_H_
#define _SX126XX_H_

//#include <stdint.h>
//#include <stdbool.h>
#include "bsp.h"
#include "lorac.h"
#include "sx126x_interface.h"
//#include "eeprom.h"
#define    FXO 32000000UL

#define SX126X_OPMODE_SLEEP            0
#define SX126X_OPMODE_STBYRC           1
#define SX126X_OPMODE_STBYXOSC         2
#define SX126X_OPMODE_FS               3
#define SX126X_OPMODE_TX               4
#define SX126X_OPMODE_RX               5
#define SX126X_OPMODE_TXSTREAMCW       6
#define SX126X_OPMODE_TXSTREAMPRE      7

#define SX126X_MODEM_FSK		0
#define SX126X_MODEM_LORA		1
#define SX126X_MODEM_FHSS		3

//IRQ mask
#define    SX126X_TXDONE_IRQMSK                          0x01 //All
#define    SX126X_RXDONE_IRQMSK                          0x02 //All
#define    SX126X_RPEDET_IRQMSK                          0x04 //All
#define    SX126X_SYNCDET_IRQMSK                         0x08 //FSK
#define    SX126X_HEADERDET_IRQMSK                       0x10 //LoRa
#define    SX126X_HEADERERR_IRQMSK                       0x20 //LoRa
#define    SX126X_CRCERR_IRQMSK                          0x40 //All
#define    SX126X_CADDONE_IRQMSK                         0x80 //LoRa
#define    SX126X_CADDET_IRQMSK                          0x100 //LoRa
#define    SX126X_TIMEOUT_IRQMSK                         0x200 //All
#define    SX126X_LRFHSSHOP_IRQMSK                       0x4000 //LR-FHSS
#define    SX126X_ALL_IRQMSK                             0x7fff

//Radio events

//Commands
#define    SX126X_GET_STATUS                         0xC0//GetStatus
#define    SX126X_WRITE_REGISTER                     0x0D
#define    SX126X_READ_REGISTER                      0x1D
#define    SX126X_WRITE_BUFFER                       0x0E
#define    SX126X_READ_BUFFER                        0x1E
#define    SX126X_SET_SLEEP                          0x84//SetSleep
#define    SX126X_SET_STANDBY                        0x80//SetStandby
#define    SX126X_SET_FS                             0xC1//SetFs
#define    SX126X_SET_TX                             0x83//SetTx
#define    SX126X_SET_RX                             0x82//SetRx
#define    SX126X_SET_RXDUTYCYCLE                    0x94//SetRxDutyCycle
#define    SX126X_SET_CAD                            0xC5//SetCad
#define    SX126X_SET_TXCONTINUOUSWAVE               0xD1//SetTxContinuousWave
#define    SX126X_SET_TXCONTINUOUSPREAMBLE           0xD2//SetTxInfinitePreamble
#define    SX126X_UPDATE_PRAM                        0xD9//Apply PRAM patch

#define    SX126X_SET_PACKETTYPE                     0x8A//SetPacketType
#define    SX126X_GET_PACKETTYPE                     0x11//GetPacketType
#define    SX126X_SET_RFFREQUENCY                    0x86//SetRfFrequency
#define    SX126X_SET_TXPARAMS                       0x8E//SetTxParams
#define    SX126X_SET_PACONFIG                       0x95//SetPaConfig
#define    SX126X_SET_CADPARAMS                      0x88
#define    SX126X_SET_BUFFERBASEADDRESS              0x8F
#define    SX126X_SET_MODULATIONPARAMS               0x8B//SetModulationParams
#define    SX126X_SET_PACKETPARAMS                   0x8C
#define    SX126X_GET_RXBUFFERSTATUS                 0x13//GetRxBufferStatus
#define    SX126X_GET_PACKETSTATUS                   0x14//GetPacketStatus
#define    SX126X_GET_RSSIINST                       0x15//GetRssiInst
#define    SX126X_GET_STATS                          0x10//GetStats
#define    SX126X_RESET_STATS                        0x00//ResetStats
#define    SX126X_CFG_DIOIRQ                         0x08//SetDioIrqParams

#define    SX126X_GET_IRQSTATUS                      0x12//GetIrqStatus
#define    SX126X_CLR_IRQSTATUS                      0x02//ClearIrqStatus
#define    SX126X_CALIBRATE                          0x89//Calibrate
#define    SX126X_CALIBRATEIMAGE                     0x98//CalibrateImage
#define    SX126X_SET_REGULATORMODE                  0x96//SetRegulatorMode
#define    SX126X_GET_ERROR                          0x17//GetDeviceErrors
#define    SX126X_CLR_ERROR                          0x07//ClearDeviceErrors
#define    SX126X_SET_TCXOMODE                       0x97//SetDIO3AsTcxoCtrl
#define    SX126X_SET_TXFALLBACKMODE                 0x93//SetRxTxFallbackMode
#define    SX126X_SET_RFSWITCHMODE                   0x9D//SetDIO2AsRfSwitchCtrl
#define    SX126X_SET_STOPRXTIMERONPREAMBLE          0x9F//StopTimerOnPreamble
#define    SX126X_SET_LORASYMBTIMEOUT                0xA0

//Registers
#define SX126X_REG_HoppingEnable               0x0385
#define SX126X_REG_PacketLength                0x0386
#define SX126X_REG_NbHoppingBlocks             0x0387

#define SX126X_REG_NbSymbols0_H                0x0388
#define SX126X_REG_NbSymbols0_L                0x0389
#define SX126X_REG_Freq0_H                     0x038a
#define SX126X_REG_Freq0_MH                    0x038b
#define SX126X_REG_Freq0_ML                    0x038c
#define SX126X_REG_Freq0_L                     0x038d

#define SX126X_REG_NbSymbols1_H                0x038e
#define SX126X_REG_NbSymbols1_L                0x038f
#define SX126X_REG_Freq1_H                     0x0390
#define SX126X_REG_Freq1_MH                    0x0391
#define SX126X_REG_Freq1_ML                    0x0392
#define SX126X_REG_Freq1_L                     0x0393

#define SX126X_REG_NbSymbols2_H                0x0394
#define SX126X_REG_NbSymbols2_L                0x0395
#define SX126X_REG_Freq2_H                     0x0396
#define SX126X_REG_Freq2_MH                    0x0397
#define SX126X_REG_Freq2_ML                    0x0398
#define SX126X_REG_Freq2_L                     0x0399

#define SX126X_REG_NbSymbols3_H                0x039a
#define SX126X_REG_NbSymbols3_L                0x039b
#define SX126X_REG_Freq3_H                     0x039c
#define SX126X_REG_Freq3_MH                    0x039d
#define SX126X_REG_Freq3_ML                    0x039e
#define SX126X_REG_Freq3_L                     0x039f

#define SX126X_REG_NbSymbols4_H                0x03a0
#define SX126X_REG_NbSymbols4_L                0x03a1
#define SX126X_REG_Freq4_H                     0x03a2
#define SX126X_REG_Freq4_MH                    0x03a3
#define SX126X_REG_Freq4_ML                    0x03a4
#define SX126X_REG_Freq4_L                     0x03a5

#define SX126X_REG_NbSymbols5_H                0x03a6
#define SX126X_REG_NbSymbols5_L                0x03a7
#define SX126X_REG_Freq5_H                     0x03a8
#define SX126X_REG_Freq5_MH                    0x03a9
#define SX126X_REG_Freq5_ML                    0x03aa
#define SX126X_REG_Freq5_L                     0x03ab

#define SX126X_REG_NbSymbols6_H                0x03ac
#define SX126X_REG_NbSymbols6_L                0x03ad
#define SX126X_REG_Freq6_H                     0x03ae
#define SX126X_REG_Freq6_MH                    0x03af
#define SX126X_REG_Freq6_ML                    0x03b0
#define SX126X_REG_Freq6_L                     0x03b1

#define SX126X_REG_NbSymbols7_H                0x03b2
#define SX126X_REG_NbSymbols7_L                0x03b3
#define SX126X_REG_Freq7_H                     0x03b4
#define SX126X_REG_Freq7_MH                    0x03b5
#define SX126X_REG_Freq7_ML                    0x03b6
#define SX126X_REG_Freq7_L                     0x03b7

#define SX126X_REG_NbSymbols8_H                0x03b8
#define SX126X_REG_NbSymbols8_L                0x03b9
#define SX126X_REG_Freq8_H                     0x03ba
#define SX126X_REG_Freq8_MH                    0x03bb
#define SX126X_REG_Freq8_ML                    0x03bc
#define SX126X_REG_Freq8_L                     0x03bd

#define SX126X_REG_NbSymbols9_H                0x03be
#define SX126X_REG_NbSymbols9_L                0x03bf
#define SX126X_REG_Freq9_H                     0x03c0
#define SX126X_REG_Freq9_MH                    0x03c1
#define SX126X_REG_Freq9_ML                    0x03c2
#define SX126X_REG_Freq9_L                     0x03c3

#define SX126X_REG_NbSymbols10_H               0x03c4
#define SX126X_REG_NbSymbols20_L               0x03c5
#define SX126X_REG_Freq10_H                    0x03c6
#define SX126X_REG_Freq10_MH                   0x03c7
#define SX126X_REG_Freq10_ML                   0x03c8
#define SX126X_REG_Freq10_L                    0x03c9

#define SX126X_REG_NbSymbols11_H               0x03ca
#define SX126X_REG_NbSymbols11_L               0x03cb
#define SX126X_REG_Freq11_H                    0x03cc
#define SX126X_REG_Freq11_MH                   0x03cd
#define SX126X_REG_Freq11_ML                   0x03ce
#define SX126X_REG_Freq11_L                    0x03cf

#define SX126X_REG_NbSymbols12_H               0x03d0
#define SX126X_REG_NbSymbols12_L               0x03d1
#define SX126X_REG_Freq12_H                    0x03d2
#define SX126X_REG_Freq12_MH                   0x03d3
#define SX126X_REG_Freq12_ML                   0x03d4
#define SX126X_REG_Freq12_L                    0x03d5

#define SX126X_REG_NbSymbols13_H               0x03d6
#define SX126X_REG_NbSymbols13_L               0x03d7
#define SX126X_REG_Freq13_H                    0x03d8
#define SX126X_REG_Freq13_MH                   0x03d9
#define SX126X_REG_Freq13_ML                   0x03da
#define SX126X_REG_Freq13_L                    0x03db

#define SX126X_REG_NbSymbols14_H               0x03de
#define SX126X_REG_NbSymbols14_L               0x03df
#define SX126X_REG_Freq14_H                    0x03e0
#define SX126X_REG_Freq14_MH                   0x03e1
#define SX126X_REG_Freq14_ML                   0x03e2
#define SX126X_REG_Freq14_L                    0x03e3

#define SX126X_REG_NbSymbols15_H               0x03e4
#define SX126X_REG_NbSymbols15_L               0x03e5
#define SX126X_REG_Freq15_H                    0x03e6
#define SX126X_REG_Freq15_MH                   0x03e7
#define SX126X_REG_Freq15_ML                   0x03e8
#define SX126X_REG_Freq15_L                    0x03e9

#define SX126X_REG_DIOOUT_EN                   0x0580
#define SX126X_REG_DIOIN_EN                    0x0583
#define SX126X_REG_DIOPU_CTRL                  0x0584
#define SX126X_REG_DIOPD_CTRL                  0x0585


#define SX126X_REG_BITSYNC                     0x06ac
#define SX126X_REG_WHITEINIT_H                 0x06b8
#define SX126X_REG_WHITEINIT_L                 0x06b9

#define SX126X_REG_CRCINIT_H                   0x06bc
#define SX126X_REG_CRCINIT_L                   0x06bd
#define SX126X_REG_CRCPOLY_H                   0x06be
#define SX126X_REG_CRCPOLY_L                   0x06bf

#define SX126X_REG_SYNC0                       0x06c0
#define SX126X_REG_SYNC1                       0x06c1
#define SX126X_REG_SYNC2                       0x06c2
#define SX126X_REG_SYNC3                       0x06c3
#define SX126X_REG_SYNC4                       0x06c4
#define SX126X_REG_SYNC5                       0x06c5
#define SX126X_REG_SYNC6                       0x06c6
#define SX126X_REG_SYNC7                       0x06c7

#define SX126X_REG_NODEADDR                    0x06cd
#define SX126X_REG_BROADCASTADDR               0x06ce

#define SX126X_REG_GAFC                        0x06d1

#define SX126X_REG_LPLDLEN                     0x0702
#define SX126X_REG_LSYNCTIMEOUT                0x0706

#define SX126X_REG_IQPOL                       0x0736
#define SX126X_REG_LRSYNC_H                    0x0740
#define SX126X_REG_LRSYNC_L                    0x0741

#define SX126X_REG_RND0                        0x0819
#define SX126X_REG_RND1                        0x081a
#define SX126X_REG_RND2                        0x081b
#define SX126X_REG_RND3                        0x081c

#define SX126X_REG_TXMOD                       0x0889
#define SX126X_REG_PLLSTEP                     0x0889
#define SX126X_REG_RXGAIN                      0x08ac

#define SX126X_REG_TXCLAMPCONFIG               0x08d8
#define SX126X_REG_OCPCONFIG                   0x08e7
#define SX126X_REG_RTCCTRL                     0x0902
#define SX126X_REG_XTATRIM                     0x0911
#define SX126X_REG_XTBTRIM                     0x0912
#define SX126X_REG_DIO3VOUTCTRL                0x0920
#define SX126X_REG_EVENTMASK                   0x0944

#define SX126X_LORA_BW_7p8             0
#define SX126X_LORA_BW_10p4            0x08
#define SX126X_LORA_BW_15p6            0x01
#define SX126X_LORA_BW_20p8            0x09
#define SX126X_LORA_BW_31p3            0x02
#define SX126X_LORA_BW_41p7            0x0a
#define SX126X_LORA_BW_62p5            0x03
#define SX126X_LORA_BW_125             0x04
#define SX126X_LORA_BW_250             0x05
#define SX126X_LORA_BW_500             0x06

#define SX126X_FSK_BW_4p8              0x1f
#define SX126X_FSK_BW_5p8              0x17
#define SX126X_FSK_BW_7p3              0x0f
#define SX126X_FSK_BW_9p7              0x1e
#define SX126X_FSK_BW_11p7             0x16
#define SX126X_FSK_BW_14p6             0x0e
#define SX126X_FSK_BW_19p5             0x1d
#define SX126X_FSK_BW_23p4             0x15
#define SX126X_FSK_BW_29p3             0x0d
#define SX126X_FSK_BW_39               0x1c
#define SX126X_FSK_BW_46p9             0x14
#define SX126X_FSK_BW_58p6             0x0c
#define SX126X_FSK_BW_78p2             0x1b
#define SX126X_FSK_BW_93p8             0x13
#define SX126X_FSK_BW_117p3            0x0b
#define SX126X_FSK_BW_156p2            0x1a
#define SX126X_FSK_BW_187p2            0x12
#define SX126X_FSK_BW_234p3            0x0a
#define SX126X_FSK_BW_312              0x19
#define SX126X_FSK_BW_373p6            0x11
#define SX126X_FSK_BW_467              0x09
#define SX126X_FSK_BW_INVALID          0x00

#define SX126X_FSK_BT0                 0
#define SX126X_FSK_BT0p3               0x08
#define SX126X_FSK_BT0p5               0x09
#define SX126X_FSK_BT0p7               0x0a
#define SX126X_FSK_BT1                 0x0b

#define SX126X_FSK_CRC_OFF             1
#define SX126X_FSK_CRC_1B              0
#define SX126X_FSK_CRC_2B              2
#define SX126X_FSK_CRC_1B_INV          4
#define SX126X_FSK_CRC_2B_INV          6

#define SX126X_LORA_CR_4_5             1
#define SX126X_LORA_CR_4_6             2
#define SX126X_LORA_CR_4_7             3
#define SX126X_LORA_CR_4_8             4

#define SX126X_RAMP_10U                0x00 //10
#define SX126X_RAMP_20U                0x01 //20
#define SX126X_RAMP_40U                0x02 //40
#define SX126X_RAMP_80U                0x03 //80
#define SX126X_RAMP_200U               0x04 //200
#define SX126X_RAMP_800U               0x05 //800
#define SX126X_RAMP_1700U              0x06 //1700
#define SX126X_RAMP_3400U              0x07 //3400

uint8_t sx126x_GetStatus(void);
void SX126X_SetSleep(bool rtctimeout,bool warmstart);
void SX126X_SetStandby(uint8_t mode);
void SX126X_SetFs(void);
void SX126X_SetTx(uint32_t timeout);
void SX126X_SetRx(uint32_t timeout);
void SX126X_StopTimerOnPreamble(uint8_t mode);
void SX126X_SetRxDutyCycle(uint32_t rxperiod, uint32_t sleepperiod);
void SX126X_SetCAD(void);
void SX126X_SetCW(void);
void SX126X_SetTxInfinitePreamble(void);
void SX126X_SetRegulatorMode(uint8_t mode);
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
void SX126X_SetPacketType(uint8_t type);
uint8_t SX126X_GetPacketType(void);
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


#endif