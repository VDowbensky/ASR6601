#include "radio.h"

radioconfig_t radioconfig;

volatile uint32_t txpacketnumber = 0;
volatile uint32_t rxpacketnumber = 0;
volatile uint32_t txpacketcount = 0;
volatile uint32_t rxpacketcount = 0;
bool master = false;
volatile bool tx_request = false;
uint32_t inter_packet_delay = 100;
volatile uint32_t pkt_timecnt;

bool answer_request = false;
bool packet_received = false;
bool crc_error = false;
bool packet_sent = false;

uint8_t opmode = 0;
uint8_t prevopmode = 0;
uint8_t txmode;

uint32_t master_id;
uint32_t slave_id;
meshtastic_pkt_t txmessage;
meshtastic_pkt_t rxmessage;

uint32_t tx_ticks = 0;

//Frequency sweep
uint32_t startfreq;
uint32_t stopfreq;
uint32_t prevfreq;
uint32_t currfreq;
uint32_t sweepstep;
uint8_t sweepstream;
volatile uint32_t sweepcnt;
uint32_t sweeptime;
volatile bool sweeptx = false;
volatile bool sweeprx = false;
volatile bool sweepflag = false; //temp/

uint8_t radio_txbuffer[RADIO_TXBUF_SIZE];
uint8_t txlen;
uint8_t radio_rxbuffer[RADIO_RXBUF_SIZE];
uint8_t rx_paylen;

rxpacketstatus_t pktstatus;

float rssitr;

void process_rx_packet(void);
void radio_prepare_answer_packet(uint8_t *txlen);
void radio_prepare_tx_packet(uint8_t *txlen);
void print_crc_error(void);

int8_t radio_initconfig(void)
{
  uint8_t i;

    radioconfig.device_id = 0xffffffff;
    radioconfig.chip = 0;//not used
    radioconfig.modem = MODEM_LORA;
    radioconfig.work_mode = 0; //not used
    //radioconfig.reserved0 = 0;
    radioconfig.frequency0 = SX126X_FREQ_DEFAULT;
    radioconfig.frequency1 = 0; //not used
    radioconfig.frequency2 = 0; //not used
    radioconfig.frequency3 = 0; //not used
    radioconfig.txpower0 = SX126X_TXPOWER_DEFAULT;
    radioconfig.txpower1 = 0; //not used
    radioconfig.txpower2 = 0; //not used
    radioconfig.txpower3 = 0; //not used
    radioconfig.ramptime0 = SX126X_RAMPTIME_DEFAULT;
    radioconfig.ramptime1 = 0; //not used
    radioconfig.ramptime2 = 0; //not used
    radioconfig.ramptime3 = 0; //not used
    //calibration and user parameters - add here
		radioconfig.user_params[0] = 15;
		radioconfig.user_params[1] = 15;
    //radioconfig.user_params[96]
    //LORA parameters
    radioconfig.bw = LORA_BW_125;
    radioconfig.sf = LORA_SF_8;
    radioconfig.cr = LORA_CR_4_5;
    radioconfig.ldropt = 0; //1 - ?
    radioconfig.loraprelen = 8;
    radioconfig.implheader = 0;
    radioconfig.lorasync = 0x1424; //private network
    radioconfig.lorapaylen = 0; //???
    radioconfig.loracrc = 1;
    radioconfig.invertiq = 0;
    //radioconfig.lora_reserved[53]
    //FSK parameters
    radioconfig.br = 50000;
    radioconfig.rbw = FSK_BW_93p8;
    radioconfig.shaping = FSK_BT0p5;
    //radioconfig.reserved1[2];//8
    radioconfig.fdev = 25000;
    radioconfig.fskprelen = 32;
    radioconfig.predetlen = 4;
    radioconfig.fsksynclen = 16;
    //radioconfig.fsksync[16];//32
    for(i = 0; i < 4; i += 2) 
    {
      radioconfig.fsksync[i*2] = 0xc9;
      radioconfig.fsksync[i*2+1] = 0x6c;
    }
    for(i = 8; i < 16; i++) radioconfig.fsksync[i] = 0; //SX126x use only 8 bytes
    radioconfig.addrcomp = FSK_ADDR_FILT_DISABLED;
    radioconfig.syncmatch = 0; //not used for 1262
    radioconfig.nodeaddr = 0;
    radioconfig.braddr = 0xff;
    radioconfig.varlen = 1;
    radioconfig.fskpaylen = 0; //???
    radioconfig.crctype = FSK_CRC_2B;
    //radioconfig.reserved3;//40
    radioconfig.crcinit = 0xffff;
    radioconfig.crcpoly = 0x1021;
    radioconfig.white = 1;
    radioconfig.whiteinit = 0xaa; //???
    //radioconfig.fsk_reserved[14];//64
    return RADIO_OK;
 }

int8_t radio_config(void)
{
	currfreq = radioconfig.frequency0;
	return SX126X_config();
}

void radio_proc(void)
{
  //main RF events handler
  if(tx_request)
  {
    tx_request = false;
		radio_prepare_tx_packet(&txlen);
    led_on();
    radio_sendpacket(txlen);
  }
  if(answer_request)
  {
    answer_request = false;
		radio_prepare_answer_packet(&txlen);
    led_on();
    radio_sendpacket(txlen);
  }
  if(packet_sent)
  {
    packet_sent = false;
		led_off();
		radio_setopmode(RADIO_OPMODE_RX);
		if(master)
		{
			txpacketcount--;
			txpacketnumber++;
			if(txpacketcount == 0) 
			{
				txpacketnumber = 0;
				master = false;
				printf("TX: DONE\r\n");
			}
		}
  }
  if(packet_received)
  {
    packet_received = false;
		led_on();
		if(crc_error)
		{
			crc_error = false;
			print_crc_error();
		}
		else process_rx_packet();
		led_off();
		radio_rx();
  }
  if(sweepflag) 
	{
		sweepflag = false;
		
		if(sweeptx)
		{
			currfreq+= sweepstep;
			if(currfreq >= stopfreq) currfreq = startfreq;
			radio_fs();
			radio_set_frequency(currfreq);
			radio_txstream(txmode);
		}
		if(sweeprx)
		{
			float rssi;
			radio_getrssi(&rssi);
			if(rssi > rssitr)
			{
				//rssi_peak = rssi;
				printf("Freq=%d,RSSI=%.1f dBm\r\n",currfreq / 1000,rssi); //temporary; send to buffer instead
				//display_rssi(currfreq/1000,rssi_peak);
				//beep(2000,100);
			}
			radio_fs();
			currfreq += sweepstep;
			if(currfreq >= stopfreq) currfreq = startfreq;
			radio_set_frequency(currfreq);
			radio_rx();
		}
	}
  //etc.
}

int8_t radio_sendpacket(uint8_t len)
{
		if(radioconfig.modem == MODEM_LORA)
		{
			//configure length
			SX126X_SetLoRaPacketParams(0,radioconfig.loraprelen, radioconfig.implheader,len,radioconfig.loracrc,radioconfig.invertiq);
		}
		else if(radioconfig.modem == MODEM_FSK)
		{
			SX126X_SetFskPacketParams(0,radioconfig.fskprelen,radioconfig.predetlen,radioconfig.fsksynclen,radioconfig.addrcomp,
			radioconfig.varlen,len,radioconfig.crctype,radioconfig.white); //must be checked
		}
		else return FEATURE_NOT_SUPPORTED;
		SX126X_writeBuffer(0,0,radio_txbuffer,len);
		SX126X_setopmode(RADIO_OPMODE_TX);
		return RADIO_OK;
}

int8_t radio_sendburst(uint32_t dest, uint32_t count,uint32_t interval)
{
	slave_id = dest;
	txpacketnumber = 0;
	txpacketcount = count;
	inter_packet_delay = interval;
	master = true;
	tx_request = true;
	return RADIO_OK;
}

int8_t radio_getrssi(float *rssi)
{
    *rssi = -((float)SX126X_GetRssiInst(0)/2);
    return RADIO_OK;
}

int8_t radio_getfreqoffset(int32_t *offset)
{
    return RADIO_TODO;
}

int8_t radio_sleep(void) //sleep mode and/or power switch
{

    SX126X_SetSleep(0,0,false);
    return RADIO_OK;
}

int8_t radio_standby(uint8_t mode)
{
  if(mode > 1) return RADIO_INVALID_PARAMETER;
  else
	{
    SX126X_SetStandby(0,mode);
    if(mode == 0) opmode = RADIO_OPMODE_STBYRC;
    else opmode = RADIO_OPMODE_STBYXOSC;
    return RADIO_OK;
  }
}

int8_t radio_fs(void)
{
    SX126X_SetFs(0);
    return RADIO_OK;
}

int8_t radio_rx(void)
{
		if(radioconfig.modem == MODEM_FSK) SX126X_FskConfig(); //restore modulation parameters
		else if(radioconfig.modem == MODEM_LORA) 
		{
			SX126X_SetLoRaPacketParams(0,radioconfig.loraprelen, radioconfig.implheader,radioconfig.lorapaylen,radioconfig.loracrc,radioconfig.invertiq);//restore packet length
			//SX126X_LoRaConfig(); 
		}
		else return FEATURE_NOT_SUPPORTED;
    SX126X_SetRx(0,0xffffff); //continuous mode
		opmode= RADIO_OPMODE_RX;
    return RADIO_OK;
}

int8_t radio_txstream(uint8_t stream)
{
  if(stream == 0) 
	{
		led_off();
		return radio_rx();
	}
  else
  {
    int8_t retval; 
    
      if(stream == 1) 
			{
				SX126X_SetCW(0);
				led_on();
				prevopmode = opmode;
				opmode = RADIO_OPMODE_TXSTREAMCW;
        retval = RADIO_OK;
			}
			else if(stream == 2) 
			{
				SX126X_SetTxInfinitePreamble(0);
				led_on();
				prevopmode = opmode;
				opmode = RADIO_OPMODE_TXSTREAMPRE;
        retval = RADIO_OK;
			}
			else retval = RADIO_INVALID_PARAMETER;
			SX126X_ClearDeviceErrors(0);
      return retval;
  }
}

int8_t radio_txsweep(uint32_t start,uint32_t stop,uint32_t step,uint32_t interval,uint8_t stream)
{
	if(stop < (start+step)) return RADIO_INVALID_PARAMETER;
	if(step < MIN_FREQ_STEP) return RADIO_INVALID_PARAMETER;	
	if((interval < MIN_TX_SWEEP_TIME) || (interval > MAX_TX_SWEEP_TIME)) return RADIO_INVALID_PARAMETER;
	if(stream > 2) return RADIO_INVALID_PARAMETER;
	if(stream == 0)
	{
		txmode = 0;
		sweeptx = false;
		radio_set_frequency(prevfreq);
		currfreq = prevfreq;
		radio_set_frequency(currfreq);
		led_off();
		radio_rx();
		return RADIO_OK;
	}
	else
	{
		prevfreq = currfreq;
		startfreq = start;
		stopfreq = stop;
		sweepstep = step;
		txmode = stream;
		radio_set_frequency(startfreq);
		currfreq = startfreq;
		led_on();
		radio_txstream(stream);
		sweeptime = interval;
		sweepcnt = sweeptime;
		sweeptx = true;
		return RADIO_OK;
	}
}

int8_t radio_rxscan(uint32_t start,uint32_t stop,uint32_t step,uint32_t interval,float tr)
{
	if(stop < (start+step)) return RADIO_INVALID_PARAMETER;
	if(step < MIN_FREQ_STEP) return RADIO_INVALID_PARAMETER;
	if((interval < MIN_RX_SWEEP_TIME) || (interval > MAX_RX_SWEEP_TIME)) return RADIO_INVALID_PARAMETER;
	//SSD1306_Clear(0);
	if(tr >= 0)
	{
//		//GUI_ShowString(0,0,"IDLE       ",16,1);
		sweeprx = false;
		radio_set_frequency(prevfreq);
		currfreq = prevfreq;
	}
	else
	{
		//GUI_ShowString(0,0,"SCAN       ",16,1);
		prevfreq = currfreq;
		startfreq = start;
		stopfreq = stop;
		sweepstep = step;
		rssitr = tr;
		currfreq = startfreq;
		radio_set_frequency(currfreq);
		sweeptime = interval;
		sweepcnt = sweeptime;
		sweeprx = true;
	}
	radio_rx();
	return RADIO_OK;
}

int8_t radio_setopmode(uint8_t mode)
{
    SX126X_setopmode(mode);
    return RADIO_OK;
}

int8_t radio_set_frequency(uint32_t freq) //change frequency "on-the-fly"
{
    SX126X_SetRfFrequency(0,(uint32_t)(freq / SX126X_SYNTH_STEP));
    return RADIO_OK;
}

int8_t radio_setmodem(uint8_t modem)
{
		if(modem == MODEM_FSK)
		{
			radioconfig.modem = MODEM_FSK;
			radio_config();
			return RADIO_OK;
		}
		else if(modem == MODEM_LORA)
		{
			radioconfig.modem = MODEM_LORA;
			radio_config();
			return RADIO_OK;
		}
		else if(modem == MODEM_LR_FHSS)
		{
			return RADIO_TODO;
		}
		else return FEATURE_NOT_SUPPORTED;
}

int8_t radio_setpower(int8_t dbm)
{
		if((dbm < -9) | (dbm > 22)) return RADIO_INVALID_PARAMETER;
		else
		{
			SX126X_SetTxParams(0,dbm,SX126X_RAMPTIME_DEFAULT);
			radioconfig.txpower0 = dbm;
			return RADIO_OK;
		}
 }

int8_t radio_getxotrim(uint16_t *trim)
{
		{
			uint8_t ctunea, ctuneb;
			ctunea = SX126X_readReg(0,SX126X_REG_XTATRIM);
			ctuneb = SX126X_readReg(0,SX126X_REG_XTBTRIM);
			*trim = ctunea + ctuneb;
			return RADIO_OK;
		}
}

int8_t radio_setxotrim(uint16_t trim)
{

			uint8_t tuneA, tuneB;
			if(trim > 94) return RADIO_INVALID_PARAMETER;
			else
			{
				tuneA = trim / 2;
				tuneB = trim - tuneA;
				radioconfig.user_params[0] = tuneA;
				radioconfig.user_params[1] = tuneB;
				prevopmode = opmode;
				SX126X_setopmode(RADIO_OPMODE_STBYXOSC);
				SX126X_writeReg(0,SX126X_REG_XTATRIM,tuneA);
				SX126X_writeReg(0,SX126X_REG_XTBTRIM,tuneB);
				SX126X_setopmode(prevopmode);
				return RADIO_OK;
			}
}

int8_t radio_get_pkt_status(rxpacketstatus_t *status)
{
		if(radioconfig.modem == MODEM_FSK)
		{
			uint8_t dummy;
			uint8_t rxstatus;
			uint8_t rssisync;
			uint8_t rssiavg;
			
			SX126X_GetFskPacketStatus(0,&dummy,&rxstatus,&rssisync,&rssiavg);
			status->rx_status = rxstatus;
			status->rssi_sync = -((float)rssisync/2);
			status->rssi_avg = -((float)rssiavg/2);
			return RADIO_OK;
		}
		else if(radioconfig.modem == MODEM_LORA)
		{
			uint8_t dummy;
			uint8_t rssipkt;
			int16_t snrpkt;
			uint8_t signalrssi;
			
			SX126X_GetLoRaPacketStatus(0,&dummy,&rssipkt,&snrpkt,&signalrssi);
			status->rssi_pkt = -((float)rssipkt/2);
			if(snrpkt < 128) status->snr_pkt = ((float)snrpkt)/4;
      else status->snr_pkt = (float)(snrpkt - 256)/4;
			status->signal_rssi_pkt = -((float)signalrssi/2);
			return RADIO_OK;
		}	
		else if(radioconfig.modem == MODEM_LR_FHSS)
		{
			return RADIO_TODO;
		}
		else return FEATURE_NOT_SUPPORTED;
}

int8_t radio_get_rx_stats(rxstats_t *stats)
{
			if(radioconfig.modem == MODEM_FSK)
			{
				uint8_t dummy;
				SX126X_FskGetStats(0,&dummy, &stats->pkt_received,&stats->crc_error,&stats->len_header_error);
				return RADIO_OK;
			}
			else if(radioconfig.modem == MODEM_LORA)
			{
				uint8_t dummy;
				SX126X_LoRaGetStats(0,&dummy, &stats->pkt_received,&stats->crc_error,&stats->len_header_error);
				return RADIO_OK;
			}
			else if(radioconfig.modem == MODEM_LR_FHSS)
			{
				return RADIO_TODO;
			}
			else return FEATURE_NOT_SUPPORTED;
}

int8_t radio_clear_rx_stats(void)
{
		SX126X_ResetStats(0);
		return RADIO_OK;
}

int8_t radio_get_rx_packet(void)
{

			uint8_t rxpointer;
			uint8_t dummy;
			SX126X_GetRxBufferStatus(0,&dummy,&rx_paylen,&rxpointer);
			SX126X_readBuffer(0,rxpointer,radio_rxbuffer,rx_paylen);
			return RADIO_OK;
}

void process_rx_packet(void)
{
	//retrieve packet params
	radio_get_pkt_status(&pktstatus);
	//copy packet to buffer
	radio_get_rx_packet();
	//process buffer
	decode_meshtastic_packet();
	print_meshtastic_packet();
}

void radio_irq_proc(void)
{
			uint16_t flags = SX126X_GetIrqStatus(0);
			SX126X_ClearIrqStatus(0,SX126X_ALL_IRQMSK);
			if(flags & SX126X_TXDONE_IRQMSK) packet_sent = true;
			if(flags & SX126X_RXDONE_IRQMSK) packet_received = true;
			//if(flags & SX126X_RPEDET_IRQMSK)
			//if(flags & SX126X_SYNCDET_IRQMSK)
			//if(flags & SX126X_HEADERDET_IRQMSK)
			//if(flags & SX126X_HEADERERR_IRQMSK)
			if(flags & SX126X_CRCERR_IRQMSK) crc_error = true;
			//if(flags & SX126X_CADDONE_IRQMSK)
			//if(flags & SX126X_CADDET_IRQMSK)
			//if(flags & SX126X_TIMEOUT_IRQMSK)
			//if(flags & SX126X_LRFHSSHOP_IRQMSK)
			//if(flags & SX126X_ALL_IRQMSK)
}

void radio_prepare_answer_packet(uint8_t *txlen)
{
	
}

void radio_prepare_tx_packet(uint8_t *txlen)
{
	uint8_t i;
	txmessage.destination_id = 0xffffffff;
	txmessage.sender_id = 0x12345678;
	txmessage.packet_id = txpacketnumber;
	txmessage.relay_node = 0;
	txmessage.next_hop = 1;
	txmessage.flags = (1 << HOPSTART_POS) | (3 << HOPLIMIT_POS); //for test
	for(i = 0; i < 16; i++) txmessage.payload[i] = i;
	*txlen = 16 + i;
	//printf("Payload: ");
	memcpy((void*)radio_txbuffer,(void*)&txmessage,*txlen);
	printf("Sent: %d\r\n",txpacketnumber);
}

void print_crc_error(void)
{
	printf("FERR\r\n");
}

