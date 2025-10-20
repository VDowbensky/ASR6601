#include "sx126x.h"
#include "bsp.h"
//#include "radio_proc.h"
//#include "flash.h"


void SX126X_SetSleep(uint8_t sx,bool rtctimeout,bool warmstart)
{
  //bit 0: RTC timeout
  //bit 2: cold/warm start
  uint8_t v = 0;
  if(rtctimeout) v |= 0x01;
  if(warmstart) v |= 0x04;
  SX126X_writeCmd(sx,SX126X_SET_SLEEP,&v,1);
  //wait 500 us
}

void SX126X_SetStandby(uint8_t sx,uint8_t mode)
{
  if(mode != 0) mode = 1;
  SX126X_writeCmd(sx,SX126X_SET_STANDBY,&mode,1);
}

void SX126X_SetFs(uint8_t sx)
{
  SX126X_writeCmd(sx,SX126X_SET_FS,NULL,0);
}

void SX126X_SetTx(uint8_t sx,uint32_t timeout)
{
  uint8_t buf[3];
  
  SX126X_rfsw_tx(sx);
  if(timeout > 0x00ffffff) timeout = 0x00ffffff;
  buf[0] = (uint8_t)((timeout & 0xff0000) >> 16);
  buf[1] = (uint8_t)((timeout & 0xff00) >> 8);
  buf[2] = (uint8_t)(timeout & 0xff);
  SX126X_writeCmd(sx,SX126X_SET_TX,buf,3);
}

void SX126X_SetRx(uint8_t sx,uint32_t timeout)
{
  uint8_t buf[3];
  
  SX126X_rfsw_rx(sx);
  if(timeout > 0x00ffffff) timeout = 0x00ffffff;
  buf[0] = (uint8_t)((timeout & 0xff0000) >> 16);
  buf[1] = (uint8_t)((timeout & 0xff00) >> 8);
  buf[2] = (uint8_t)(timeout & 0xff);
  SX126X_writeCmd(sx,SX126X_SET_RX,buf,3);
}

void SX126X_StopTimerOnPreamble(uint8_t sx,uint8_t mode)
{
  uint8_t v = 0;
  if(mode != 0) v = 1;
  SX126X_writeCmd(sx,SX126X_SET_STOPRXTIMERONPREAMBLE,&v,1);
}

void SX126X_SetRxDutyCycle(uint8_t sx,uint32_t rxperiod, uint32_t sleepperiod)
{
  uint8_t buf[6];
  
  if(rxperiod > 0x00ffffff) rxperiod = 0x00ffffff;
  if(sleepperiod > 0x00ffffff) sleepperiod = 0x00ffffff;
  buf[0] = (uint8_t)((rxperiod & 0xff0000) >> 16);
  buf[1] = (uint8_t)((rxperiod & 0xff00) >> 8);
  buf[2] = (uint8_t)(rxperiod & 0xff);
  buf[3] = (uint8_t)((sleepperiod & 0xff0000) >> 16);
  buf[4] = (uint8_t)((sleepperiod & 0xff00) >> 8);
  buf[5] = (uint8_t)(sleepperiod & 0xff);
  SX126X_writeCmd(sx,SX126X_SET_RXDUTYCYCLE,buf,6);
}

void SX126X_SetCAD(uint8_t sx)
{
  SX126X_rfsw_rx(sx);
  SX126X_writeCmd(sx,SX126X_SET_CAD,NULL,0);
}

void SX126X_SetCW(uint8_t sx)
{
  SX126X_rfsw_tx(sx);
  SX126X_writeCmd(sx,SX126X_SET_TXCONTINUOUSWAVE,NULL,0);
}

void SX126X_SetTxInfinitePreamble(uint8_t sx)
{
  SX126X_rfsw_tx(sx);
  SX126X_writeCmd(sx,SX126X_SET_TXCONTINUOUSPREAMBLE,NULL,0);
}

void SX126X_SetRegulatorMode(uint8_t sx,uint8_t mode)
{
  if(mode != 0) mode = 1;
  SX126X_writeCmd(sx,SX126X_SET_REGULATORMODE,&mode,1);
}

void SX126X_Calibrate(uint8_t sx,bool rc64k, bool rc13M, bool pll, bool adc, bool adcbulkn, bool adcbulkp, bool image)
{
  uint8_t v = 0;
  if(rc64k) v |= 1;
  if(rc13M) v |= 2;
  if(pll) v |= 4;
  if(adc) v |= 8;
  if(adcbulkn) v |= 0x10;
  if(adcbulkp) v |= 0x20;
  if(image) v |= 0x40;
  SX126X_writeCmd(sx,SX126X_CALIBRATE,&v,1);
}

void SX126X_CalibrateImage(uint8_t sx,uint8_t f1, uint8_t f2)
{
  uint8_t buf[2];
  
  buf[0] = f1;
  buf[1] = f2;
  SX126X_writeCmd(sx,SX126X_CALIBRATEIMAGE,buf,2);
  //lorac_wait_on_busy();
}

void SX126X_SetRxTxFallbackMode(uint8_t sx,uint8_t mode)
{
  //uint8_t v;
  
  //if(mode > 2) mode = 2;
  //v = 0x20 + (mode << 4);
  //switch(mode)
  //{
  //case 0:
  //default:
  //  v = 0x20; //STDBY_RC
  //  break;
  //case 1:
  //  v = 0x30;//0x20+0x01<<4
  //  break;
  //case 2:
  //  v = 0x40;//0x20+0x02<<4
  //  break;
  //}
  SX126X_writeCmd(sx,SX126X_SET_TXFALLBACKMODE,&mode,1);
}

void SX126X_SetDioIrqParams(uint8_t sx,uint16_t IRQmsk, uint16_t DIO1msk, uint16_t DIO2msk, uint16_t DIO3msk)
{
  uint8_t buf[8];
  
  buf[0] = (uint8_t)((IRQmsk & 0xff00) >> 8);
  buf[1] = (uint8_t)(IRQmsk & 0xff);
  buf[2] = (uint8_t)((DIO1msk & 0xff00) >> 8);
  buf[3] = (uint8_t)(DIO1msk & 0xff);
  buf[4] = (uint8_t)((DIO2msk & 0xff00) >> 8);
  buf[5] = (uint8_t)(DIO2msk & 0xff);
  buf[6] = (uint8_t)((DIO3msk & 0xff00) >> 8);
  buf[7] = (uint8_t)(DIO3msk & 0xff);
  SX126X_writeCmd(sx,SX126X_CFG_DIOIRQ,buf,8);
}

uint16_t SX126X_GetIrqStatus(uint8_t sx)
{
  uint8_t buf[3];
  
  SX126X_readCmd(sx,SX126X_GET_IRQSTATUS,buf,3);
  return ((uint16_t)buf[1] << 8) + buf[2];
}

void SX126X_ClearIrqStatus(uint8_t sx,uint16_t msk)
{
  uint8_t buf[2];
  buf[0] = (msk >> 8) & 0xff;
	buf[1] = msk & 0xff;
  SX126X_writeCmd(sx,SX126X_CLR_IRQSTATUS,buf,2);
}

void SX126X_SetDIO2AsRfSwitchCtrl(uint8_t sx,bool enable)
{
  uint8_t v = 0;
  if(enable) v= 1;
  SX126X_writeCmd(sx,SX126X_SET_RFSWITCHMODE,&v,1);
}

void SX126X_SetDIO3AsTCXOCtrl(uint8_t sx,uint8_t voltage, uint32_t timeout)
{
  uint8_t buf[4];
  
  buf[0] = voltage;
  if(timeout > 0xffffff) timeout = 0xffffff;
  buf[1] = (uint8_t)((timeout & 0xff0000) >> 16);
  buf[2] = (uint8_t)((timeout & 0xff00) >> 8);
  buf[3] = (uint8_t)(timeout & 0xff);
  SX126X_writeCmd(sx,SX126X_SET_TCXOMODE,buf,4);
}

void SX126X_SetRfFrequency(uint8_t sx,uint32_t freqcode)
{
  uint8_t buf[4];
  
  buf[0] = (uint8_t)((freqcode & 0xff000000) >> 24);
  buf[1] = (uint8_t)((freqcode & 0xff0000) >> 16);
  buf[2] = (uint8_t)((freqcode & 0xff00) >> 8);
  buf[3] = (uint8_t)(freqcode & 0xff);
  SX126X_writeCmd(sx,SX126X_SET_RFFREQUENCY,buf,4);
}

void SX126X_SetPacketType(uint8_t sx,uint8_t type)
{
  SX126X_writeCmd(sx,SX126X_SET_PACKETTYPE,&type,1);
}

uint8_t SX126X_GetPacketType(uint8_t sx)
{
  uint8_t buf[2];
  
  SX126X_readCmd(sx,SX126X_GET_PACKETTYPE,buf,2);
  return (buf[1] & 0x03);
}

void SX126X_SetTxParams(uint8_t sx,uint8_t power,uint8_t ramptime)
{
  uint8_t buf[2];
  
  buf[0] = power;
  buf[1] = ramptime;
  SX126X_writeCmd(sx,SX126X_SET_TXPARAMS,buf,2);
	//TxClampConfig 
  uint8_t reg = SX126X_readReg(sx,SX126X_REG_TXCLAMPCONFIG); //(0x08d8);
  reg |= 0x1e;
  SX126X_writeReg(sx,SX126X_REG_TXCLAMPCONFIG,reg);
}

void SX126X_SetPaConfig(uint8_t sx,uint8_t dutycycle, uint8_t hpmax, bool lp)
{
  uint8_t buf[4];
  
  buf[0] = dutycycle;
  if(hpmax > 7) hpmax = 7; //protect PA
  buf[1] = hpmax;
  if(lp) buf[2] = 1; //deviceSel
  else buf[2] = 0;
  buf[3] = 1; //paLut
  SX126X_writeCmd(sx,SX126X_SET_PACONFIG,buf,4);
}


void SX126X_SetFskModParams(uint8_t sx,uint32_t br, uint8_t shaping, uint8_t rxbw, uint32_t fdev)
{
  uint32_t br_val, dev_val;
  uint8_t buf[8];
  
  br_val = (FXO * 32) / br;
  if(br_val == 0) br_val = 1;
  if(br_val > 0xffffff) br_val = 0xffffff;
  //dev_val = (uint32_t)(fdev / ((double)FXO / 33554432));
  dev_val = ((fdev % 15625) * 16384 + 7812) / 15625 + (fdev / 15625) * 16384;
  if(dev_val > 0xffffff) dev_val = 0xffffff;
  buf[0] = (uint8_t)((br_val & 0xff0000) >> 16);
  buf[1] = (uint8_t)((br_val & 0xff00) >> 8);
  buf[2] = (uint8_t)(br_val & 0xff);
  buf[3] = shaping;
  buf[4] = rxbw;
  buf[5] = (uint8_t)((dev_val & 0xff0000) >> 16);
  buf[6] = (uint8_t)((dev_val & 0xff00) >> 8);
  buf[7] = (uint8_t)(dev_val & 0xff);
  SX126X_writeCmd(sx,SX126X_SET_MODULATIONPARAMS,buf,8);
  
  //uint8_t reg = SX126X_readReg(SX126X_REG_TXMOD); //(0x0889);
  //reg |= 0x4;
  //SX126X_writeReg(SX126X_REG_TXMOD,reg);
}

void SX126X_SetLoRaModParams(uint8_t sx,uint8_t sf, uint8_t bw, uint8_t cr)
{
  uint8_t buf[4];
  
  if(sf < 5) sf = 5;
  if(sf > 12) sf = 12;
  buf[0] = sf;
  buf[1] = bw;
  buf[2] = cr;
  if(sf > 10) buf[3] = 1;
  else buf[3] = 0;
  SX126X_writeCmd(sx,SX126X_SET_MODULATIONPARAMS,buf,4);
  uint8_t reg = SX126X_readReg(sx,SX126X_REG_TXMOD); //(0x0889);
  if(bw == SX126X_LORA_BW_500) reg &= 0xfb;
  else reg |= 4;
  SX126X_writeReg(sx,SX126X_REG_TXMOD,reg);//(0x0889,reg);
}

void SX126X_SetFskPacketParams(uint8_t sx,uint16_t prelen, uint8_t pdetlen, uint8_t synclen, uint8_t addrfilt, bool varlen, uint8_t paylen, uint8_t crctype, bool white)
{
  uint8_t buf[9];
  
  if(prelen == 0) prelen = 1;
  buf[0] = (uint8_t)((prelen & 0xff00) >> 8);
  buf[1] = (uint8_t)(prelen & 0xff);
  if((pdetlen > 0) && (pdetlen < 4)) pdetlen = 4;
  if(pdetlen > 7) pdetlen = 7;
  buf[2] = pdetlen;
  if(synclen > 0x40) synclen = 0x40;
  buf[3] = synclen;
  if(addrfilt > 2) addrfilt = 2;
  buf[4] = addrfilt;
  if(varlen) buf[5] = 1;
  else buf[5] = 0;
  buf[6] = paylen;
  buf[7] = crctype;
  if(white) buf[8] = 1;
  else buf[8] = 0;
  SX126X_writeCmd(sx,SX126X_SET_PACKETPARAMS,buf,9);
}

void SX126X_SetFskAddr(uint8_t sx,uint8_t node_addr,uint8_t br_addr)
{
  SX126X_writeReg(sx,SX126X_REG_NODEADDR,node_addr);
  SX126X_writeReg(sx,SX126X_REG_BROADCASTADDR,br_addr);
}

void SX126X_SetFskSyncWord(uint8_t sx,uint8_t *sync)
{
  for(uint8_t i = 0; i < 8; i++) SX126X_writeReg(sx,SX126X_REG_SYNC0 + i, sync[i]);
}

void SX126X_SetFskCrcWhitening(uint8_t sx,uint16_t crcinit, uint16_t crcpoly, uint16_t whiteinit)
{
  uint8_t reg;
  
  SX126X_writeReg(sx,SX126X_REG_CRCINIT_H, (crcinit & 0xff00) >> 8);
  SX126X_writeReg(sx,SX126X_REG_CRCINIT_L, crcinit & 0xff);
  SX126X_writeReg(sx,SX126X_REG_CRCPOLY_H, (crcpoly & 0xff00) >> 8);
  SX126X_writeReg(sx,SX126X_REG_CRCPOLY_L, crcpoly & 0xff);
  reg = SX126X_readReg(sx,SX126X_REG_WHITEINIT_H);
  reg &= ~0x01;
  reg |= ((whiteinit & 0xff00) >> 8) & 0x01;
  SX126X_writeReg(sx,SX126X_REG_WHITEINIT_H, reg);
  SX126X_writeReg(sx,SX126X_REG_WHITEINIT_L, whiteinit & 0xff);
}

void SX126X_SetLoRaPacketParams(uint8_t sx,uint16_t prelen, bool implheader, uint8_t paylen, bool crcon, bool invertIQ)
{
  uint8_t buf[6];
  
  if(prelen == 0) prelen = 1;
  buf[0] = (uint8_t)((prelen & 0xff00) >> 8);
  buf[1] = (uint8_t)(prelen & 0xff);
  if(implheader) buf[2] = 1;
  else buf[2] = 0;
  buf[3] = paylen;
  if(crcon) buf[4] = 1;
  else buf[4] = 0;
  if(invertIQ) buf[5] = 1;
  else buf[5] = 0;
  SX126X_writeCmd(sx,SX126X_SET_PACKETPARAMS,buf,6);
  uint8_t reg = SX126X_readReg(sx,SX126X_REG_IQPOL); //(0x736);
  if(invertIQ) reg &= 0xfb;
  else reg |= 4;
  SX126X_writeReg(sx,SX126X_REG_IQPOL,reg);
}

void SX126X_SetCadParams(uint8_t sx,uint8_t cadSymbolNum, uint8_t cadDetPeak, uint8_t cadDetMin, bool cadrx, uint32_t cadTimeout)
{
  uint8_t buf[7];
  
  if(cadSymbolNum > 4) cadSymbolNum = 4;
  buf[0] = cadSymbolNum;
  buf[1] = cadDetPeak;
  buf[2] = cadDetMin;
  if(cadrx) buf[3] = 1;
  else buf[3] = 0;
  if(cadTimeout > 0xffffff) cadTimeout = 0xffffff;
  buf[4] = (uint8_t)((cadTimeout & 0xff0000) >> 16);
  buf[5] = (uint8_t)((cadTimeout & 0xff00) >> 8);
  buf[6] = (uint8_t)(cadTimeout & 0xff);
  SX126X_writeCmd(sx,SX126X_SET_CADPARAMS,buf,7);
}

void SX126X_SetBufferBaseAddress(uint8_t sx,uint8_t txaddr, uint8_t rxaddr)
{
  uint8_t buf[2];
  
  buf[0] = txaddr;
  buf[1] = rxaddr;
  SX126X_writeCmd(sx,SX126X_SET_BUFFERBASEADDRESS,buf,2);
}

void SX126X_SetLoRaSymbNumTimeout(uint8_t sx,uint8_t SymbNum)
{
  SX126X_writeCmd(sx,SX126X_SET_LORASYMBTIMEOUT,&SymbNum,1);
  if(SymbNum > 63) SX126X_writeReg(sx,0x706,(SymbNum & 0xf8) + 1);
}

uint8_t SX126X_GetStatus(uint8_t sx)
{
  //lorac_TXbuffer[0] = SX126X_GET_STATUS;
  //lorac_TXbuffer[1] = 0;
  //lorac_transferblock(2);
  //return lorac_RXbuffer[1];
  uint8_t v;
  
  SX126X_readCmd(sx,SX126X_GET_STATUS,&v,1);
  return v;
}

void SX126X_GetRxBufferStatus(uint8_t sx,uint8_t *status, uint8_t *PayloadLengthRx, uint8_t *RxStartBufferPointer)
{
  uint8_t buf[3];
  
  SX126X_readCmd(sx,SX126X_GET_RXBUFFERSTATUS,buf,3);
  *status = buf[0];
  *PayloadLengthRx = buf[1];
  *RxStartBufferPointer = buf[2];
}

void SX126X_GetFskPacketStatus(uint8_t sx,uint8_t *Status, uint8_t *RxStatus, uint8_t *RssiSync, uint8_t *RssiAvg)
{
  uint8_t buf[4];
  
  SX126X_readCmd(sx,SX126X_GET_PACKETSTATUS,buf,4);
  *Status = buf[0];
  *RxStatus = buf[1];
  *RssiSync = buf[2];
  *RssiAvg = buf[3];
}

void SX126X_GetLoRaPacketStatus(uint8_t sx,uint8_t *Status, uint8_t *RssiPkt, int16_t *SnrPkt, uint8_t *SignalRssiPkt)
{
  uint8_t buf[4];
  
  SX126X_readCmd(sx,SX126X_GET_PACKETSTATUS,buf,4);
  *Status = buf[0];
  *RssiPkt = buf[1];
  *SnrPkt = buf[2];
  *SignalRssiPkt = buf[3];
}

uint8_t SX126X_GetRssiInst(uint8_t sx)
{
  uint8_t buf[2];
  
  SX126X_readCmd(sx,SX126X_GET_RSSIINST,buf,2);
  return buf[1];
}

void SX126X_FskGetStats(uint8_t sx,uint8_t *Status, uint16_t *NbPktReceived, uint16_t *NbPktCrcError, uint16_t *NbPktLengthError)
{
  uint8_t buf[7];
  
  SX126X_readCmd(sx,SX126X_GET_STATS,buf,7);
  *Status = buf[0];
  *NbPktReceived = ((uint16_t)buf[1] << 8) + buf[2];
  *NbPktCrcError = ((uint16_t)buf[3] << 8) + buf[4];
  *NbPktLengthError = ((uint16_t)buf[5] << 8) + buf[6];
}

void SX126X_LoRaGetStats(uint8_t sx,uint8_t *Status, uint16_t *NbPktReceived, uint16_t *NbPktCrcError, uint16_t *NbPktHeaderErr)
{
  uint8_t buf[7];
  
  SX126X_readCmd(sx,SX126X_GET_STATS,buf,7);
  *Status = buf[0];
  *NbPktReceived = ((uint16_t)buf[1] << 8) + buf[2];
  *NbPktCrcError = ((uint16_t)buf[3] << 8) + buf[4];
  *NbPktHeaderErr = ((uint16_t)buf[5] << 8) + buf[6];
}

void SX126X_ResetStats(uint8_t sx)
{
  uint8_t buf[6] = {0,0,0,0,0,0};
  
  SX126X_writeCmd(sx,SX126X_RESET_STATS,buf,6);
}

void SX126X_GetDeviceErrors(uint8_t sx,uint8_t *Status, uint16_t *OpError)
{
  uint8_t buf[3];
  
  SX126X_readCmd(sx,SX126X_GET_ERROR,buf,3);
  *Status = buf[0];
  *OpError = ((uint16_t)buf[1] << 8) + buf[2];
}

void SX126X_ClearDeviceErrors(uint8_t sx)
{
  uint8_t v[2];
	v[0] = 0;
	v[1] = 0;
  
  SX126X_writeCmd(sx,SX126X_CLR_ERROR,v,2);
}

void SX126X_LNAboost(uint8_t sx,bool boost)
{
  if(boost) SX126X_writeReg(sx,0x08ac,0x96);
  else SX126X_writeReg(sx,0x08ac,0x94);
}
