#include "rfconfig.h"

#define SYNTH_STEP ((double)FXO / 33554432)

txconfig_t txconfig;
rxconfig_t rxconfig;

void radio_txconfig(void)
{
  //SX126X_setopmode(SX126X_OPMODE_STBYRC);
	SX126X_SetPacketType(txconfig.modem);
	if(txconfig.modem == SX126X_MODEM_LORA)
	{
		SX126X_SetRfFrequency(txconfig.rffreq / SYNTH_STEP);
		SX126X_SetBufferBaseAddress(0, 0x80);
		SX126X_SetLoRaModParams(txconfig.sf,txconfig.bw,txconfig.cr);
		SX126X_SetLoRaPacketParams(txconfig.loraprelen, txconfig.header,txconfig.lorapaylen,txconfig.loracrc,txconfig.invertiq);
		//SX126X_SetDioIrqParams(radioConfig.LoRaIRQmsk,radioConfig.LoRaDIO1msk,radioConfig.LoRaDIO2msk,radioConfig.LoRaDIO3msk);
		SX126X_SetDioIrqParams(SX126X_TXDONE_IRQMSK | SX126X_TXDONE_IRQMSK | SX126X_RPEDET_IRQMSK | SX126X_SYNCDET_IRQMSK | SX126X_RXDONE_IRQMSK, 0, 0);
		SX126X_writeReg(SX126X_REG_LRSYNC_H,(txconfig.lorasync & 0xff00) >> 8);
		SX126X_writeReg(SX126X_REG_LRSYNC_L,txconfig.lorasync & 0xff);
		SX126X_SetTxParams(); //config power
	}

  else if (txconfig.modem == SX126X_MODEM_FSK) 
	{
		SX126X_FskConfig();
	}
  else return -1; //wrong modem
  SX126X_SetRx(radioConfig.LoRaRxTimeout);
  SX126X_setopmode(SX126X_OPMODE_STBYXOSC);
  SX126X_CalibrateIR();
  //SX126X_setopmode(SX126X_OPMODE_STBYXOSC);
  //SX126X_writeReg(SX126X_REG_XTATRIM,radioConfig.CtuneA);
  //SX126X_writeReg(SX126X_REG_XTBTRIM,radioConfig.CtuneB);
  
  SX126X_SetTxParams();
  SX126X_LNAboost(true); //???
  SX126X_SetDIO2AsRfSwitchCtrl(true); //???
  //SX126X_SetDioIrqParams(TXDONE_IRQMSK | RXDONE_IRQMSK | CRCERR_IRQMSK, TXDONE_IRQMSK | RXDONE_IRQMSK, 0, 0);
  SX126X_SetDioIrqParams(SX126X_TXDONE_IRQMSK | SX126X_RPEDET_IRQMSK | SX126X_SYNCDET_IRQMSK | SX126X_RXDONE_IRQMSK | SX126X_CRCERR_IRQMSK, SX126X_TXDONE_IRQMSK | SX126X_RPEDET_IRQMSK | SX126X_SYNCDET_IRQMSK | SX126X_RXDONE_IRQMSK, 0, 0);
  SX126X_setopmode(SX126X_OPMODE_RX);
  return 0;
}



void radio_rxconfig(void)
{
  SX126X_setopmode(SX126X_OPMODE_STBYRC);
	if(radioConfig.modem == SX126X_MODEM_LORA) SX126X_LoRaConfig();
  else if (radioConfig.modem == SX126X_MODEM_FSK) SX126X_FskConfig();
  else return -1; //wrong modem
  SX126X_SetRx(radioConfig.LoRaRxTimeout);
  SX126X_setopmode(SX126X_OPMODE_STBYXOSC);
  SX126X_CalibrateIR();
  //SX126X_setopmode(SX126X_OPMODE_STBYXOSC);
  //SX126X_writeReg(SX126X_REG_XTATRIM,radioConfig.CtuneA);
  //SX126X_writeReg(SX126X_REG_XTBTRIM,radioConfig.CtuneB);
  
  SX126X_SetTxParams();
  SX126X_LNAboost(true); //???
  SX126X_SetDIO2AsRfSwitchCtrl(true); //???
  //SX126X_SetDioIrqParams(TXDONE_IRQMSK | RXDONE_IRQMSK | CRCERR_IRQMSK, TXDONE_IRQMSK | RXDONE_IRQMSK, 0, 0);
  SX126X_SetDioIrqParams(SX126X_TXDONE_IRQMSK | SX126X_RPEDET_IRQMSK | SX126X_SYNCDET_IRQMSK | SX126X_RXDONE_IRQMSK | SX126X_CRCERR_IRQMSK, SX126X_TXDONE_IRQMSK | SX126X_RPEDET_IRQMSK | SX126X_SYNCDET_IRQMSK | SX126X_RXDONE_IRQMSK, 0, 0);
  SX126X_setopmode(SX126X_OPMODE_RX);
  return 0;
}
