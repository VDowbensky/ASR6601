#ifndef _LR112X_H_
#define _LR112X_H_

#include "lr112x_interface.h"

void LR112X_SetStandby(uint8_t mode);
void LR112X_CalibImage(uint8_t f1,uint8_t f2);
void LR112X_SetSleep(uint8_t config,uint32_t time);
void LR112X_Reboot(bool stayinbootloader);
void LR112X_SetFs(void);
void LR112X_GetVersion(uint8_t *hw,uint8_t *use_case,uint8_t *fw_major,uint8_t *fw_minor);
void LR112X_EraseFlash(void);
void LR112X_WriteFlashEncrypted(uint32_t offset,uint8_t *src,uint16_t len);
void LR112X_EraseInfoPage(void);
void LR112X_WriteInfoPage(uint16_t wordaddr,uint8_t *src,uint16_t len);
void LR112X_ReadInfoPage(uint16_t wordaddr,uint8_t *dst,uint16_t len);
uint32_t LR112X_GetStatus(void);
uint16_t LR112X_GetErrors(void);
void LR112X_ClearErrors(void);
void LR112X_ClearRxBuffer(void);
uint32_t LR112X_GetRandomNumber(void);
void LR112X_EnableSpiCrc(bool enable,uint8_t crc);
void LR112X_SetDioIrqParams(uint32_t irq1,uint32_t irq2);
void LR112X_ClearIrq(uint32_t mask);
void LR112X_SetDioAsRfSwitch(uint8_t enable,uint8_t stby,uint8_t rx,uint8_t tx,uint8_t txhp,uint8_t txhf);
void LR112X_DriveDiosInSleepMode(bool enable);
float LR112X_GetTemp(void);
void LR112X_SetRegMode(bool mode);
float LR112X_GetVbat(void);
void LR112X_ConfigLfClock(uint8_t clk,bool wait);
void LR112X_SetTcxoMode(uint8_t voltage,uint32_t delay);
void LR112X_SetRfFrequency(uint32_t hz);
void LR112X_SetRx(uint32_t timeout);
void LR112X_SetTx(uint32_t timeout);
void LR112X_AutoTxRx(uint32_t delay,uint8_t mode,uint32_t timeout);
void LR112X_SetRxTxFallbackMode(uint8_t mode);
void LR112X_SetRxDutyCycle(uint32_t rxperiod,uint32_t sleepperiod,uint8_t mode);
void LR112X_StopTimeoutOnPreamble(bool enable);
float LR112X_GetRssiInst(void);
void LR112X_GetStats(uint16_t *nbpkt,uint16_t *nbcrcerr,uint16_t *data1,uint16_t *data2);
void LR112X_ResetStats(void);
void LR112X_GetRxBufferStatus(uint8_t *rxpaylen,uint8_t *rxstartbufferpointer);
void LR112X_SetRxBoosted(bool enable);
void LR112X_SetLoRaSyncWord(uint8_t sync);
void LR112X_GetLoRaRxHeaderInfos(uint8_t *infos);
void LR112X_SetRssiCalibration(uint8_t *tunes,uint16_t gainoffset);
void LR112X_SetPacketType(uint8_t type);
uint8_t LR112X_ GetPacketType(void);
void LR112X_SetLoRaModulationParams(uint8_t sf,uint8_t bw,uint8_t cr,bool ldropt);
void LR112X_SetLoRaPacketParams(uint16_t prelen,bool implheader,uint8_t paylen,bool crcen,bool invertiq);
void LR112X_SetCad(void);
void LR112X_SetCadParams(uint8_t symnum,uint8_t detpeak,uint8_t detmin,uint8_t exitmode,uint32_t timeout);
void LR112X_SetLoRaSynchTimeout(uint8_t symnum);
void LR112X_SetLoRaPublicNetwork(bool enable);
void LR112X_GetLoRaPacketStatus(float *rssipkt,float *snrpkt,float *signalrssipkt);
void LR112X_SetFskModulationParams(uint32_t br,uint8_t shape,uint8_t bw,uint32_t fdevHz);
void LR112X_SetFskPacketParams(uint16_t prelen,uint8_t predet,uint8_t synclen,uint8_t addrcomp,uint8_t pkttype,uint8_t paylen,uint8_t crctype,uint8_t dcfree);
void LR112X_SetGfskSyncWord(uint64_t syncword);
void LR112X_SetPacketAdrs(uint8_t ndaddr,uint8_t braddr);
void LR112X_SetGfskCrcParams(uint32_t init,uint32_t poly);
void LR112X_SetGfskWhitParams(uint16_t seed);
void LR112X_GetFskPacketStatus(float *rssisync,float *rssiavg,uint8_t *rxlen,uint8_t *status);
void LR112X_SetPaConfig(uint8_t pasel,uint8_t ps,uint8_t dutycycle,uint8_t pahpsel);
void LR112X_SetTxParams(int8_t power,uint8_t ramptime);
uint64_t LR112X_GetChipEui(void);
uint64_t GetSemtechJoinEui(void);
void LR112X_SetTxCw(void);
void LR112X_SetTxInfinitePreamble(void);

#endif
