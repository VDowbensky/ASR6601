#include "radio_proc.h"
#include "flash.h"

void processHardEvents(void);
void processTxEvents(void);
void processRxEvents(void);

uint32_t tx_ticks = 0;

//uint8_t HardEvents;
//uint8_t RxEvents;
//uint8_t TxEvents;

uint8_t RADIO_txBuffer[RADIO_BUFFER_SIZE]; // = TX_PACKET_TEMPLATE;
uint8_t RADIO_rxBuffer[RADIO_BUFFER_SIZE];

SX126XX_radioconfig_t radioConfig;

//uint8_t iv[16]  = {0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff};

//const uint16_t deviceID = 0x7f0f;

volatile uint16_t txpacketnumber = 0;
volatile uint16_t rxpacketnumber = 0;
volatile uint16_t txpacketcount = 0;
volatile uint16_t rxpacketcount = 0;
bool master = false;
bool tx_needed = false;
uint16_t inter_packet_delay = 100;
bool answer_needed = false;
bool pre_rcv = false;
bool sync_rcv = false;
bool packet_received = false;
bool crc_error = false;
bool packet_sent = false;

rfpacket_t txpacket, rxpacket;
float RssiPkt;
float SnrPkt;
float SignalRssiPkt;

int16_t Channel;

uint16_t masterID;
uint16_t slaveID;

int16_t Rssi;

uint8_t minchan;
uint8_t maxchan;
volatile bool sweeptx = false;
volatile bool sweeprx = false;
uint32_t sweepdelay;

bool sweepflag; //temp/

//struct AES_ctx aes_ctx;

void radio_init(void)
{
	lorac_init();
	readconfig();
	if(radioConfig.MagicNumber != MAGICNUMBER)SX126X_initconfigstructure();

  if(SX126X_config() == 0) printf("Radio Init OK.\r\n");
  else printf("Radio Init Error.\r\n");
//  if(radioConfig.AesEnabled == true)
//  {
//    AES_init_ctx_iv(&aes_ctx, (uint8_t *)&radioConfig.AesKey,(uint8_t *)&radioConfig.AesIv);
//    printf("AES enabled.\r\n");
//  }
//  else printf("AES disabled.\r\n");
}

//radio events handler
void radio_proc(void)
{
  static uint8_t phase = 0;
  
  if(tx_needed)
  {
    tx_needed = false;
    prepareTxPacket();
    led_on();
    SX126X_setopmode(OPMODE_TX);
  }

  if(answer_needed)
  {
    answer_needed = false;
    prepareAnswerPacket();
    led_on();
    SX126X_setopmode(OPMODE_TX);
  }

  if(packet_received)
  {
    packet_received = false;
    led_on();
    //LoRa
    if(radioConfig.modem == MODEM_LORA)
    {
      uint8_t pktstatus,rssipkt,signalrssipkt;
      int16_t snrpkt;
      
      SX126X_GetLoRaPacketStatus(&pktstatus,&rssipkt,&snrpkt,&signalrssipkt);
      RssiPkt = -((float)rssipkt/2);
      if(snrpkt < 128) SnrPkt = ((float)snrpkt)/4;
      else SnrPkt = (float)(snrpkt - 256)/4;
      //( status[1] < 128 ) ? ( pktStatus->Params.LoRa.SnrPkt = status[1] >> 2 ) : ( pktStatus->Params.LoRa.SnrPkt = ( ( status[1] - 256 ) >> 2 ) );
      SignalRssiPkt = -((float)signalrssipkt/2);
    }
    else
    {
      uint8_t status,rxstatus,rssisync,rssiavg;
      SX126X_GetFskPacketStatus(&status,&rxstatus,&rssisync,&rssiavg);
      RssiPkt = -((float)rssisync/2);
      SignalRssiPkt = -((float)rssiavg/2);
      SnrPkt = 0;
    }
    //retrieve packet from transceiver to RADIO_rxBuffer
    SX126X_readbuffer(0x80, 32, RADIO_rxBuffer);
    if(crc_error)
    {
      crc_error = false;
      printcrcerror();
    }
    else
    {
//      if(radioConfig.AesEnabled == true)
//      {
        //struct AES_ctx aes_ctx;
        //AES_init_ctx(&aes_ctx, (uint8_t *)&radioConfig.AesKey);
//        AES_CBC_decrypt_buffer(&aes_ctx,RADIO_rxBuffer,32);
        //AES_ECB_decrypt_buffer(aes_ctx,RADIO_rxBuffer,2);
        //AES_CTR_xcrypt_buffer(&aes_ctx,RADIO_rxBuffer,32);
#if DEBUG_MODE
        printf("RX buffer decrypted.\r\n");
#endif
//      }
      memcpy((void*)&rxpacket, (void*) RADIO_rxBuffer, 32);
      printRxPacket();
      rxpacketnumber = rxpacket.packetnumber;
      if(!master && (rxpacket.slaveid == radioConfig.deviceID)) answer_needed = true;
    }
    led_off();
    //execute commands if needed
    SX126X_setopmode(OPMODE_RX);
  }

  if(packet_sent)
  {
    packet_sent = false;
    SX126X_setopmode(OPMODE_RX);
    led_off();
  }
  //sweep RF
  if(sweepflag)
  {
    sweepflag = false;
    if(sweeptx)
    {
      if(phase == 0)
      {
        Channel++;
        if(Channel > maxchan) 
        {
          Channel = maxchan;
          phase = 1;
        }
    }
    else
    {
      Channel--;
      if(Channel < minchan)
      {
        Channel = minchan;
        phase = 0;
      }
    }
    SX126X_SetChannel();
   }
  if(sweeprx)
  {
    float rssi;
    rssi = -((float)SX126X_GetRssiInst()/2);
    printf("CH=%d,RSSI=%.1f dBm\r\n",Channel,rssi);
    Channel++;
    if(Channel > maxchan) Channel = minchan;
    SX126X_SetChannel();
   }
  }
}

void radio_startburst(void)
{
  master = true;
  txpacketnumber = 1;
  //start TX timer here
	tx_ticks = 0; //to systick
  tx_needed = true;
}

void prepareTxPacket(void)
{
  txpacket.packetnumber = txpacketnumber;
  txpacket.params[0] = 'T';
  preparepacketcommon();
}

void prepareAnswerPacket(void)
{
  txpacket.packetnumber = rxpacketnumber;
  txpacket.params[0] = 'R';
  preparepacketcommon();
}

void preparepacketcommon(void)
{
  uint8_t i;

  txpacket.masterid = radioConfig.deviceID;
  txpacket.slaveid = slaveID;
//  txpacket.latitude = GPSdata.lat;
//  txpacket.longitude = GPSdata.lon;
//  txpacket.status = GPSdata.status;
//  txpacket.sat = GPSdata.sat;
//  txpacket.hour = GPSdata.hour;
//  txpacket.min = GPSdata.min;
//  txpacket.sec = GPSdata.sec;
//  txpacket.alt = GPSdata.alt;
//  calcVccTemp();
//  txpacket.temp = (int8_t)Temp;

  for(i = 1; i < 8; i++) txpacket.params[i] = 2;
  memcpy((void*)RADIO_txBuffer, (void*)&txpacket, 32);
  //fill SX126x TX buffer
//  if(radioConfig.AesEnabled == true)
//  {
    //struct AES_ctx aes_ctx;
    //AES_init_ctx(&aes_ctx, (uint8_t *)&radioConfig.AesKey);
//    AES_CBC_encrypt_buffer(&aes_ctx,RADIO_txBuffer,32);
    //AES_ECB_encrypt_buffer(aes_ctx,RADIO_txBuffer,2);
    //AES_CTR_xcrypt_buffer(&aes_ctx,RADIO_txBuffer,32);
#if DEBUG_MODE
    printf("TX buffer encrypted.\r\n");
#endif
//  }
  SX126X_writebuffer(0,32,RADIO_txBuffer);
}

void printRxPacket(void)
{
  printf("%d,0x%04X,%c,%d:%d:%d,%d,%d,%.4f,%.4f,%.1f,%d,%.1f,%.1f,%.1f\r\n",
         rxpacket.packetnumber,rxpacket.masterid,rxpacket.params[0],rxpacket.hour,
         rxpacket.min,rxpacket.sec,rxpacket.status,rxpacket.sat,rxpacket.latitude,
         rxpacket.longitude,rxpacket.alt,rxpacket.temp,RssiPkt,SnrPkt,SignalRssiPkt);
}

void printcrcerror(void)
{
  printf("FERR\r\n");
}

void LORA_IRQHandler(void)
{
  uint16_t irqstatus;

//  EXTI_ClearITPendingBit(RADIO_DIO1_EXTI_IT_PIN);
  //read SX126x status
  irqstatus = SX126X_GetIrqStatus();
  SX126X_ClearIrqStatus(ALL_IRQMSK);

  if(irqstatus & TXDONE_IRQMSK) packet_sent = true;
  if(irqstatus & RXDONE_IRQMSK) packet_received = true;
  if(irqstatus & CRCERR_IRQMSK) crc_error = true;
  //set flags
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
  SX126X_setopmode(OPMODE_STBYXOSC);
  SX126X_writereg(REG_XTATRIM,tuneA);
  SX126X_writereg(REG_XTBTRIM,tuneB);
  SX126X_setopmode(prevopmode);
}

void RADIO_ctunecorrection(void)
{
  int16_t tune;
  float deltaCtune;
  float deltaT;

//  calcVccTemp();
//  deltaT = Temp - radioConfig.CalTemp;
//  tune = SX126X_readreg(REG_XTATRIM) + SX126X_readreg(REG_XTBTRIM);
//  deltaCtune = K_CTUNE3 * deltaT * deltaT * deltaT + K_CTUNE2 * deltaT * deltaT + K_CTUNE1 * deltaT;
//  tune = tune + (int16_t)deltaCtune;
//  if(tune < 0) tune = 0;
//  RADIO_setctune((uint8_t)tune);
}




