#include "SX126X_config.h"
#include "sx126x.h"
#include "radio.h"


void SX126X_configcommon(void);

int8_t SX126X_config(void)
{
	SX126X_setopmode(RADIO_OPMODE_STBYXOSC);
	SX126X_writeReg(0,SX126X_REG_XTATRIM,radioconfig.xta);
	SX126X_writeReg(0,SX126X_REG_XTBTRIM,radioconfig.xtb);
	SX126X_SetPacketType(0,SX126X_MODEM_LORA);
	SX126X_SetLoRaModParams(0,radioconfig.sf,radioconfig.bw,radioconfig.cr);
	SX126X_SetLoRaPacketParams(0,radioconfig.loraprelen, radioconfig.implheader,radioconfig.lorapaylen,radioconfig.loracrc,radioconfig.invertiq);
	//set sync
	//It is also possible to configure the LoRa SyncWord. With the 1 byte SynchWord taking the format 0xXY, it is written across the MSB of two registers as described below:
	//write X in the register@0x944, in position [7:4] without modifying [3:0] (using a Read / Modify / Write operation)
	//write Y in the register@0x945, in position [7:4] without modifying [3:0] (using a Read / Modify / Write operation
	//SX126X_readRegs(sx,SX126X_REG_LRSYNC_H,buf,2);
	//buf[0] &= 0x0f;
	//buf[1] &= 0x0f;
	//tmp = radioconfig.lorasync & 0xf0;
	//buf[0] |= tmp;
	//tmp = (radioconfig.lorasync & 0x0f) << 4;
			//buf[1] |= tmp;
	SX126X_writeReg(0,SX126X_REG_LRSYNC_H,(radioconfig.lorasync & 0xff00) >> 8);
	SX126X_writeReg(0,SX126X_REG_LRSYNC_L,radioconfig.lorasync & 0xff);
	SX126X_SetRfFrequency(0,(uint32_t)(radioconfig.frequency / SX126X_SYNTH_STEP));
	SX126X_SetBufferBaseAddress(0,0,0);
	SX126X_SetPaConfig(0,4, 7, false);
  SX126X_writeReg(0,SX126X_REG_OCPCONFIG, 0x38);// 140 mA; current max 160mA for the whole device
  SX126X_SetTxParams(0,radioconfig.txpower,radioconfig.ramptime);
	//SX126X_CalibrateIR();
	SX126X_LNAboost(0,true); //???
	SX126X_SetDIO2AsRfSwitchCtrl(0,true);
	SX126X_SetDioIrqParams(0,SX126X_TXDONE_IRQMSK | SX126X_RXDONE_IRQMSK | SX126X_CRCERR_IRQMSK,SX126X_TXDONE_IRQMSK | SX126X_RXDONE_IRQMSK,0,0);
	SX126X_setopmode(RADIO_OPMODE_RX);
	return RADIO_OK;
}

void SX126X_setopmode(uint8_t mode)
{
  switch(mode)
  {
    case RADIO_OPMODE_SLEEP:
    SX126X_SetSleep(0,0,false);
		opmode = RADIO_OPMODE_SLEEP;
    break;

    case RADIO_OPMODE_STBYRC:
    SX126X_SetStandby(0,0);
		opmode = RADIO_OPMODE_STBYRC;
    break;

    case RADIO_OPMODE_STBYXOSC:
    opmode = RADIO_OPMODE_STBYXOSC;
    break;

    case RADIO_OPMODE_FS:
    SX126X_SetFs(0);
		opmode = RADIO_OPMODE_FS;
    break;

    case RADIO_OPMODE_TX:
    SX126X_SetTx(0,0xffffff); //temp.
		opmode = RADIO_OPMODE_TX;
    break;

    case RADIO_OPMODE_RX:
    default:
    SX126X_SetRx(0,0); //temp.
		opmode = RADIO_OPMODE_RX;
    break;

    case RADIO_OPMODE_TXSTREAMCW:
    SX126X_SetCW(0);
		opmode = RADIO_OPMODE_TXSTREAMCW;
    break;

    case RADIO_OPMODE_TXSTREAMPRE:
    SX126X_SetTxInfinitePreamble(0);
		opmode = RADIO_OPMODE_TXSTREAMPRE;
    break;
  }
}
