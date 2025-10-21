#include "radio_func.h"

//globals
//buffers
uint8_t radio_txbuffer[RADIO_TXBUF_SIZE];
uint8_t txlen;
uint8_t radio_rxbuffer[RADIO_RXBUF_SIZE];
uint8_t rxlen;

//radio configuration structure
radioconfig_t radioconfig;

//init radio
int8_t radio_initconfig(uint16_t chip)
{
  switch(chip)
  {
    case 1262:
    radioconfig.chip = 1262;
    radioconfig.id = 0;
    radioconfig.freq = 433125;
    radioconfig.txpower = 10;
    //modulation
    radioconfig.sf = 12;
    radioconfig.bw = 250;
    radioconfig.cr = CR_4_8;
    radioconfig.ldropt = 0;
    //packet
    radioconfig.sync = 0x24b4;
    radioconfig.prelen = 16;
    radioconfig.header = 0;
    radioconfig.paylen = 0;
    radioconfig.crc = 1;
    radioconfig.invertiq = 0;
    //params[64]; //maybe different
    //radioconfig.crc;
    return RADIO_OK;
    
    default:
    return INVALID_CHIP;
  }
}

int8_t radio_init(void)
{
  switch(chip)
  {
    case 1262:
    SX126X_reset();
    SX126X_Wakeup();
    delay_ms(10);
    SX126X_setopmode(SX126X_OPMODE_STBYRC);
    SX126X_SetRegulatorMode(true);
    //set TCXO here if needed
    SX126X_SetPacketType(SX126X_MODEM_LORA);
    SX126X_SetRfFrequency((uint32_t)(radioconfig.freq * 1000.0 * SX126X_FREQ_STEP));
    SX126X_SetBufferBaseAddress(0,0);
    SX126X_SetLoRaModParams(radioconfig.sf,radioconfig.bw,radioconfig.cr);
    SX126X_SetLoRaPacketParams(radioconfig.prelen,radioconfig.header,radioconfig.paylen,radioconfig.crc,radioconfig.invertiq);
    SX126X_writeReg(SX126X_REG_LRSYNC_H,(radioconfig.sync & 0xff00) >> 8);
    SX126X_writeReg(SX126X_REG_LRSYNC_L,radioconfig.sync & 0xff);
    SX126X_SetRx(0xffffff);
    //SX126X_setopmode(OPMODE_STBYXOSC);
    SX126X_CalibrateIR();
    SX126X_setopmode(SX126X_OPMODE_STBYXOSC);
    SX126X_writeReg(SX126X_REG_XTATRIM,radioConfig.CtuneA);
    SX126X_writeReg(SX126X_REG_XTBTRIM,radioConfig.CtuneB);
    SX126X_SetTxParams();
    SX126X_LNAboost(true);
    SX126X_SetDIO2AsRfSwitchCtrl(true);
    SX126X_SetDioIrqParams(SX126X_TXDONE_IRQMSK | SX126X_RPEDET_IRQMSK | SX126X_SYNCDET_IRQMSK | SX126X_RXDONE_IRQMSK | SX126X_CRCERR_IRQMSK, SX126X_TXDONE_IRQMSK | SX126X_RPEDET_IRQMSK | SX126X_SYNCDET_IRQMSK | SX126X_RXDONE_IRQMSK, 0, 0);
    SX126X_setopmode(SX126X_OPMODE_RX);
    return RADIO_OK;
    
    default:
    return INVALID_CHIP;
  }
}

//set RF frequency
int8_t radio_set_freq(uint32_t khz)
{
  switch(chip)
  {
    case 1262:
    return RADIO_OK;
    
    default:
    return INVALID_CHIP;
  }
}

//set tx power
int8_t radio_set_power(int8_t dbm)
{
  switch(chip)
  {
    case 1262:
    return RADIO_OK;
    
    default:
    return INVALID_CHIP;
  }
}

//set modulation parameters
int8_t radio_setmodparams(uint8_t sf,uint8_t bw,uint8_t cr,uint8_t ldropt)
{
  switch(chip)
  {
    case 1262:
    return RADIO_OK;
    
    default:
    return INVALID_CHIP;
  }
}
//set packet parameters
int8_t radio_setpktparams(uint16_t sync,uint16_t prelen,uint8_t header,uint8_t paylen,uint8_t crc,uint8_t invertiq)
{
  switch(chip)
  {
    case 1262:
    return RADIO_OK;
    
    default:
    return INVALID_CHIP;
  }
}
//send one packet
int8_t radio_sendpacket(uint8_t *buf,uint8_t length)
{
  switch(chip)
  {
    case 1262:
    SX126X_SetLoRaPacketParams(radioConfig.LoRaPreLen, radioConfig.LoRaImplHeader,length,radioConfig.LoRaCrcOn,radioConfig.LoRaInvertIQ);
    SX126X_writeBuffer(buf,length);
    SX126X_setopmode(SX126X_OPMODE_TX);
    return RADIO_OK;
    
    default:
    return INVALID_CHIP;
  }
}
//retrieve packet info
int8_t radio_getpktstatus(rxpacketstatus_t *status)
{
  switch(chip)
  {
    case 1262:
    uint8_t dummy;
    uint8_t rssipkt;
    int16_t snrpkt;
    uint8_t signalrssi;
	
    SX126X_GetLoRaPacketStatus(&dummy,&rssipkt,&snrpkt,&signalrssi);
    status->rssi_pkt = -((float)rssipkt/2);
    if(snrpkt < 128) status->snr_pkt = ((float)snrpkt)/4;
    else status->snr_pkt = (float)(snrpkt - 256)/4;
    status->signal_rssi_pkt = -((float)signalrssi/2);
    return RADIO_OK;
    
    default:
    return INVALID_CHIP;
  }
}

//receive one packet
int8_t radio_getpacket(uint8_t *buf,uint8_t *length)
{
  switch(chip)
  {
    case 1262:
    {
      uint8_t dummy;
      uint8_t rxpointer;
      SX126X_GetRxBufferStatus(&dummy,&length,&rxpointer);
      SX126X_readBuffer(rxpointer,buf,length);
    }
    return RADIO_OK;
    
    default:
    return INVALID_CHIP;
  }
}

//helpers
int8_t radio_getstats(void)
{
  switch(chip)
  {
    case 1262:
    
    return RADIO_OK;
    
    default:
    return INVALID_CHIP;
  }
}

int8_t radio_clearstats(void)
{
  
}

//irq handler
void radio_irq_handler(void)
{
  switch(chip)
  {
    case 1262:
    uint16_t irqstatus;

    //read SX126x status
    irqstatus = SX126X_GetIrqStatus();
    SX126X_ClearIrqStatus(SX126X_ALL_IRQMSK);
    if(irqstatus & SX126X_TXDONE_IRQMSK) packet_sent = true;
    if(irqstatus & SX126X_RXDONE_IRQMSK) packet_received = true;
    if(irqstatus & SX126X_CRCERR_IRQMSK) crc_error = true;
    //etc.    
    return RADIO_OK;
    
    default:
    return INVALID_CHIP;
  }
}

//working modes
int8_t radio_rx(void)
{
  switch(chip)
  {
    case 1262:
    return RADIO_OK;
    
    default:
    return INVALID_CHIP;
  }
}

int8_t radio_getrssi(float *dbm)
{
  switch(chip)
  {
    case 1262:
    return RADIO_OK;
    
    default:
    return INVALID_CHIP;
  }
}

int8_t radio_stream(uint8_t stream)
{
  switch(chip)
  {
    case 1262:
    return RADIO_OK;
    
    default:
    return INVALID_CHIP;
  }
}
