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

double step;
  step = (double)FXO / 33554432;
((uint32_t)(freq / step));

int8_t SX126X_LoRaConfig(void)
{
//2. Define the protocol (LoRa® or FSK) with the command SetPacketType(...)
    SX126X_SetPacketType(SX126X_MODEM_LORA);
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
    SX126X_writeReg(SX126X_REG_LRSYNC_H,(radioConfig.LoRaSyncWord & 0xff00) >> 8);
    SX126X_writeReg(SX126X_REG_LRSYNC_L,radioConfig.LoRaSyncWord & 0xff);
//9. Set the circuit in reception mode: use the command SetRx(). Set the parameter to enable timeout or continuous mode
//10. Wait for IRQ RxDone2 or Timeout: the chip will stay in Rx and look for a new packet if the continuous mode is selected
//otherwise it will goes to STDBY_RC mode.
//11. In case of the IRQ RxDone, check the status to ensure CRC is correct: use the command GetIrqStatus()
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
