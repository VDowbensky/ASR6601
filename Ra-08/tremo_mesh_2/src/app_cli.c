#include <stdio.h>
#include "app_cli.h"
#include "bsp.h"
#include "radio_proc.h"
#include "adc.h"
#include "flash.h"
#include "gui.h"
//#include "gpsparser.h"


CommandState_t state;
char ciBuffer[256];
//uint8_t printmode = 0;
//uint8_t txmode = 0;

void printerror(int8_t error);

//General
void cli_reset(int argc, char **argv);
void cli_info(int argc, char **argv);
void cli_getver(int argc, char **argv);
void cli_getdevid(int argc, char **argv);
void cli_setdevid(int argc, char **argv);
//RF settings

void cli_getfreq(int argc, char **argv);
void cli_setfreq(int argc, char **argv);
void cli_getpower(int argc, char **argv);
void cli_setpower(int argc, char **argv);
void cli_getxotrim(int argc, char **argv);
void cli_setxotrim(int argc, char **argv);
//void cli_storectune(int argc, char **argv);

void cli_getRSSI(int argc, char **argv);

void cli_getstatus(int argc, char **argv);

//LoRa
void cli_getmodparams(int argc, char **argv);
void cli_setmodparams(int argc, char **argv);
void cli_getpacketparams(int argc, char **argv);
void cli_setpacketparams(int argc, char **argv);
void cli_getsync(int argc, char **argv);
void cli_setsync(int argc, char **argv);


//debug functions
void cli_readreg(int argc, char **argv);
void cli_writereg(int argc, char **argv);
void cli_initconfig(int argc, char **argv);
void cli_storeconfig(int argc, char **argv);

void cli_dumpregs(int argc, char **argv);


//void cli_storeconfig(int argc, char **argv);

void cli_sendburst(int argc, char **argv);
void cli_stopburst (int argc, char **argv);
void cli_txstream(int argc, char **argv);

void cli_getstats(int argc, char **argv);
void cli_clearstats(int argc, char **argv);

//device dependent
void cli_getvt(int argc, char **argv);
//special tests

void cli_setpaconfig(int argc, char **argv); //technological


CommandEntry_t commands[] =
  {
    //general
    COMMAND_ENTRY("RESET", "", cli_reset, "Device reset"),
    COMMAND_ENTRY("GET_UID", "", cli_info, "Get CPU ID"),
    COMMAND_ENTRY("GET_VERSION", "", cli_getver, "Get HW/FW version"),
    COMMAND_ENTRY("GET_DEVID", "", cli_getdevid, "Get device ID"),
    COMMAND_ENTRY("SET_DEVID", "w", cli_setdevid, "Get device ID"),
    //RF settings
    COMMAND_ENTRY("GET_FREQ", "", cli_getfreq, "Get frequency settings"),
    COMMAND_ENTRY("SET_FREQ", "w", cli_setfreq, "Set frequency settings"),

    COMMAND_ENTRY("GET_POWER", "", cli_getpower, ""),
    COMMAND_ENTRY("SET_POWER", "w", cli_setpower, ""),
    COMMAND_ENTRY("GET_XOTRIM", "", cli_getxotrim, ""),
    COMMAND_ENTRY("SET_XOTRIM", "w", cli_setxotrim, ""),
    //COMMAND_ENTRY("STORE_CTUNE", "", cli_storectune, ""),

    COMMAND_ENTRY("GET_RSSI", "", cli_getRSSI, ""),
    
    COMMAND_ENTRY("GET_STATUS", "", cli_getstatus, ""),

    //LoRa
    COMMAND_ENTRY("GET_MODPARAMS", "", cli_getmodparams, "Get LoRa modulation parameters"),
    COMMAND_ENTRY("SET_MODPARAMS", "wwww", cli_setmodparams, "Set LoRa modulation parameters"),
    COMMAND_ENTRY("GET_PACKETPARAMS", "", cli_getpacketparams, "Get LoRa packet parameters"),
    COMMAND_ENTRY("SET_PACKETPARAMS", "wwwww", cli_setpacketparams, "Set LoRa packet parameters"),
    COMMAND_ENTRY("GET_SYNC", "", cli_getsync, "Get LoRa sync word"),
    COMMAND_ENTRY("SET_SYNC", "w", cli_setsync, "Set LoRa sync word"),
                        
    //RF tests
    COMMAND_ENTRY("START_TX", "www", cli_sendburst, "Start packet burst"),
    COMMAND_ENTRY("STOP_TX", "", cli_stopburst, "Stop packet burst"),
    COMMAND_ENTRY("TX_STREAM", "v", cli_txstream, "Start/stop TX stream"),
		
		COMMAND_ENTRY("GET_STATS", "", cli_getstats, ""),
		COMMAND_ENTRY("CLR_STATS", "", cli_clearstats, ""),
    //System health - device dependent
    COMMAND_ENTRY("GET_VT", "", cli_getvt, "Get ADC data"),

    COMMAND_ENTRY("READ_REG", "w", cli_readreg, ""),
    COMMAND_ENTRY("WRITE_REG", "ww", cli_writereg, ""),
    COMMAND_ENTRY("DUMP_REGS", "ww", cli_dumpregs, ""),
                        
    COMMAND_ENTRY("INIT_CONFIG", "", cli_initconfig, ""),
		COMMAND_ENTRY("STORE_CONFIG", "", cli_storeconfig, ""),
    COMMAND_ENTRY("SET_PACONFIG", "ww", cli_setpaconfig, ""), //technological
		
		COMMAND_ENTRY(NULL, NULL, NULL, NULL)
  };


void cli_init(void)
{
  ciInitState(&state, ciBuffer, sizeof(ciBuffer), commands);
}

void cli_proc(void)
{
  char input = RETARGET_ReadChar();
  if(input != '\0' && input != 0xFF) ciProcessInput(&state, &input, 1);
}

void ciErrorCallback(char* command, CommandError_t error)
{
  if (error == CI_UNKNOWN_COMMAND) {
    printf("INVALID COMMAND\r\n");
  } else if (error == CI_MAX_ARGUMENTS) {
    printf("TOO MANY ARGUMENTS\r\n");
  } else if (error == CI_INVALID_ARGUMENTS) {
    printf("INVALID ARGUMENT\r\n");
  }
}


//cli functions implementation
//General
void cli_reset(int argc, char **argv)
{
  printf("RESET: OK\r\n");
	delay_ms(100);
	NVIC_SystemReset();
}

void cli_info(int argc, char **argv)
{
  uint64_t id;
	id = *(uint64_t*)0x4002002c;
	//id = EFC->SN_L | ((uint64_t)EFC->SN_H << 32);
	printf("GET_UID: 0x%llX\r\n", id);
}

void cli_getver(int argc, char **argv)
{
  printf("GET_VERSION: HW=%d,FW=%d.%d\r\n", HW_VERSION, FW_VERSION, FW_REVISION);
}

void cli_getdevid(int argc, char **argv)
{
  printf("GET_DEVID: 0x%04X\r\n", radioConfig.deviceID);
}

void cli_setdevid(int argc, char **argv)
{
  uint32_t id;
  id = ciGetUnsigned(argv[1]);
  radioConfig.deviceID = id;
  writeconfig();
  printf("SET_DEVID: 0x%04X\r\n", radioConfig.deviceID);
}



void cli_getfreq(int argc, char **argv)
{
  printf("GET_FREQ: %u\r\n",radioConfig.RfFreq / 1000);
}

void cli_setfreq(int argc, char **argv)
{
  radioConfig.RfFreq = ciGetUnsigned(argv[1]) * 1000;
  radio_set_freq(radioConfig.RfFreq);
  printf("SET_FREQ: %u\r\n",radioConfig.RfFreq / 1000);
	updatescreen();
}

void cli_getpower(int argc, char **argv)
{
  printf("GET_POWER: %d dBm\r\n",radioConfig.TxPowerDbm);
}

void cli_setpower(int argc, char **argv)
{
  int8_t pwr;

  pwr = ciGetSigned(argv[1]);
  if(pwr > 22) pwr = 22;
  if(pwr < -9) pwr = -9;
  radioConfig.TxPowerDbm = pwr;
  SX126X_SetTxParams();
  printf("SET_POWER: %d dBm\r\n",radioConfig.TxPowerDbm);
	updatescreen();
}


void cli_getxotrim(int argc, char **argv)
{
  uint8_t ctunea, ctuneb;

  ctunea = SX126X_readReg(SX126X_REG_XTATRIM);
  ctuneb = SX126X_readReg(SX126X_REG_XTBTRIM);
  printf("GET_CTUNE: %d\r\n", ctunea + ctuneb);
}

void cli_setxotrim(int argc, char **argv)
{
  uint8_t tune;

  tune = ciGetUnsigned(argv[1]);
  if(tune > 94) tune = 94;
  RADIO_setctune(tune);
  printf("SET_XOTRIM: %d\r\n",tune);
}

void cli_getRSSI(int argc, char **argv)
{
  float rssi;
  rssi = -((float)SX126X_GetRssiInst()/2);
  printf("GET_RSSI: %.1f dBm\r\n",rssi);
}

void cli_getstatus(int argc, char **argv)
{
  uint8_t s = SX126X_GetStatus();
  printf("GET_STATUS: 0x%02X\r\n",s);
}

//LoRa

void cli_getmodparams(int argc, char **argv)
{
  float bw_kHz;

  if(radioConfig.LoRaBw == SX126X_LORA_BW_7p8) bw_kHz = 7.8;
  else if(radioConfig.LoRaBw == SX126X_LORA_BW_10p4) bw_kHz = 10.4;
  else if(radioConfig.LoRaBw == SX126X_LORA_BW_15p6) bw_kHz = 15.6;
  else if(radioConfig.LoRaBw == SX126X_LORA_BW_20p8) bw_kHz = 20.8;
  else if(radioConfig.LoRaBw == SX126X_LORA_BW_31p3) bw_kHz = 31.3;
  else if(radioConfig.LoRaBw == SX126X_LORA_BW_41p7) bw_kHz = 41.7;
  else if(radioConfig.LoRaBw == SX126X_LORA_BW_62p5) bw_kHz = 62.5;
  else if(radioConfig.LoRaBw == SX126X_LORA_BW_125) bw_kHz = 125.0;
  else if(radioConfig.LoRaBw == SX126X_LORA_BW_250) bw_kHz = 250.0;
  else if(radioConfig.LoRaBw == SX126X_LORA_BW_500) bw_kHz = 500.0;
  else bw_kHz = -1.0;
  printf("GET_MODPARAMS:\r\nBW=%.1f\r\nSF=%d\r\n",bw_kHz,radioConfig.LoRaSf);
  printf("CR:");
    switch(radioConfig.LoRaCr)
    {
      case 0:
      printf("OFF\r\n");
      break;
      case 1:
      printf("4_5\r\n");
      break;
      case 2:
      printf("4_6\r\n");
      break;
      case 3:
      printf("4_7\r\n");
      break;
      case 4:
      printf("4_8\r\n");
      break;
      default:
      printf("INVALID\r\n");
      break;
    }
  printf("LROPT:");
  if(radioConfig.LoRaLowRateOpt == true) printf("ON\r\n");
  else printf("OFF\r\n");
}

void cli_setmodparams(int argc, char **argv)
{
    uint8_t bw_val,sf,cr,opt;
    uint16_t bw;
    bw = ciGetUnsigned(argv[1]);
    float bw_kHz;
    //BW in kHz
    if(bw <= 8) {bw_val = SX126X_LORA_BW_7p8; bw_kHz = 7.8;}
    else if(bw <= 11) {bw_val = SX126X_LORA_BW_10p4; bw_kHz = 10.4;}
    else if(bw <= 16) {bw_val = SX126X_LORA_BW_15p6; bw_kHz = 15.6;}
    else if(bw <= 21) {bw_val = SX126X_LORA_BW_20p8; bw_kHz = 20.8;}
    else if(bw <= 31) {bw_val = SX126X_LORA_BW_31p3; bw_kHz = 31.3;}
    else if(bw <= 42) {bw_val = SX126X_LORA_BW_41p7; bw_kHz = 41.7;}
    else if(bw <= 63) {bw_val = SX126X_LORA_BW_62p5; bw_kHz = 62.5;}
    else if(bw <= 125) {bw_val = SX126X_LORA_BW_125; bw_kHz = 125.0;}
    else if(bw <= 250) {bw_val = SX126X_LORA_BW_250; bw_kHz = 250.0;}
    else {bw_val = SX126X_LORA_BW_500; bw_kHz = 500.0;}

    sf = ciGetUnsigned(argv[2]);
    //check
    if(sf < 6) sf = 6;
    if(sf > 12) sf = 12;
    cr = ciGetUnsigned(argv[3]);
    //check
    if(cr > 4) cr = 4;
    opt = ciGetUnsigned(argv[4]);
    radioConfig.LoRaBw = bw_val;
    radioConfig.LoRaSf = sf;
    radioConfig.LoRaCr = cr;
    if(opt != 0) radioConfig.LoRaLowRateOpt = true;
    else radioConfig.LoRaLowRateOpt = false;
    SX126X_config();
    printf("SET_MODPARAMS:\r\nBW=%.1f\r\nSF=%d\r\n",bw_kHz,radioConfig.LoRaSf);
    printf("CR:");
    switch(radioConfig.LoRaCr)
    {
      case 0:
      printf("OFF\r\n");
      break;
      case 1:
      printf("4_5\r\n");
      break;
      case 2:
      printf("4_6\r\n");
      break;
      case 3:
      printf("4_7\r\n");
      break;
      case 4:
      printf("4_8\r\n");
      break;
      default:
      printf("INVALID\r\n");
      break;
    }
    printf("LROPT:");
    if(radioConfig.LoRaLowRateOpt == true) printf("ON\r\n");
    else printf("OFF\r\n");
}

void cli_getpacketparams(int argc, char **argv)
{
  printf("GET_PACKETPARAMS:\r\nPRE_LEN=%d\r\nPAY_LEN=%d\r\nHEADER:",radioConfig.LoRaPreLen,radioConfig.LoRaPayLen);
  if(radioConfig.LoRaImplHeader == true) printf("IMPLICIT\r\n");
  else printf("EXPLICIT\r\n");
  printf("CRC:");
  if(radioConfig.LoRaCrcOn == true) printf("ON\r\n");
  else printf("OFF\r\n");
  printf("INVERTIQ:");
  if(radioConfig.LoRaInvertIQ == true) printf("ON\r\n");
  else printf("OFF\r\n");
}

void cli_setpacketparams(int argc, char **argv)
{
    uint16_t pre;
    uint8_t pay;
    uint8_t head;
    uint8_t crc;
    uint8_t inviq;
    pre = ciGetUnsigned(argv[1]) & 0xffff;
    pay = ciGetUnsigned(argv[2]) & 0xff;
    head = ciGetUnsigned(argv[3]) & 0x1;
    crc = ciGetUnsigned(argv[4]) & 0x1;
    inviq = ciGetUnsigned(argv[5]) & 0x1;
    radioConfig.LoRaPreLen = pre;
    radioConfig.LoRaPayLen = pay;
    if(head != 0) radioConfig.LoRaImplHeader = true;
    else radioConfig.LoRaImplHeader = false;
    if(crc != 0) radioConfig.LoRaCrcOn = true;
    else radioConfig.LoRaCrcOn = false;
    if(inviq != 0) radioConfig.LoRaInvertIQ = true;
    else radioConfig.LoRaInvertIQ = false;
    SX126X_config();
    printf("SET_PACKETPARAMS:\r\nPRE_LEN=%d\r\nPAY_LEN=%d\r\nHEADER:",radioConfig.LoRaPreLen,radioConfig.LoRaPayLen);
    if(radioConfig.LoRaImplHeader == true) printf("IMPLICIT\r\n");
    else printf("EXPLICIT\r\n");
    printf("CRC:");
    if(radioConfig.LoRaCrcOn == true) printf("ON\r\n");
    else printf("OFF\r\n");
    printf("INVERTIQ:");
    if(radioConfig.LoRaInvertIQ == true) printf("ON\r\n");
    else printf("OFF\r\n");
}

void cli_getsync(int argc, char **argv)
{
  printf("GET_SYNC: 0x%04X\r\n",radioConfig.LoRaSyncWord);
}

void cli_setsync(int argc, char **argv)
{
	uint16_t sw;
	sw = ciGetUnsigned(argv[1]) & 0xffff;
	radioConfig.LoRaSyncWord = sw;
	SX126X_config();
	printf("SET_SYNC: 0x%04X\r\n",radioConfig.LoRaSyncWord);
}


//System tests
void cli_sendburst(int argc, char **argv)
{
  txpacketcount = ciGetUnsigned(argv[1]);
  if(txpacketcount == 0) txpacketcount = 1;
  inter_packet_delay = ciGetUnsigned(argv[2]);
  slave_id = ciGetUnsigned(argv[3]);
  if(inter_packet_delay < 100) inter_packet_delay = 100;
  radio_startburst();
  printf("START_TX: %d,%d\r\n",txpacketcount,inter_packet_delay);
	updatescreen();
}

void cli_stopburst (int argc, char **argv)
{
  if(master)
  {
    master = false;
    tx_request = false;
    printf("STOP_TX: OK\r\n");
  }
  else printf("STOP_TX: ERROR\r\n");
	updatescreen();
}

void cli_txstream(int argc, char **argv)
{
  uint8_t s;
  s = ciGetUnsigned(argv[1]);
  if(s > 2) s = 2;
  switch(s)
  {
    case 0:
    default:
       
      SX126X_setopmode(SX126X_OPMODE_RX); //SX126X_SetRx(radioConfig.LoRaRxTimeout);
      printf("TX_STREAM: OFF\r\n");
      txmode = 0;
      break;

    case 1:
      prevopmode = opmode;
      SX126X_setopmode(SX126X_OPMODE_TXSTREAMCW);//SX126X_SetCW();
      printf("TX_STREAM: CW\r\n");
      txmode = 1;
      break;

    case 2:
      prevopmode = opmode;
      SX126X_setopmode(SX126X_OPMODE_TXSTREAMPRE);//SX126X_SetTxInfinitePreamble();
      printf("TX_STREAM: PREAMBLE\r\n");
      txmode = 2;
      break;
  }
	updatescreen();
}



//Device dependent commands
void cli_getvt(int argc, char **argv)
{
  printf("GET_VT: T=%.3f,V=%.3f\r\n",T,Vcc);
}

void cli_readreg(int argc, char **argv)
{
  uint16_t r;
  r = ciGetUnsigned(argv[1]);
  printf("READ_REG: 0x%02X\r\n",SX126X_readReg(r));
}

void cli_writereg(int argc, char **argv)
{
  uint16_t reg;
  uint8_t val;
  reg = ciGetUnsigned(argv[1]) & 0xfff;
  val = ciGetUnsigned(argv[2]) & 0xff;
  SX126X_writeReg(reg,val);
  printf("WRITE_REG: 0x%04X,0x%02X\r\n",reg,val);
}

void cli_dumpregs(int argc, char **argv)
{
  uint16_t reg_l,reg_h;
  uint16_t i;
  reg_l = ciGetUnsigned(argv[1]);
  reg_h = ciGetUnsigned(argv[2]);
  printf("DUMP_REGS:\r\n"); 
  for(i = reg_l; i <= reg_h; i++)
  {
    printf("0x%04X,0x%02X\r\n",i,SX126X_readReg(i));
  }
}

void cli_initconfig(int argc, char **argv)
{
  int8_t retval;

  retval = SX126X_initconfigstructure();
  if(retval == 0) printf("INIT_CONFIG: OK\r\n");
  else printf("INIT_CONFIG: ERROR %d\r\n", retval);
}

void cli_storeconfig(int argc, char **argv)
{
	writeconfig();
	printf("STORE_CONFIG: OK\r\n");
}

void cli_setpaconfig(int argc, char **argv)
{
	uint8_t dutycycle;
	uint8_t hpmax;
	
	dutycycle = ciGetUnsigned(argv[1]);
	if(dutycycle > 7) dutycycle = 7;
	if(hpmax > 7) hpmax = 7;
	hpmax = ciGetUnsigned(argv[2]);
	SX126X_SetPaConfig(dutycycle,hpmax,false);
	printf("SET_PACONFIG: %d,%d\r\n",dutycycle,hpmax);
}

void cli_getstats(int argc, char **argv)
{
	radio_getrxstats();
	printf("GET_STATS: PKT=%d,FERR=%d,HERR=%d\r\n",rxstats.pkt_received,rxstats.crc_error,rxstats.header_error);
}

void cli_clearstats(int argc, char **argv)
{
	radio_clrrxstats();
	printf("CLR_STATS: OK\r\n");
}

void updatescreen(void)
{
	SSD1306_Clear(0);
	sprintf(strbuffer, "Freq:%dkHz",radioConfig.RfFreq/1000);
	GUI_ShowString(0,0,strbuffer,16,1);
	//sprintf(strbuffer, "Channel:%d",radioConfig.Channel);
	GUI_ShowString(0,16,strbuffer,16,1);
	sprintf(strbuffer, "Power:%ddBm",radioConfig.TxPowerDbm);
	GUI_ShowString(0,32,strbuffer,16,1);
	switch(opmode)
	{
    case SX126X_OPMODE_SLEEP:
		sprintf(strbuffer, "Mode: Sleep");
    break;

    case SX126X_OPMODE_STBYRC:
    sprintf(strbuffer, "Mode: StbyRC");
    break;

    case SX126X_OPMODE_STBYXOSC:
    sprintf(strbuffer, "Mode: StbyXO");
    break;

    case SX126X_OPMODE_FS:
    sprintf(strbuffer, "Mode: FS");
    break;

    case SX126X_OPMODE_TX:
			sprintf(strbuffer, "Mode: TX packet");
    break;

    case SX126X_OPMODE_RX:
    default:
    sprintf(strbuffer, "Mode: RX");
    break;

    case SX126X_OPMODE_TXSTREAMCW:
    sprintf(strbuffer, "Mode: TX CW");
    break;

    case SX126X_OPMODE_TXSTREAMPRE:
    sprintf(strbuffer, "Mode: TX Pre");
    break;
	}
	GUI_ShowString(0,48,strbuffer,16,1);
}

void printerror(int8_t error)
{
	//printf("ERROR: ");
	switch(error)
	{
		case INVALID_CHIP:
		printf("INVALID CHIP\r\n");
		break;
		case RADIO_COMM_FAIL:
		printf("NOT RESPONDING\r\n");
		break;
		case RADIO_BUSY:
		printf("BUSY\r\n");
		break;
		case FEATURE_NOT_SUPPORTED:
		printf("NOT SUPPORTED\r\n");
		break;
		case RADIO_INVALID_MODE:
		printf("INVALID MODE\r\n");
		break;
		case RADIO_INVALID_PARAMETER:
		printf("INVALID PARAMETER\r\n");
		break;
		case RADIO_TODO:
		printf("TODO\r\n");
		break;
		default:
		printf("UNKNOWN\r\n");
	}
}      


