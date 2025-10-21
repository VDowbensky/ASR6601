#include "radio_proc.h"
#include "flash.h"
#include "mesh_packet.h"


SX126XX_radioconfig_t radioConfig;

uint8_t radio_txbuffer[RADIO_TXBUF_SIZE];
uint8_t txlen;
uint8_t radio_rxbuffer[RADIO_RXBUF_SIZE];
uint8_t rx_paylen;

volatile uint32_t txpacketnumber = 0;
volatile uint32_t rxpacketnumber = 0;
volatile uint32_t txpacketcount = 0;
volatile uint32_t rxpacketcount = 0;
bool master = false;
uint32_t inter_packet_delay = 100;
volatile uint32_t pkt_timecnt;
volatile bool tx_request = false;
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
rxpacketstatus_t pktstatus;
rxbufferstatus_t bufstatus;

//radio events handler
void radio_proc(void)
{
  //static uint8_t phase = 0;
  
  if(tx_request)
  {
    tx_request = false;
    prepareTxPacket();
    txled_on();
    radio_sendpacket(radio_txbuffer,txlen);
  }

  if(packet_received)
  {
    packet_received = false;
		rxled_on();
		process_rx_packet();
		rxled_off();
    radio_rx();
  }

  if(packet_sent)
  {
    packet_sent = false;
    txled_off();
    radio_rx();
  }
}

void process_rx_packet(void)
{
	if(crc_error)
	{
		crc_error = false;
		printcrcerror();
	}
	else
	{
		//retrieve packet params
		radio_get_pkt_status(&pktstatus);
		//copy packet to buffer
    radio_getbufstatubufstatus(&bufstatus);
    rxpointer = bufstatus.ptr;
    rxlen = bufstatus.data_length;
    rxlen = bufstatus.data_length;
		radio_getpacket(radio_rxbuffer,rxlen);
		//process buffer
		decode_meshtastic_packet();
		print_meshtastic_packet();
	}
}

int8_t radio_get_pkt_status(rxpacketstatus_t *status)
{
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
}

int8_t radio_get_rx_packet(void)
{
	uint8_t rxpointer;
	uint8_t dummy;
	radio_getbufstatubufstatus(&bufstatus);
  rxpointer = bufstatus.ptr;
  rxlen = bufstatus.data_length;
	SX126X_readBuffer(rxpointer,radio_rxbuffer,rxlen);
	return RADIO_OK;
}


void radio_startburst(void)
{
  master = true;
  txpacketnumber = 1;
  //start TX timer here
	pkt_timecnt = 0; //to systick
  tx_request = true;
}

void prepareTxPacket(void)
{
	uint8_t i;
	
	txmessage.destination_id = slaveID;
	txmessage.sender_id = radioConfig.deviceID;
	txmessage.packet_id = txpacketnumber;
	txmessage.relay_node = 0;
	txmessage.next_hop = 1;
	txmessage.flags = (1 << HOPSTART_POS) | (3 << HOPLIMIT_POS); //for test
	for(i = 0; i < 16; i++) txmessage.payload[i] = i;
	txlen = 16 + i;
	//printf("Payload: ");
	memcpy((void*)radio_txbuffer,(void*)&txmessage,txlen);
	printf("TX: %d\r\n",txpacketnumber);
}

void printcrcerror(void)
{
  printf("FERR\r\n");
}

void RADIO_setctune(uint8_t tune)
{
  uint8_t tuneA, tuneB;

  if(tune > 94) tune = 94;
  tuneA = tune / 2;
  tuneB = tune - tuneA;
	radioConfig.CtuneA = tuneA;
	radioConfig.CtuneB = tuneB;
  prevopmode = opmode;
  SX126X_setopmode(SX126X_OPMODE_STBYXOSC);
  SX126X_writeReg(SX126X_REG_XTATRIM,tuneA);
  SX126X_writeReg(SX126X_REG_XTBTRIM,tuneB);
  SX126X_setopmode(prevopmode);
}

void LORA_IRQHandler(void)
{
  uint16_t irqstatus;

  //read SX126x status
  irqstatus = SX126X_GetIrqStatus();
  SX126X_ClearIrqStatus(SX126X_ALL_IRQMSK);

  if(irqstatus & SX126X_TXDONE_IRQMSK) packet_sent = true;
  if(irqstatus & SX126X_RXDONE_IRQMSK) packet_received = true;
  if(irqstatus & SX126X_CRCERR_IRQMSK) crc_error = true;
}

void radio_set_freq(uint32_t freq)
{
	SX126X_SetRfFrequency((uint32_t)(freq / SX126X_SYNTH_STEP));
}

void radio_rx(void)
{
	SX126X_SetLoRaPacketParams(radioConfig.LoRaPreLen, radioConfig.LoRaImplHeader,radioConfig.LoRaPayLen,radioConfig.LoRaCrcOn,radioConfig.LoRaInvertIQ);
	SX126X_setopmode(SX126X_OPMODE_RX);
}






