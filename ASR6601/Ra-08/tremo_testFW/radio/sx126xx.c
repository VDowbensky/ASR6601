#include "sx126xx.h"
#include "bsp.h"
#include "radio_proc.h"
#include "flash.h"

uint8_t opmode = 0;
uint8_t prevopmode = 0;
uint16_t irqflags = 0;
uint8_t rfstatus = 0;

/*
#define OPMODE_SLEEP            0
#define OPMODE_STBYRC           1
#define OPMODE_STBYXOSC         2
#define OPMODE_FS               3
#define OPMODE_TX               4
#define OPMODE_RX               5
#define OPMODE_TXSTREAMCW       6
#define OPMODE_TXSTREAMPRE      7
*/

int8_t SX126X_config(void)
{
	lorac_reset();
  SX126X_Wakeup();
  delay_ms(10);
  SX126X_setopmode(OPMODE_STBYRC);
  SX126X_SetRegulatorMode(true);
  if(radioConfig.modem == MODEM_LORA) SX126xx_LoRaConfig();
  else if (radioConfig.modem == MODEM_FSK) SX126xx_FskConfig();
  else return -1; //wrong modem
  SX126X_SetRx(radioConfig.LoRaRxTimeout);
  //SX126X_setopmode(OPMODE_STBYXOSC);
  SX126X_CalibrateIR();
  SX126X_setopmode(OPMODE_STBYXOSC);
  SX126X_writereg(REG_XTATRIM,radioConfig.CtuneA);
  SX126X_writereg(REG_XTBTRIM,radioConfig.CtuneB);
  
  SX126X_SetTxParams();
  SX126X_LNAboost(true); //???
  SX126X_SetDIO2AsRfSwitchCtrl(true); //???
  //SX126X_SetDioIrqParams(TXDONE_IRQMSK | RXDONE_IRQMSK | CRCERR_IRQMSK, TXDONE_IRQMSK | RXDONE_IRQMSK, 0, 0);
  SX126X_SetDioIrqParams(TXDONE_IRQMSK | RPEDET_IRQMSK | SYNCDET_IRQMSK | RXDONE_IRQMSK | CRCERR_IRQMSK, TXDONE_IRQMSK | RPEDET_IRQMSK | SYNCDET_IRQMSK | RXDONE_IRQMSK, 0, 0);
	
	lorac_rfsw_on();
  SX126X_setopmode(OPMODE_RX);
  return 0;
}

#define RFSW_CTRL_PORT	GPIOA
#define RFSW_CTRL_PIN		GPIO_PIN_10 //GPIO10
#define RFSW_TXRX_PORT	GPIOD
#define RFSW_TXRX_PIN		GPIO_PIN_11 //GPIO59

int8_t SX126xx_LoRaConfig(void)
{
//2. Define the protocol (LoRa® or FSK) with the command SetPacketType(...)
    SX126X_SetLoRaMode(true);
//3. Define the RF frequency with the command SetRfFrequency(...)
    Channel = radioConfig.Channel;
    SX126X_SetChannel();
//4. Define where the data will be stored inside the data buffer in Rx with the command SetBufferBaseAddress(...)
    SX126X_SetBufferBaseAddress(0, 0x80);
//5. Define the modulation parameter according to the chosen protocol with the command SetModulationParams(...)1
    SX126X_SetLoRaModParams(radioConfig.LoRaSf,radioConfig.LoRaBw,radioConfig.LoRaCr);
//6. Define the frame format to be used with the command SetPacketParams(...)
    SX126X_SetLoRaPacketParams(radioConfig.LoRaPreLen, radioConfig.LoRaImplHeader,radioConfig.LoRaPayLen,radioConfig.LoRaCrcOn,radioConfig.LoRaInvertIQ);
//7. Configure DIO and irq: use the command SetDioIrqParams(...) to select the IRQ RxDone and map this IRQ to a DIO (DIO1
//or DIO2 or DIO3), set IRQ Timeout as well.

    //SX126X_SetDioIrqParams(radioConfig.LoRaIRQmsk,radioConfig.LoRaDIO1msk,radioConfig.LoRaDIO2msk,radioConfig.LoRaDIO3msk);

//8. Define Sync Word value: use the command WriteReg(...) to write the value of the register via direct register access.
    SX126X_writereg(REG_LRSYNC_H,(radioConfig.LoRaSyncWord & 0xff00) >> 8);
    SX126X_writereg(REG_LRSYNC_L,radioConfig.LoRaSyncWord & 0xff);
//9. Set the circuit in reception mode: use the command SetRx(). Set the parameter to enable timeout or continuous mode
//10. Wait for IRQ RxDone2 or Timeout: the chip will stay in Rx and look for a new packet if the continuous mode is selected
//otherwise it will goes to STDBY_RC mode.
//11. In case of the IRQ RxDone, check the status to ensure CRC is correct: use the command GetIrqStatus()
    return 0;
}

int8_t SX126xx_FskConfig(void)
{
  uint8_t sw[8];
  
  SX126X_SetLoRaMode(false);
//3. Define the RF frequency with the command SetRfFrequency(...)
  Channel = radioConfig.Channel;
  SX126X_SetChannel();
//4. Define where the data will be stored inside the data buffer in Rx with the command SetBufferBaseAddress(...)
  SX126X_SetBufferBaseAddress(0, 0x80);
  
  SX126X_SetFskModParams(radioConfig.FskBr,radioConfig.FskShaping,radioConfig.FskBw,radioConfig.FskFdev);
  //SX126X_SetFskPacketParams();
  SX126X_SetFskPacketParams(radioConfig.FskPreLen,radioConfig.FskPreDetLen,radioConfig.FskSyncLen,radioConfig.FskAddrComp,
                            radioConfig.FskVarLen,radioConfig.FskPayLen,radioConfig.FskCrcType,radioConfig.FskWhiteEn);
  //set syncword
  for(uint8_t i = 0; i < 8; i++) sw[i] = radioConfig.FskSync[i];
  SX126X_SetFskSyncWord(sw);
  SX126X_SetFskAddr(radioConfig.FskNodeAddr,radioConfig.FskBrAddr);
  //set CRC & whitening
  SX126X_SetFskCrcWhitening(radioConfig.FskCrcInit,radioConfig.FskCrcPoly,radioConfig.FskWhiteInit);
  //SX126X_writereg(REG_BITSYNC,0);
  return 0;
}


int8_t SX126X_initconfigstructure(void)
{
  uint8_t i;

//  FLASH_Unlock(FLASH_MemType_Data);
  //Common calibration parameters
  radioConfig.MagicNumber = MAGICNUMBER;
  radioConfig.deviceID = 0xffff;
  //radioConfig.CtuneA = 16;
  //radioConfig.CtuneB = 16;
  radioConfig.XoTempFactor = 0;
  radioConfig.CalTemp = 25;

  radioConfig.modem = MODEM_LORA;

  radioConfig.RfFreq = 380000000;
  radioConfig.ChannelStep = 100000;
  radioConfig.Channel = 0;
  radioConfig.TxPowerDbm = 10;
  radioConfig.RampTimeUs = 10;

  radioConfig.rtctimeout = false;
  radioConfig.warmstart = false;

  //LoRa config
  radioConfig.LoRaSf = 8; //spreading factor
  radioConfig.LoRaBw = LORA_BW_41p7;//LORA_BW_  20p8; //LORA_BW_7p8; //LORA_BW_125; //bandwidth
  radioConfig.LoRaCr = LORA_CR_4_5; //coding rate
  radioConfig.LoRaLowRateOpt = true;
  radioConfig.LoRaPreLen = 64;
  radioConfig.LoRaImplHeader = false;
  radioConfig.LoRaPayLen = 32;
  radioConfig.LoRaCrcOn = true;
  radioConfig.LoRaInvertIQ = false;
  radioConfig.LoRaTxTimeout = 0;
  radioConfig.LoRaRxTimeout = 0;
  radioConfig.LoRaSyncWord = 0xc96c;
  //FSK parameters
  radioConfig.FskBr = 50000; //baud rate
  radioConfig.FskShaping = FSK_BT0p5;
  radioConfig.FskBw = FSK_BW_93p8;
  radioConfig.FskFdev = 25000;
  radioConfig.FskPreLen = 32;
  radioConfig.FskPreDetLen = 4; //8 bits
  radioConfig.FskSyncLen = 16;
  radioConfig.FskSync[0] = 0xc9;
  radioConfig.FskSync[1] = 0x6c;
  radioConfig.FskSync[2] = 0x23;
  radioConfig.FskSync[3] = 0x6b;
  radioConfig.FskSync[4] = 0xc9;
  radioConfig.FskSync[5] = 0x6c;
  radioConfig.FskSync[6] = 0x23;
  radioConfig.FskSync[7] = 0x6b;
  radioConfig.FskAddrComp = 0; //addr. filtering disable
  radioConfig.FskNodeAddr = 0;
  radioConfig.FskBrAddr = 0; //broadcast addr
  radioConfig.FskVarLen = false;
  radioConfig.FskPayLen = 32;
  radioConfig.FskCrcType = FSK_CRC_1B;
  radioConfig.FskCrcInit = 0x1d0f;
  radioConfig.FskCrcPoly = 0x1021;
  radioConfig.FskWhiteEn = true;
  radioConfig.FskWhiteInit = 0x0100;
  radioConfig.FskTxTimeout = 0;
  radioConfig.FskRxTimeout = 0;
  
  radioConfig.InterPacketDelay = 5000;
  radioConfig.SlaveID = 0xffff;
  radioConfig.AesEnabled = false;
  for(i = 0; i < 32; i++) radioConfig.AesKey[i] = i;
	writeconfig();
  printf("Config init OK.\r\n");
  return 0;
}


void SX126X_writereg(uint16_t reg, uint8_t val)
{
  lorac_TXbuffer[0] = CMD_WRITE_REGISTER;
  lorac_TXbuffer[1] = (uint8_t)((reg & 0xff00) >> 8);
  lorac_TXbuffer[2] = (uint8_t)(reg & 0xff);
  lorac_TXbuffer[3] = val;
  lorac_transferblock(4);
}

void SX126X_writeregblock(uint16_t startaddr, uint8_t len, uint8_t *buf)
{
  uint8_t i;
  
  lorac_TXbuffer[0] = CMD_WRITE_REGISTER;
  lorac_TXbuffer[1] = (uint8_t)((startaddr & 0xff00) >> 8);
  lorac_TXbuffer[2] = (uint8_t)(startaddr & 0xff);
  for(i = 0; i < len; i++) lorac_TXbuffer[i+3] = buf[i];
  lorac_transferblock(len + 3);
}

uint8_t SX126X_readreg(uint16_t reg)
{
  lorac_TXbuffer[0] = CMD_READ_REGISTER;
  lorac_TXbuffer[1] = (uint8_t)((reg & 0xff00) >> 8);
  lorac_TXbuffer[2] = (uint8_t)(reg & 0xff);
  lorac_TXbuffer[3] = 0;
  lorac_TXbuffer[4] = 0;
  lorac_transferblock(5);
  return lorac_RXbuffer[4];
}

void SX126X_readregblock(uint16_t startaddr, uint8_t len, uint8_t *buf)
{
  //uint8_t i;
  
  //lorac_TXbuffer[0] = CMD_READ_REGISTER;
  //lorac_TXbuffer[1] = (uint8_t)((reg & 0xff00) >> 8);
  //lorac_TXbuffer[2] = (uint8_t)(reg & 0xff);
}

void SX126X_writebuffer(uint8_t offset, uint8_t len, uint8_t *buf)
{
  uint8_t i;
  lorac_TXbuffer[0] = CMD_WRITE_BUFFER;
  lorac_TXbuffer[1] = offset;
  for(i = 0; i < len; i++) lorac_TXbuffer[i+2] = buf[i];
  lorac_transferblock(len + 2);
}

void SX126X_readbuffer(uint8_t offset, uint8_t len, uint8_t *buf)
{
  uint8_t i;
  lorac_TXbuffer[0] = CMD_READ_BUFFER;
  lorac_TXbuffer[1] = offset;
  for(i = 0; i < len + 1; i++) lorac_RXbuffer[i+2] = 0;
  lorac_transferblock(len + 3);
  for(i = 0; i < len; i++) buf[i] = lorac_RXbuffer[i+3];
}

uint8_t sx126x_GetStatus(void)
{
  lorac_TXbuffer[0] = CMD_GET_STATUS;
  lorac_TXbuffer[1] =0;
  lorac_TXbuffer[2] =0;
  lorac_transferblock(3);
  return lorac_RXbuffer[2];
}

void SX126X_SetSleep(bool rtctimeout,bool warmstart)
{
  //bit 0: RTC timeout
  //bit 2: cold/warm start
  uint8_t v = 0;
  if(rtctimeout) v |= 0x01;
  if(warmstart) v |= 0x04;
  lorac_TXbuffer[0] = CMD_SET_SLEEP;
  lorac_TXbuffer[1] = v;
  lorac_transferblock(2);
  //wait 500 us
}

void SX126X_SetStby(bool xosc)
{
  uint8_t v = 0;
  if(xosc) v |= 1;
  lorac_TXbuffer[0] = CMD_SET_STANDBY;
  lorac_TXbuffer[1] = v;
  lorac_transferblock(2);
}

void SX126X_SetFs(void)
{
  lorac_TXbuffer[0] = CMD_SET_FS;
  lorac_transferblock(1);
}

void SX126X_SetTx(uint32_t timeout)
{
  if(timeout > 0x00ffffff) timeout = 0x00ffffff;
  lorac_TXbuffer[0] = CMD_SET_TX;
  lorac_TXbuffer[1] = (uint8_t)((timeout & 0xff0000) >> 16);
  lorac_TXbuffer[2] = (uint8_t)((timeout & 0xff00) >> 8);
  lorac_TXbuffer[3] = (uint8_t)(timeout & 0xff);
  lorac_transferblock(4);
}

void SX126X_SetRx(uint32_t timeout)
{
  if(timeout > 0x00ffffff) timeout = 0x00ffffff;
  lorac_TXbuffer[0] = CMD_SET_RX;
  lorac_TXbuffer[1] = (uint8_t)((timeout & 0xff0000) >> 16);
  lorac_TXbuffer[2] = (uint8_t)((timeout & 0xff00) >> 8);
  lorac_TXbuffer[3] = (uint8_t)(timeout & 0xff);
  lorac_transferblock(4);
}

void SX126X_StopTimerOnPreamble(bool stop)
{
  uint8_t v = 0;
  if(stop) v |= 1;
  lorac_TXbuffer[0] = CMD_SET_STOPRXTIMERONPREAMBLE;
  lorac_TXbuffer[1] = v;
  lorac_transferblock(2);
}

void SX126X_SetRxDutyCycle(uint32_t rxperiod, uint32_t sleepperiod)
{
  if(rxperiod > 0x00ffffff) rxperiod = 0x00ffffff;
  if(sleepperiod > 0x00ffffff) sleepperiod = 0x00ffffff;
  lorac_TXbuffer[0] = CMD_SET_RXDUTYCYCLE;
  lorac_TXbuffer[1] = (uint8_t)((rxperiod & 0xff0000) >> 16);
  lorac_TXbuffer[2] = (uint8_t)((rxperiod & 0xff00) >> 8);
  lorac_TXbuffer[3] = (uint8_t)(rxperiod & 0xff);
  lorac_TXbuffer[4] = (uint8_t)((sleepperiod & 0xff0000) >> 16);
  lorac_TXbuffer[5] = (uint8_t)((sleepperiod & 0xff00) >> 8);
  lorac_TXbuffer[6] = (uint8_t)(sleepperiod & 0xff);
  lorac_transferblock(7);
}

void SX126X_SetCAD(void)
{
  lorac_TXbuffer[0] = CMD_SET_CAD;
  lorac_transferblock(1);
}

void SX126X_SetCW(void)
{
  lorac_TXbuffer[0] = CMD_SET_TXCONTINUOUSWAVE;
  lorac_transferblock(1);
}

void SX126X_SetTxInfinitePreamble(void)
{
  lorac_TXbuffer[0] = CMD_SET_TXCONTINUOUSPREAMBLE;
  lorac_transferblock(1);
}

void SX126X_SetRegulatorMode(bool dcdc)
{
  uint8_t v = 0;
  if(dcdc) v |= 1;
  lorac_TXbuffer[0] = CMD_SET_REGULATORMODE;
  lorac_TXbuffer[1] = v;
  lorac_transferblock(2);
}

void SX126X_Calibrate(bool rc64k, bool rc13M, bool pll, bool adc, bool adcbulkn, bool adcbulkp, bool image)
{
  uint8_t v = 0;
  if(rc64k) v |= 1;
  if(rc13M) v |= 2;
  if(pll) v |= 4;
  if(adc) v |= 8;
  if(adcbulkn) v |= 0x10;
  if(adcbulkp) v |= 0x20;
  if(image) v |= 0x40;
  lorac_TXbuffer[0] = CMD_CALIBRATE;
  lorac_TXbuffer[1] = v;
  lorac_transferblock(2);
}

void SX126X_CalibrateImage(uint8_t f1, uint8_t f2)
{
  lorac_TXbuffer[0] = CMD_CALIBRATEIMAGE;
  lorac_TXbuffer[1] = f1;
  lorac_TXbuffer[2] = f2;
  lorac_transferblock(3);
  lorac_wait_on_busy();
}

void SX126X_SetRxTxFallbackMode(uint8_t mode)
{
  if(mode > 2) mode = 2;
  lorac_TXbuffer[0] = CMD_SET_TXFALLBACKMODE;
  switch(mode)
  {
  case 0:
  default:
    lorac_TXbuffer[1] = 0x20; //STDBY_RC
    break;
  case 1:
    lorac_TXbuffer[1] = 0x30;//
    break;
  case 2:
    lorac_TXbuffer[1] = 0x40;//
    break;
  }
  lorac_transferblock(2);
}

void SX126X_SetDioIrqParams(uint16_t IRQmsk, uint16_t DIO1msk, uint16_t DIO2msk, uint16_t DIO3msk)
{
  lorac_TXbuffer[0] = CMD_CFG_DIOIRQ;
  lorac_TXbuffer[1] = (uint8_t)((IRQmsk & 0xff00) >> 8);
  lorac_TXbuffer[2] = (uint8_t)(IRQmsk & 0xff);
  lorac_TXbuffer[3] = (uint8_t)((DIO1msk & 0xff00) >> 8);
  lorac_TXbuffer[4] = (uint8_t)(DIO1msk & 0xff);
  lorac_TXbuffer[5] = (uint8_t)((DIO2msk & 0xff00) >> 8);
  lorac_TXbuffer[6] = (uint8_t)(DIO2msk & 0xff);
  lorac_TXbuffer[7] = (uint8_t)((DIO3msk & 0xff00) >> 8);
  lorac_TXbuffer[8] = (uint8_t)(DIO3msk & 0xff);
  lorac_transferblock(9);
}

uint16_t SX126X_GetIrqStatus(void)
{
  lorac_TXbuffer[0] = CMD_GET_IRQSTATUS;
  lorac_TXbuffer[1] = 0;
  lorac_TXbuffer[2] = 0;
  lorac_TXbuffer[3] = 0;
  lorac_transferblock(4);
  return ((uint16_t)lorac_RXbuffer[2] << 8) + lorac_RXbuffer[3];
}

void SX126X_ClearIrqStatus(uint16_t msk)
{
  lorac_TXbuffer[0] = CMD_CLR_IRQSTATUS;
  lorac_TXbuffer[1] = (uint8_t)((msk & 0xff00) >> 8);
  lorac_TXbuffer[2] = (uint8_t)(msk & 0xff);
  lorac_transferblock(3);
}

void SX126X_SetDIO2AsRfSwitchCtrl(bool enable)
{
  lorac_TXbuffer[0] = CMD_SET_RFSWITCHMODE;
  if(enable) lorac_TXbuffer[1] = 1;
  else lorac_TXbuffer[1] = 0;
  lorac_transferblock(2);
}

void SX126X_SetDIO3AsTCXOCtrl(uint8_t voltage, uint32_t timeout)
{
  lorac_TXbuffer[0] = CMD_SET_TCXOMODE;
  lorac_TXbuffer[1] = voltage;
  if(timeout > 0xffffff) timeout = 0xffffff;
  lorac_TXbuffer[2] = (uint8_t)((timeout & 0xff0000) >> 16);
  lorac_TXbuffer[3] = (uint8_t)((timeout & 0xff00) >> 8);
  lorac_TXbuffer[4] = (uint8_t)(timeout & 0xff);
  lorac_transferblock(5);
}

void SX126X_SetRfFrequency(uint32_t freqcode)
{
  lorac_TXbuffer[0] = CMD_SET_RFFREQUENCY;
  lorac_TXbuffer[1] = (uint8_t)((freqcode & 0xff000000) >> 24);
  lorac_TXbuffer[2] = (uint8_t)((freqcode & 0xff0000) >> 16);
  lorac_TXbuffer[3] = (uint8_t)((freqcode & 0xff00) >> 8);
  lorac_TXbuffer[4] = (uint8_t)(freqcode & 0xff);
  lorac_transferblock(5);
}

//void SX126X_SetPacketType(bool loramode)
void SX126X_SetLoRaMode(bool loramode)
{
  lorac_TXbuffer[0] = CMD_SET_PACKETTYPE;
  if(loramode) lorac_TXbuffer[1] = 1;
  else lorac_TXbuffer[1] = 0;
  //else lorac_TXbuffer[1] = 2;
  lorac_transferblock(2);
}

//bool SX126X_GetPacketType(void)
bool SX126X_GetLoRaMode(void)
{
  lorac_TXbuffer[0] = CMD_GET_PACKETTYPE;
  lorac_TXbuffer[1] = 0;
  lorac_TXbuffer[2] = 0;
  lorac_transferblock(3);
  if(lorac_RXbuffer[2] == 1) return true;
  else return false;
}

void SX126X_SetTxParams(void)
{
  uint8_t time;
 /* uint8_t padutycycle;
  uint8_t hpmax;
  
  if(radioConfig.TxPowerDbm >=21) 
  {
    padutycycle = 4;
    hpmax = 7;
  }
  else if(radioConfig.TxPowerDbm >=18) 
  {
    padutycycle = 3;
    hpmax = 5;
  }
  else if(radioConfig.TxPowerDbm >=10) 
  {
    padutycycle = 2;
    if(radioConfig.TxPowerDbm > 14) hpmax = 3;
    else hpmax = 2;
  }
  else 
  {
    padutycycle = 1; //not sure
    hpmax = 1; //not sure
  }
*/
  //SX126X_SetPaConfig(padutycycle, hpmax, false);
  SX126X_SetPaConfig(4, 7, false);
  SX126X_writereg(REG_OCPCONFIG, 0x38);// 140 mA; current max 160mA for the whole device
  lorac_TXbuffer[0] = CMD_SET_TXPARAMS;
  lorac_TXbuffer[1] = radioConfig.TxPowerDbm;
  if(radioConfig.RampTimeUs >= 3400) time = 7;
  if((radioConfig.RampTimeUs >= 1700) && (radioConfig.RampTimeUs < 3400)) time = 6;
  if((radioConfig.RampTimeUs >= 800) && (radioConfig.RampTimeUs < 1700)) time = 5;
  if((radioConfig.RampTimeUs >= 200) && (radioConfig.RampTimeUs < 800)) time = 4;
  if((radioConfig.RampTimeUs >= 80) && (radioConfig.RampTimeUs < 200)) time = 3;
  if((radioConfig.RampTimeUs >= 40) && (radioConfig.RampTimeUs < 80)) time = 2;
  if((radioConfig.RampTimeUs >= 20) && (radioConfig.RampTimeUs < 40)) time = 1;
  else time = 0;
  lorac_TXbuffer[2] = time;
  lorac_transferblock(3);
  //TxClampConfig 
  uint8_t reg = SX126X_readreg(REG_TXCLAMPCONFIG); //(0x08d8);
  reg |= 0x1e;
  SX126X_writereg(REG_TXCLAMPCONFIG,reg);
}

void SX126X_SetPaConfig(uint8_t dutycycle, uint8_t hpmax, bool lp)
{
  lorac_TXbuffer[0] = CMD_SET_PACONFIG;
  lorac_TXbuffer[1] = dutycycle;
  if(hpmax > 7) hpmax = 7; //protect PA
  lorac_TXbuffer[2] = hpmax;
  if(lp) lorac_TXbuffer[3] = 1; //paLut
  else lorac_TXbuffer[3] = 0;
  lorac_TXbuffer[4] = 1; //paLut
  lorac_transferblock(5);
}


void SX126X_SetFskModParams(uint32_t br, uint8_t shaping, uint8_t rxbw, uint32_t fdev)
{
  uint32_t br_val, dev_val;
  
  br_val = (FXO * 32) / br;
  if(br_val == 0) br_val = 1;
  if(br_val > 0xffffff) br_val = 0xffffff;
  //dev_val = (uint32_t)(fdev / ((double)FXO / 33554432));
  dev_val = ((fdev % 15625) * 16384 + 7812) / 15625 + (fdev / 15625) * 16384;
  if(dev_val > 0xffffff) dev_val = 0xffffff;
    
  lorac_TXbuffer[0] = CMD_SET_MODULATIONPARAMS;
  lorac_TXbuffer[1] = (uint8_t)((br_val & 0xff0000) >> 16);
  lorac_TXbuffer[2] = (uint8_t)((br_val & 0xff00) >> 8);
  lorac_TXbuffer[3] = (uint8_t)(br_val & 0xff);
  lorac_TXbuffer[4] = shaping;
  lorac_TXbuffer[5] = rxbw;
  lorac_TXbuffer[6] = (uint8_t)((dev_val & 0xff0000) >> 16);
  lorac_TXbuffer[7] = (uint8_t)((dev_val & 0xff00) >> 8);
  lorac_TXbuffer[8] = (uint8_t)(dev_val & 0xff);
  lorac_transferblock(9);
  
  //uint8_t reg = SX126X_readreg(REG_TXMOD); //(0x0889);
  //reg |= 0x4;
  //SX126X_writereg(REG_TXMOD,reg);
}

void SX126X_SetLoRaModParams(uint8_t sf, uint8_t bw, uint8_t cr)
{
  if(sf < 5) sf = 5;
  if(sf > 12) sf = 12;
  lorac_TXbuffer[0] = CMD_SET_MODULATIONPARAMS;
  lorac_TXbuffer[1] = sf;
  lorac_TXbuffer[2] = bw;
  lorac_TXbuffer[3] = cr;
  if(sf > 10) lorac_TXbuffer[4] = 1;
  else lorac_TXbuffer[4] = 0;
  lorac_transferblock(5);
  uint8_t reg = SX126X_readreg(REG_TXMOD); //(0x0889);
  if(bw == LORA_BW_500) reg &= 0xfb;
  else reg |= 4;
  SX126X_writereg(REG_TXMOD,reg);//(0x0889,reg);
}

void SX126X_SetFskPacketParams(uint16_t prelen, uint8_t pdetlen, uint8_t synclen, uint8_t addrfilt, bool varlen, uint8_t paylen, uint8_t crctype, bool white)
{
  if(prelen == 0) prelen = 1;
  lorac_TXbuffer[0] = CMD_SET_PACKETPARAMS;
  lorac_TXbuffer[1] = (uint8_t)((prelen & 0xff00) >> 8);
  lorac_TXbuffer[2] = (uint8_t)(prelen & 0xff);
  if((pdetlen > 0) && (pdetlen < 4)) pdetlen = 4;
  if(pdetlen > 7) pdetlen = 7;
  lorac_TXbuffer[3] = pdetlen;
  if(synclen > 0x40) synclen = 0x40;
  lorac_TXbuffer[4] = synclen;
  if(addrfilt > 2) addrfilt = 2;
  lorac_TXbuffer[5] = addrfilt;
  if(varlen) lorac_TXbuffer[6] = 1;
  else lorac_TXbuffer[6] = 0;
  lorac_TXbuffer[7] = paylen;
  lorac_TXbuffer[8] = crctype;
  if(white) lorac_TXbuffer[9] = 1;
  else lorac_TXbuffer[9] = 0;
  lorac_transferblock(10);
}

void SX126X_SetFskAddr(uint8_t node_addr,uint8_t br_addr)
{
  SX126X_writereg(REG_NODEADDR,node_addr);
  SX126X_writereg(REG_BROADCASTADDR,br_addr);
}

void SX126X_SetFskSyncWord(uint8_t *sync)
{
  for(uint8_t i = 0; i < 8; i++) SX126X_writereg(REG_SYNC0 + i, sync[i]);
}

void SX126X_SetFskCrcWhitening(uint16_t crcinit, uint16_t crcpoly, uint16_t whiteinit)
{
  uint8_t reg;
  
  SX126X_writereg(REG_CRCINIT_H, (crcinit & 0xff00) >> 8);
  SX126X_writereg(REG_CRCINIT_L, crcinit & 0xff);
  SX126X_writereg(REG_CRCPOLY_H, (crcpoly & 0xff00) >> 8);
  SX126X_writereg(REG_CRCPOLY_L, crcpoly & 0xff);
  reg = SX126X_readreg(REG_WHITEINIT_H);
  reg &= ~0x01;
  reg |= ((whiteinit & 0xff00) >> 8) & 0x01;
  SX126X_writereg(REG_WHITEINIT_H, reg);
  SX126X_writereg(REG_WHITEINIT_L, whiteinit & 0xff);
}

void SX126X_SetLoRaPacketParams(uint16_t prelen, bool implheader, uint8_t paylen, bool crcon, bool invertIQ)
{
  if(prelen == 0) prelen = 1;
  lorac_TXbuffer[0] = CMD_SET_PACKETPARAMS;
  lorac_TXbuffer[1] = (uint8_t)((prelen & 0xff00) >> 8);
  lorac_TXbuffer[2] = (uint8_t)(prelen & 0xff);
  if(implheader) lorac_TXbuffer[3] = 1;
  else lorac_TXbuffer[3] = 0;
  lorac_TXbuffer[4] = paylen;
  if(crcon) lorac_TXbuffer[5] = 1;
  else lorac_TXbuffer[5] = 0;
  if(invertIQ) lorac_TXbuffer[6] = 1;
  else lorac_TXbuffer[6] = 0;
  lorac_transferblock(7);
  uint8_t reg = SX126X_readreg(REG_IQPOL); //(0x736);
  if(invertIQ) reg &= 0xfb;
  else reg |= 4;
  SX126X_writereg(REG_IQPOL,reg);
}

void SX126X_SetCadParams(uint8_t cadSymbolNum, uint8_t cadDetPeak, uint8_t cadDetMin, bool cadrx, uint32_t cadTimeout)
{
  if(cadSymbolNum > 4) cadSymbolNum = 4;
  lorac_TXbuffer[0] = CMD_SET_CADPARAMS;
  lorac_TXbuffer[1] = cadSymbolNum;
  lorac_TXbuffer[2] = cadDetPeak;
  lorac_TXbuffer[3] = cadDetMin;
  if(cadrx) lorac_TXbuffer[4] = 1;
  else lorac_TXbuffer[4] = 0;
  if(cadTimeout > 0xffffff) cadTimeout = 0xffffff;
  lorac_TXbuffer[5] = (uint8_t)((cadTimeout & 0xff0000) >> 16);
  lorac_TXbuffer[6] = (uint8_t)((cadTimeout & 0xff00) >> 8);
  lorac_TXbuffer[7] = (uint8_t)(cadTimeout & 0xff);
  lorac_transferblock(8);
}

void SX126X_SetBufferBaseAddress(uint8_t txaddr, uint8_t rxaddr)
{
  lorac_TXbuffer[0] = CMD_SET_BUFFERBASEADDRESS;
  lorac_TXbuffer[1] = txaddr;
  lorac_TXbuffer[2] = rxaddr;
  lorac_transferblock(3);
}

void SX126X_SetLoRaSymbNumTimeout(uint8_t SymbNum)
{
  lorac_TXbuffer[0] = CMD_SET_LORASYMBTIMEOUT;
  lorac_TXbuffer[1] = SymbNum;
  lorac_transferblock(2);
  if(SymbNum > 63) SX126X_writereg(0x706,(SymbNum & 0xf8) + 1);
}

uint8_t SX126X_GetStatus(void)
{
  lorac_TXbuffer[0] = CMD_GET_STATUS;
  lorac_TXbuffer[1] = 0;
  lorac_transferblock(2);
  return lorac_RXbuffer[1];
}

void SX126X_GetRxBufferStatus(uint8_t *status, uint8_t *PayloadLengthRx, uint8_t *RxStartBufferPointer)
{
  lorac_TXbuffer[0] = CMD_GET_RXBUFFERSTATUS;
  lorac_TXbuffer[1] = 0;
  lorac_TXbuffer[2] = 0;
  lorac_TXbuffer[3] = 0;
  lorac_transferblock(4);
  *status = lorac_RXbuffer[1];
  *PayloadLengthRx = lorac_RXbuffer[2];
  *RxStartBufferPointer = lorac_RXbuffer[3];
}

void SX126X_GetFskPacketStatus(uint8_t *Status, uint8_t *RxStatus, uint8_t *RssiSync, uint8_t *RssiAvg)
{
  lorac_TXbuffer[0] = CMD_GET_PACKETSTATUS;
  lorac_TXbuffer[1] = 0;
  lorac_TXbuffer[2] = 0;
  lorac_TXbuffer[3] = 0;
  lorac_TXbuffer[4] = 0;
  lorac_transferblock(5);
  *Status = lorac_RXbuffer[1];
  *RxStatus = lorac_RXbuffer[2];
  *RssiSync = lorac_RXbuffer[3];
  *RssiAvg = lorac_RXbuffer[4];
}

void SX126X_GetLoRaPacketStatus(uint8_t *Status, uint8_t *RssiPkt, int16_t *SnrPkt, uint8_t *SignalRssiPkt)
{
  lorac_TXbuffer[0] = CMD_GET_PACKETSTATUS;
  lorac_TXbuffer[1] = 0;
  lorac_TXbuffer[2] = 0;
  lorac_TXbuffer[3] = 0;
  lorac_TXbuffer[4] = 0;
  lorac_transferblock(5);
  *Status = lorac_RXbuffer[1];
  *RssiPkt = lorac_RXbuffer[2];
  *SnrPkt = lorac_RXbuffer[3];
  *SignalRssiPkt = lorac_RXbuffer[4];
}

uint8_t SX126X_GetRssiInst(void)
{
  lorac_TXbuffer[0] = CMD_GET_RSSIINST;
  lorac_TXbuffer[1] = 0;
  lorac_TXbuffer[2] = 0;
  lorac_transferblock(3);
  return lorac_RXbuffer[2];
}

void SX126X_FskGetStats(uint8_t *Status, uint16_t *NbPktReceived, uint16_t *NbPktCrcError, uint16_t *NbPktLengthError)
{
  uint8_t i;
  lorac_TXbuffer[0] = CMD_GET_STATS;
  for(i = 1; i < 8; i++) lorac_TXbuffer[i] = 0;
  lorac_transferblock(8);
  *Status = lorac_RXbuffer[1];
  *NbPktReceived = ((uint16_t)lorac_RXbuffer[2] << 8) + lorac_RXbuffer[3];
  *NbPktCrcError = ((uint16_t)lorac_RXbuffer[4] << 8) + lorac_RXbuffer[5];
  *NbPktLengthError = ((uint16_t)lorac_RXbuffer[6] << 8) + lorac_RXbuffer[7];
}

void SX126X_LoRaGetStats(uint8_t *Status, uint16_t *NbPktReceived, uint16_t *NbPktCrcError, uint16_t *NbPktHeaderErr)
{
  uint8_t i;
  lorac_TXbuffer[0] = CMD_GET_STATS;
  for(i = 1; i < 8; i++) lorac_TXbuffer[i] = 0;
  lorac_transferblock(8);
  *Status = lorac_RXbuffer[1];
  *NbPktReceived = ((uint16_t)lorac_RXbuffer[2] << 8) + lorac_RXbuffer[3];
  *NbPktCrcError = ((uint16_t)lorac_RXbuffer[4] << 8) + lorac_RXbuffer[5];
  *NbPktHeaderErr = ((uint16_t)lorac_RXbuffer[6] << 8) + lorac_RXbuffer[7];
}

void SX126X_ResetStats(void)
{
  uint8_t i;
  //lorac_TXbuffer[0] = CMD_RESET_STATS;
  for(i = 0; i < 7; i++) lorac_TXbuffer[i] = 0;
  lorac_transferblock(7);
}

void SX126X_GetDeviceErrors(uint8_t *Status, uint16_t *OpError)
{
  lorac_TXbuffer[0] = CMD_GET_ERROR;
  lorac_TXbuffer[1] = 0;
  lorac_TXbuffer[2] = 0;
  lorac_TXbuffer[3] = 0;
  lorac_transferblock(4);
  *Status = lorac_RXbuffer[1];
  *OpError = ((uint16_t)lorac_RXbuffer[2] << 8) + lorac_RXbuffer[3];
}

void SX126X_ClearDeviceErrors(void)
{
  lorac_TXbuffer[0] = CMD_CLR_ERROR;
  lorac_TXbuffer[1] = 0;
  lorac_transferblock(2);
}

void SX126X_SetChannel(void)
{
  //here maybe save and restore working mode
  double step;
  step = (double)FXO / 33554432;
  uint32_t freq;
  freq = radioConfig.RfFreq + Channel * radioConfig.ChannelStep;
  SX126X_SetRfFrequency((uint32_t)(freq / step));
}

void SX126X_setopmode(uint8_t mode)
{
  switch(mode)
  {
    case OPMODE_SLEEP:
    opmode = OPMODE_SLEEP;
    SX126X_SetSleep(radioConfig.rtctimeout,radioConfig.warmstart);
    break;

    case OPMODE_STBYRC:
    opmode = OPMODE_STBYRC;
    SX126X_SetStby(false);
    break;

    case OPMODE_STBYXOSC:
    opmode = OPMODE_STBYXOSC;
    SX126X_SetStby(true);
    break;

    case OPMODE_FS:
    opmode = OPMODE_FS;
    SX126X_SetFs();
    break;

    case OPMODE_TX:
    opmode = OPMODE_TX;
    SX126X_SetTx(radioConfig.LoRaTxTimeout); //temp.
    break;

    case OPMODE_RX:
    default:
    opmode = OPMODE_RX;
    //SX126X_SetRx(radioConfig.LoRaRxTimeout); //temp.
    SX126X_SetRx(0); //temp.
    break;

    case OPMODE_TXSTREAMCW:
    opmode = OPMODE_TXSTREAMCW;
    SX126X_SetCW();
    break;

    case OPMODE_TXSTREAMPRE:
    opmode = OPMODE_TXSTREAMPRE;
    SX126X_SetTxInfinitePreamble();
    break;
  }
}

//boost LNA
void SX126X_LNAboost(bool boost)
{
  if(boost) SX126X_writereg(0x08ac,0x96);
  else SX126X_writereg(0x08ac,0x94);
}

//calibrate IR according to RF ftequency
//The calibration frequencies are computed as follows:
//Calibration freq = CalFreq * 4 MHz where CalFreq1 < CalFreq2
//Example: 0x6B = 428 MHz.
//When CalFreq1 = CalFreq2, the image calibration is done at a single frequency.
//For frequencies between CalFreq1 and CalFreq2, the calibration coefficient is linearly
//interpolated from the values obtained during the image calibration at CalFreq1 and CalFreq2.
//For frequencies < CalFreq1, the coefficient obtained during the image calibration at CalFreq1 is used.
//For frequencies > CalFreq2, the coefficient obtained during the image calibration at CalFreq2 is used.

void SX126X_CalibrateIR(void)
{
  uint8_t f1,f2;
  uint32_t f;
  f = radioConfig.RfFreq / 1000000UL;
  if(f > 1020) f = 1020;
  f1 = (uint8_t)(f / 4);
  f = (f * 3) / 2; //probably 1.5 times
  if(f > 1020) f = 1020;
  f2 = (uint8_t)(f / 4);
  SX126X_CalibrateImage(f1,f2);
  //restore Ctune needed
}


