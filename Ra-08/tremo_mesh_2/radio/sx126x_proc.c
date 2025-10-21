#include "sx126x_proc.h"

//uint8_t opmode = 0;
//uint8_t prevopmode = 0;
//uint16_t irqflags = 0;
//uint8_t rfstatus = 0;

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
  SX126X_setopmode(SX126X_OPMODE_STBYRC);
  SX126X_SetRegulatorMode(true);
  SX126X_LoRaConfig();
  SX126X_SetRx(0xffffff);
  //SX126X_setopmode(OPMODE_STBYXOSC);
  SX126X_CalibrateIR();
  SX126X_setopmode(SX126X_OPMODE_STBYXOSC);
  SX126X_writeReg(SX126X_REG_XTATRIM,radioConfig.CtuneA);
  SX126X_writeReg(SX126X_REG_XTBTRIM,radioConfig.CtuneB);
  SX126X_SetTxParams();
  SX126X_LNAboost(true); //???
  SX126X_SetDIO2AsRfSwitchCtrl(true); //???
  //SX126X_SetDioIrqParams(TXDONE_IRQMSK | RXDONE_IRQMSK | CRCERR_IRQMSK, TXDONE_IRQMSK | RXDONE_IRQMSK, 0, 0);
  SX126X_SetDioIrqParams(SX126X_TXDONE_IRQMSK | SX126X_RPEDET_IRQMSK | SX126X_SYNCDET_IRQMSK | SX126X_RXDONE_IRQMSK | SX126X_CRCERR_IRQMSK, SX126X_TXDONE_IRQMSK | SX126X_RPEDET_IRQMSK | SX126X_SYNCDET_IRQMSK | SX126X_RXDONE_IRQMSK, 0, 0);
  SX126X_setopmode(SX126X_OPMODE_RX);
  return 0;
}
int8_t SX126X_LoRaConfig(void)
{
//2. Define the protocol (LoRa® or FSK) with the command SetPacketType(...)
    SX126X_SetPacketType(SX126X_MODEM_LORA);
//3. Define the RF frequency with the command SetRfFrequency(...)
    SX126X_SetRfFrequency(radioConfig.RfFreq / SX126X_SYNTH_STEP);
//4. Define where the data will be stored inside the data buffer in Rx with the command SetBufferBaseAddress(...)
    SX126X_SetBufferBaseAddress(0,0);
//5. Define the modulation parameter according to the chosen protocol with the command SetModulationParams(...)1
    SX126X_SetLoRaModParams(radioConfig.LoRaSf,radioConfig.LoRaBw,radioConfig.LoRaCr);
//6. Define the frame format to be used with the command SetPacketParams(...)
    SX126X_SetLoRaPacketParams(radioConfig.LoRaPreLen, radioConfig.LoRaImplHeader,radioConfig.LoRaPayLen,radioConfig.LoRaCrcOn,radioConfig.LoRaInvertIQ);
//7. Configure DIO and irq: use the command SetDioIrqParams(...) to select the IRQ RxDone and map this IRQ to a DIO (DIO1
//or DIO2 or DIO3), set IRQ Timeout as well.

    //SX126X_SetDioIrqParams(radioConfig.LoRaIRQmsk,radioConfig.LoRaDIO1msk,radioConfig.LoRaDIO2msk,radioConfig.LoRaDIO3msk);

//8. Define Sync Word value: use the command WriteReg(...) to write the value of the register via direct register access.
    SX126X_writeReg(SX126X_REG_LRSYNC_H,(radioConfig.LoRaSyncWord & 0xff00) >> 8);
    SX126X_writeReg(SX126X_REG_LRSYNC_L,radioConfig.LoRaSyncWord & 0xff);
//9. Set the circuit in reception mode: use the command SetRx(). Set the parameter to enable timeout or continuous mode
//10. Wait for IRQ RxDone2 or Timeout: the chip will stay in Rx and look for a new packet if the continuous mode is selected
//otherwise it will goes to STDBY_RC mode.
//11. In case of the IRQ RxDone, check the status to ensure CRC is correct: use the command GetIrqStatus()
    return 0;
}

int8_t SX126X_initconfigstructure(void)
{
  uint8_t i;

  //Common calibration parameters
  radioConfig.MagicNumber = MAGICNUMBER;
  radioConfig.deviceID = 0x01234567;
  radioConfig.CtuneA = 16;
  radioConfig.CtuneB = 16;
  radioConfig.RfFreq = 433125000;
  radioConfig.TxPowerDbm = 10;

  //LoRa config
  radioConfig.LoRaSf = 8; //spreading factor
  radioConfig.LoRaBw = SX126X_LORA_BW_41p7;//LORA_BW_  20p8; //LORA_BW_7p8; //LORA_BW_125; //bandwidth
  radioConfig.LoRaCr = SX126X_LORA_CR_4_5; //coding rate
  radioConfig.LoRaLowRateOpt = true;
  radioConfig.LoRaPreLen = 64;
  radioConfig.LoRaImplHeader = false;
  radioConfig.LoRaPayLen = 0;
  radioConfig.LoRaCrcOn = true;
  radioConfig.LoRaInvertIQ = false;
  radioConfig.LoRaSyncWord = 0x24b4;
  
	writeconfig();
  printf("Config init OK.\r\n");
  return 0;
}

void SX126X_setopmode(uint8_t mode)
{
  switch(mode)
  {
    case SX126X_OPMODE_SLEEP:
    opmode = SX126X_OPMODE_SLEEP;
    SX126X_SetSleep(0,0);
    break;

    case SX126X_OPMODE_STBYRC:
    opmode = SX126X_OPMODE_STBYRC;
    SX126X_SetStandby(0);
    break;

    case SX126X_OPMODE_STBYXOSC:
    opmode = SX126X_OPMODE_STBYXOSC;
    SX126X_SetStandby(1);
    break;

    case SX126X_OPMODE_FS:
    opmode = SX126X_OPMODE_FS;
    SX126X_SetFs();
    break;

    case SX126X_OPMODE_TX:
    opmode = SX126X_OPMODE_TX;
    SX126X_SetTx(0xffffff); //temp.
    break;

    case SX126X_OPMODE_RX:
    default:
    opmode = SX126X_OPMODE_RX;
    SX126X_SetRx(0xffffff); //temp.
    break;

    case SX126X_OPMODE_TXSTREAMCW:
    opmode = SX126X_OPMODE_TXSTREAMCW;
    SX126X_SetCW();
    break;

    case SX126X_OPMODE_TXSTREAMPRE:
    opmode = SX126X_OPMODE_TXSTREAMPRE;
    SX126X_SetTxInfinitePreamble();
    break;
  }
}

//boost LNA
void SX126X_LNAboost(bool boost)
{
  if(boost) SX126X_writeReg(0x08ac,0x96);
  else SX126X_writeReg(0x08ac,0x94);
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




