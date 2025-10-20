#ifndef _SX126XX_H_
#define _SX126XX_H_

#include "bsp.h"
#include "sx126x_interface.h"


uint8_t sx126x_GetStatus(uint8_t sx);
void SX126X_SetSleep(uint8_t sx,bool rtctimeout,bool warmstart);
void SX126X_SetStandby(uint8_t sx,uint8_t mode);
void SX126X_SetFs(uint8_t sx);
void SX126X_SetTx(uint8_t sx,uint32_t timeout);
void SX126X_SetRx(uint8_t sx,uint32_t timeout);
void SX126X_StopTimerOnPreamble(uint8_t sx,uint8_t mode);
void SX126X_SetRxDutyCycle(uint8_t sx,uint32_t rxperiod, uint32_t sleepperiod);
void SX126X_SetCAD(uint8_t sx);
void SX126X_SetCW(uint8_t sx);
void SX126X_SetTxInfinitePreamble(uint8_t sx);
void SX126X_SetRegulatorMode(uint8_t sx,uint8_t mode);
void SX126X_Calibrate(uint8_t sx,bool rc64k, bool rc13M, bool pll, bool adc, bool adcbulkn, bool adcbulkp, bool image);
void SX126X_CalibrateImage(uint8_t sx,uint8_t f1, uint8_t f2);
void SX126X_SetPaConfig(uint8_t sx,uint8_t dutycycle, uint8_t hpmax, bool lp);
void SX126X_SetRxTxFallbackMode(uint8_t sx,uint8_t mode);
void SX126X_SetDioIrqParams(uint8_t sx,uint16_t IRQmsk, uint16_t DIO1msk, uint16_t DIO2msk, uint16_t DIO3msk);
uint16_t SX126X_GetIrqStatus(uint8_t sx);
void SX126X_ClearIrqStatus(uint8_t sx,uint16_t msk);
void SX126X_SetDIO2AsRfSwitchCtrl(uint8_t sx,bool enable);
void SX126X_SetDIO3AsTCXOCtrl(uint8_t sx,uint8_t voltage, uint32_t timeout);
void SX126X_SetRfFrequency(uint8_t sx,uint32_t freqcode);
void SX126X_SetPacketType(uint8_t sx,uint8_t type);
uint8_t SX126X_GetPacketType(uint8_t sx);
void SX126X_SetTxParams(uint8_t sx,uint8_t power,uint8_t ramptime);
void SX126X_SetFskModParams(uint8_t sx,uint32_t br, uint8_t shaping, uint8_t rxbw, uint32_t fdev);
void SX126X_SetLoRaModParams(uint8_t sx,uint8_t sf, uint8_t bw, uint8_t cr);
void SX126X_SetFskPacketParams(uint8_t sx,uint16_t prelen, uint8_t pdetlen, uint8_t synclen, uint8_t addrfilt, bool varlen, uint8_t paylen, uint8_t crctype, bool white);
void SX126X_SetLoRaPacketParams(uint8_t sx,uint16_t prelen, bool implheader, uint8_t paylen, bool crcon, bool invertIQ);
void SX126X_SetFskAddr(uint8_t sx,uint8_t node_addr,uint8_t br_addr);
void SX126X_SetFskSyncWord(uint8_t sx,uint8_t *sync);
void SX126X_SetFskCrcWhitening(uint8_t sx,uint16_t crcinit, uint16_t crcpoly, uint16_t whiteinit);
void SX126X_SetCadParams(uint8_t sx,uint8_t cadSymbolNum, uint8_t cadDetPeak, uint8_t cadDetMin, bool cadrx, uint32_t cadTimeout);
void SX126X_SetBufferBaseAddress(uint8_t sx,uint8_t txaddr, uint8_t rxaddr);
void SX126X_SetLoRaSymbNumTimeout(uint8_t sx,uint8_t SymbNum);
uint8_t SX126X_GetStatus(uint8_t sx);
void SX126X_GetRxBufferStatus(uint8_t sx,uint8_t *status, uint8_t *PayloadLengthRx, uint8_t *RxStartBufferPointer);
void SX126X_GetFskPacketStatus(uint8_t sx,uint8_t *Status, uint8_t *RxStatus, uint8_t *RssiSync, uint8_t *RssiAvg);
void SX126X_GetLoRaPacketStatus(uint8_t sx,uint8_t *Status, uint8_t *RssiPkt, int16_t *SnrPkt, uint8_t *SignalRssiPkt);
uint8_t SX126X_GetRssiInst(uint8_t sx);
void SX126X_FskGetStats(uint8_t sx,uint8_t *Status, uint16_t *NbPktReceived, uint16_t *NbPktCrcError, uint16_t *NbPktLengthError);
void SX126X_LoRaGetStats(uint8_t sx,uint8_t *Status, uint16_t *NbPktReceived, uint16_t *NbPktCrcError, uint16_t *NbPktHeaderErr);
void SX126X_ResetStats(uint8_t sx);
void SX126X_GetDeviceErrors(uint8_t sx,uint8_t *Status, uint16_t *OpError);
void SX126X_ClearDeviceErrors(uint8_t sx);
void SX126X_LNAboost(uint8_t sx,bool boost);


#endif
