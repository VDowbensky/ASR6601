#include <stdio.h>
#include "app_cli.h"
#include "bsp.h"
#include "radio_proc.h"
#include "adc.h"
#include "flash.h"
//#include "gpsparser.h"


CommandState_t state;
char ciBuffer[256];
//uint8_t printmode = 0;
uint8_t txmode = 0;

//General
void cli_reset(int argc, char **argv);
void cli_info(int argc, char **argv);
void cli_getver(int argc, char **argv);
void cli_getdevid(int argc, char **argv);
void cli_setdevid(int argc, char **argv);
void cli_aesenable(int argc, char **argv);
void cli_getaeskey(int argc, char **argv);
void cli_setaeskey(int argc, char **argv);
void cli_setaesiv(int argc, char **argv);
//RF settings
void cli_getmodem(int argc, char **argv);
void cli_setmodem(int argc, char **argv);
void cli_getopmode(int argc, char **argv);
void cli_setopmode(int argc, char **argv);
void cli_getfreq(int argc, char **argv);
void cli_setfreq(int argc, char **argv);
void cli_getchannel(int argc, char **argv);
void cli_setchannel(int argc, char **argv);
void cli_storechannel(int argc, char **argv);
void cli_getpower(int argc, char **argv);
void cli_setpower(int argc, char **argv);
void cli_getramptime(int argc, char **argv);
void cli_setramptime(int argc, char **argv);
void cli_getxoparams(int argc, char **argv);
void cli_setxoparams(int argc, char **argv);
void cli_getctune(int argc, char **argv);
void cli_setctune(int argc, char **argv);
//void cli_storectune(int argc, char **argv);

void cli_getRSSI(int argc, char **argv);
void cli_gettimeouts(int argc, char **argv);
void cli_settimeouts(int argc, char **argv);
void cli_getsleepparams(int argc, char **argv);
void cli_setsleepparams(int argc, char **argv);

void cli_getstatus(int argc, char **argv);

//LoRa
void cli_lr_getmodparams(int argc, char **argv);
void cli_lr_setmodparams(int argc, char **argv);
void cli_lr_getpacketparams(int argc, char **argv);
void cli_lr_setpacketparams(int argc, char **argv);
void cli_lr_getsync(int argc, char **argv);
void cli_lr_setsync(int argc, char **argv);
void cli_lr_getsymboltimeout(int argc, char **argv);
void cli_lr_setsymboltimeout(int argc, char **argv);


//FSK
//baudrate,pulse shape,RXBW,deviation
void cli_fs_getmodparams(int argc, char **argv);
void cli_fs_setmodparams(int argc, char **argv);
//prelen, predetlen,synclen,addr.filt,varlen,paylen,crctype,whitening
void cli_fs_getpacketparams(int argc, char **argv);
void cli_fs_setpacketparams(int argc, char **argv);
//node and broadcast addr.
void cli_fs_getaddr(int argc, char **argv);
void cli_fs_setaddr(int argc, char **argv);
//sync word(s)
void cli_fs_getsync(int argc, char **argv);
void cli_fs_setsync(int argc, char **argv);
//CRC init, CRC poly, whitening init
void cli_fs_getcrcwhite(int argc, char **argv);
void cli_fs_setcrcwhite(int argc, char **argv);


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
void cli_startrxcount(int argc, char **argv);
void cli_stoprxcount(int argc, char **argv);
void cli_getrxcount(int argc, char **argv);
void cli_getautosendparams(int argc, char **argv);
void cli_setautosendparams(int argc, char **argv);
//void cli_startber(int argc, char **argv);
//void cli_stopber(int argc, char **argv);
//void cli_getber(int argc, char **argv);
void cli_getgpsdata(int argc, char **argv);
//device dependent
void cli_getvt(int argc, char **argv);
void cli_setem(int argc, char **argv);
//special tests
void cli_sweeptx(int argc, char **argv);
void cli_sweeprx(int argc, char **argv);
//device specific
void cli_setem(int argc, char **argv);

void cli_setpaconfig(int argc, char **argv); //technological


CommandEntry_t commands[] =
  {
    //general
    COMMAND_ENTRY("RESET", "", cli_reset, "Device reset"),
    COMMAND_ENTRY("GET_UID", "", cli_info, "Get CPU ID"),
    COMMAND_ENTRY("GET_VERSION", "", cli_getver, "Get HW/FW version"),
    COMMAND_ENTRY("GET_DEVID", "", cli_getdevid, "Get device ID"),
    COMMAND_ENTRY("SET_DEVID", "w", cli_setdevid, "Get device ID"),
    COMMAND_ENTRY("AES_EN", "w", cli_aesenable, "Enable/disable AES"),
    COMMAND_ENTRY("GET_AESSTATUS", "", cli_getaeskey, "Get AES key"),
    COMMAND_ENTRY("SET_AESKEY", "wwwwwwww", cli_setaeskey, "Set AES key"),
    COMMAND_ENTRY("SET_AESIV", "wwww", cli_setaesiv, "Set AES init vector"),
    //RF settings
    COMMAND_ENTRY("GET_MODEM", "", cli_getmodem, ""),
    COMMAND_ENTRY("SET_MODEM", "v", cli_setmodem, ""),
    COMMAND_ENTRY("GET_OPMODE", "", cli_getopmode, "Get operation mode"),
    COMMAND_ENTRY("SET_OPMODE", "v", cli_setopmode, "Set operation mode"),
    COMMAND_ENTRY("GET_FREQ", "", cli_getfreq, "Get frequency settings"),
    COMMAND_ENTRY("SET_FREQ", "ww", cli_setfreq, "Set frequency settings"),
    COMMAND_ENTRY("GET_CHANNEL", "", cli_getchannel, "Get channel"),
    COMMAND_ENTRY("SET_CHANNEL", "w", cli_setchannel, "Set channel"),
    COMMAND_ENTRY("STORE_CHANNEL", "", cli_storechannel, "Store channel"),
    COMMAND_ENTRY("GET_POWER", "", cli_getpower, ""),
    COMMAND_ENTRY("SET_POWER", "w", cli_setpower, ""),
    COMMAND_ENTRY("GET_RAMPTIME", "", cli_getramptime, ""),
    COMMAND_ENTRY("SET_RAMPTIME", "w", cli_setramptime, ""),
		
    COMMAND_ENTRY("GET_XOPARAMS", "", cli_getxoparams, ""),
    COMMAND_ENTRY("SET_XOPARAMS", "ww", cli_setxoparams, ""),
    COMMAND_ENTRY("GET_CTUNE", "", cli_getctune, ""),
    COMMAND_ENTRY("SET_CTUNE", "w", cli_setctune, ""),
    //COMMAND_ENTRY("STORE_CTUNE", "", cli_storectune, ""),

    COMMAND_ENTRY("GET_RSSI", "", cli_getRSSI, ""),
    COMMAND_ENTRY("GET_TIMEOUTS", "", cli_gettimeouts, ""),
    COMMAND_ENTRY("SET_TIMEOUTS", "ww", cli_settimeouts, ""),
    COMMAND_ENTRY("GET_SLEEPPARAMS", "", cli_getsleepparams, ""),
    COMMAND_ENTRY("SET_SLEEPPARAMS", "ww", cli_setsleepparams, ""),
    
    COMMAND_ENTRY("GET_STATUS", "", cli_getstatus, ""),

    //LoRa
    COMMAND_ENTRY("LR_GET_MODPARAMS", "", cli_lr_getmodparams, "Get LoRa modulation parameters"),
    COMMAND_ENTRY("LR_SET_MODPARAMS", "wwww", cli_lr_setmodparams, "Set LoRa modulation parameters"),
    COMMAND_ENTRY("LR_GET_PACKETPARAMS", "", cli_lr_getpacketparams, "Get LoRa packet parameters"),
    COMMAND_ENTRY("LR_SET_PACKETPARAMS", "wwwww", cli_lr_setpacketparams, "Set LoRa packet parameters"),
    COMMAND_ENTRY("LR_GET_SYNC", "", cli_lr_getsync, "Get LoRa sync word"),
    COMMAND_ENTRY("LR_SET_SYNC", "w", cli_lr_setsync, "Set LoRa sync word"),
    COMMAND_ENTRY("LR_GET_SYMTO", "", cli_lr_getsymboltimeout, ""),
    COMMAND_ENTRY("LR_SET_SYMTO", "w", cli_lr_setsymboltimeout, ""),
                        
    //FSK
    COMMAND_ENTRY("FS_GET_MODPARAMS", "", cli_fs_getmodparams, "Get FSK modulation parameters"),
    COMMAND_ENTRY("FS_SET_MODPARAMS", "wwww", cli_fs_setmodparams, "Set FSK modulation parameters"),
    COMMAND_ENTRY("FS_GET_PACKETPARAMS", "", cli_fs_getpacketparams, "Get FSK packet parameters"),
    COMMAND_ENTRY("FS_SET_PACKETPARAMS", "wwwwwwww", cli_fs_setpacketparams, "Set FSK packet parameters"),
    COMMAND_ENTRY("FS_GET_ADDR", "", cli_fs_getaddr, "Get FSK node and broadcast addresses"),
    COMMAND_ENTRY("FS_SET_ADDR", "ww", cli_fs_setaddr, "Set FSK node and broadcast addresses"),
    COMMAND_ENTRY("FS_GET_SYNC", "", cli_fs_getsync, "Get FSK sync word"),
    COMMAND_ENTRY("FS_SET_SYNC", "wwwwwwww", cli_fs_setsync, "Set FSK sync word"),
    COMMAND_ENTRY("FS_GET_CRCWH", "", cli_fs_getcrcwhite, "Get FSK CRC/WH parameters"),
    COMMAND_ENTRY("FS_SET_CRCWH", "www", cli_fs_setcrcwhite, "Set FSK CRC/WH parameters"),

    //RF tests
    COMMAND_ENTRY("SEND_PACKET", "www", cli_sendburst, "Start packet burst"),
    COMMAND_ENTRY("STOP_TX", "", cli_stopburst, "Stop packet burst"),
    COMMAND_ENTRY("GET_AUTOSEND", "", cli_getautosendparams, "Get default inter-packet delay"),
    COMMAND_ENTRY("SET_AUTOSEND", "ww", cli_setautosendparams, "Set default inter-packet delay"),
    COMMAND_ENTRY("TX_STREAM", "v", cli_txstream, "Start/stop TX stream"),
    COMMAND_ENTRY("START_RXCOUNT", "", cli_startrxcount, "Start RX packet count"),
    COMMAND_ENTRY("STOP_RXCOUNT", "", cli_stoprxcount, "Stop RX packet count"),
    COMMAND_ENTRY("GET_RXCOUNT", "", cli_getrxcount, "Get RX packet count"),
                        
    COMMAND_ENTRY("GET_GNSS", "", cli_getgpsdata, "Get GNSS data"),
    //System health - device dependent
    COMMAND_ENTRY("GET_VT", "", cli_getvt, "Get ADC data"),
    COMMAND_ENTRY("SET_EM", "v", cli_setem, "Set energy mode"),

    COMMAND_ENTRY("READ_REG", "w", cli_readreg, ""),
    COMMAND_ENTRY("WRITE_REG", "ww", cli_writereg, ""),
    COMMAND_ENTRY("DUMP_REGS", "ww", cli_dumpregs, ""),
                        
    COMMAND_ENTRY("SWEEP_TX", "wwww", cli_sweeptx, "TX sweep"),
    COMMAND_ENTRY("SWEEP_RX", "www", cli_sweeprx, "RX scan"),

    COMMAND_ENTRY("INIT_CONFIG", "", cli_initconfig, ""),
		COMMAND_ENTRY("STORE_CONFIG", "", cli_storeconfig, ""),
    COMMAND_ENTRY("SET_EM", "w", cli_setem, ""),
		
		COMMAND_ENTRY("SET_PACONFIG", "ww", cli_setpaconfig, ""), //technological
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
  uint16_t id;
  id = ciGetUnsigned(argv[1]);
  radioConfig.deviceID = id;
  writeconfig();
  printf("SET_DEVID: 0x%04X\r\n", radioConfig.deviceID);
}

void cli_aesenable(int argc, char **argv)
{
  uint8_t aes;
  aes = ciGetUnsigned(argv[1]);
  //FLASH_Unlock(FLASH_MemType_Data);
  if(aes == 0) radioConfig.AesEnabled = false;
  else radioConfig.AesEnabled = true;
  //FLASH_Lock(FLASH_MemType_Data);
  if(radioConfig.AesEnabled == true) printf("AES: ON\r\n");
  else printf("AES_EN: OFF\r\n");
	//printf("AES_EN: TODO\r\n");
}

void cli_getaeskey(int argc, char **argv)
{
    uint8_t i;
    //add password request?
    printf("GET_AESSTATUS: ");
    if(radioConfig.AesEnabled == true) printf("AES:ON, ");
    else printf("AES:OFF, ");
    printf("KEY=");
    for (i = 0; i < 32; i++) printf("0x%02X,",radioConfig.AesKey[i]);
    printf("IV=");
    for (i = 0; i < 15; i++) printf("0x%02X,",radioConfig.AesIv[i]);
    printf("0x%02X\r\n",radioConfig.AesIv[15]);
}

void cli_setaeskey(int argc, char **argv)
{
  uint32_t tmp[8];
  uint8_t i;

  for(i = 0; i < 8; i++) tmp[i] = ciGetUnsigned(argv[i+1]);
  for(i = 0; i < 8; i++)
  {
    radioConfig.AesKey[i*4] = (tmp[i] >> 24) & 0xff;
    radioConfig.AesKey[i*4 + 1] = (tmp[i] >> 16) & 0xff;
    radioConfig.AesKey[i*4 + 2] = (tmp[i] >> 8) & 0xff;
    radioConfig.AesKey[i*4 + 3] = tmp[i] & 0x000000ff;
  }
  printf("SET_AESKEY: OK\r\n");
}

void cli_setaesiv(int argc, char **argv)
{
  uint32_t tmp[4];
  uint8_t i;

  for(i = 0; i < 4; i++) tmp[i] = ciGetUnsigned(argv[i+1]);
  for(i = 0; i < 4; i++)
  {
    radioConfig.AesIv[i*4] = (tmp[i] >> 24) & 0xff;
    radioConfig.AesIv[i*4 + 1] = (tmp[i] >> 16) & 0xff;
    radioConfig.AesIv[i*4 + 2] = (tmp[i] >> 8) & 0xff;
    radioConfig.AesIv[i*4 + 3] = tmp[i] & 0x000000ff;
  }
  printf("SET_AESIV: OK\r\n");
}

//RF settings
void cli_getmodem(int argc, char **argv)
{
  if(radioConfig.modem == MODEM_LORA) printf("GET_MODEM: LORA\r\n");
  else printf("GET_MODEM: FSK\r\n");
}

void cli_setmodem(int argc, char **argv)
{
  //0 - FSK, 1 - LoRa
  uint8_t m = ciGetUnsigned(argv[1]);
  if((m == MODEM_FSK) || (m == MODEM_LORA))
  {
    radioConfig.modem = m;
    SX126X_config();
    if(m == MODEM_FSK) printf("SET_MODEM: FSK\r\n");
    else printf("SET_MODEM: LORA\r\n");
  }
  else printf("SET_MODEM: ERROR\r\n");
}

void cli_getopmode(int argc, char **argv)
{
  switch(opmode)
  {
    case OPMODE_SLEEP:
    printf("GET_OPMODE: SLEEP\r\n");
    break;

    case OPMODE_STBYRC:
    printf("GET_OPMODE: STANDBY RC\r\n");
    break;

    case OPMODE_STBYXOSC:
    printf("GET_OPMODE: STANDBY XOSC\r\n");
    break;

    case OPMODE_FS:
    printf("GET_OPMODE: SYNTH\r\n");
    break;

    case OPMODE_TX:
    printf("GET_OPMODE: TX\r\n");
    break;

    case OPMODE_RX:
    default:
    printf("GET_OPMODE: RX\r\n");
    break;

    case OPMODE_TXSTREAMCW:
    printf("GET_OPMODE: TX CW\r\n");
    break;

    case OPMODE_TXSTREAMPRE:
    printf("GET_OPMODE: TX PREAMBLE\r\n");
    break;
  }
}

void cli_setopmode(int argc, char **argv)
{
  uint8_t mode;
  mode = ciGetUnsigned(argv[1]);
  prevopmode = opmode;
  SX126X_setopmode(mode);
  switch(opmode)
  {
    case OPMODE_SLEEP:
    SX126X_setopmode(OPMODE_SLEEP);
    printf("SET_OPMODE: SLEEP\r\n");
    break;

    case OPMODE_STBYRC:
    SX126X_setopmode(OPMODE_STBYRC);
    printf("SET_OPMODE: STANDBY RC\r\n");
    break;

    case OPMODE_STBYXOSC:
    SX126X_setopmode(OPMODE_STBYXOSC);
    printf("SET_OPMODE: STANDBY XOSC\r\n");
    break;

    case OPMODE_FS:
    SX126X_setopmode(OPMODE_FS);
    printf("SET_OPMODE: SYNTH\r\n");
    break;

    case OPMODE_TX:
    SX126X_setopmode(OPMODE_TX);
    printf("SET_OPMODE: TX\r\n");
    break;

    case OPMODE_RX:
    default:
    SX126X_setopmode(OPMODE_RX);
    printf("SET_OPMODE: RX\r\n");
    break;

    case OPMODE_TXSTREAMCW:
    SX126X_setopmode(OPMODE_TXSTREAMCW);
    printf("SET_OPMODE: TX CW\r\n");
    break;

    case OPMODE_TXSTREAMPRE:
    SX126X_setopmode(OPMODE_TXSTREAMPRE);
    printf("SET_OPMODE: TX PREAMBLE\r\n");
    break;
  }
}

void cli_getfreq(int argc, char **argv)
{
  printf("GET_FREQ: %u,%u\r\n",radioConfig.RfFreq,radioConfig.ChannelStep);
}

void cli_setfreq(int argc, char **argv)
{
  radioConfig.RfFreq = ciGetUnsigned(argv[1]);
  radioConfig.ChannelStep = ciGetUnsigned(argv[2]);
  SX126X_SetChannel();
  printf("SET_FREQ: %u,%u\r\n",radioConfig.RfFreq,radioConfig.ChannelStep);
}

void cli_getchannel(int argc, char **argv)
{
  printf("GET_CHANNEL: %d\r\n",Channel);
}

void cli_setchannel(int argc, char **argv)
{
  Channel = ciGetSigned(argv[1]);
  SX126X_SetChannel();
  printf("SET_CHANNEL: %d\r\n",Channel);
}

void cli_storechannel(int argc, char **argv)
{
  radioConfig.Channel = Channel;
  writeconfig();
  printf("STORE_CHANNEL: %d\r\n",Channel);
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
}

void cli_getramptime(int argc, char **argv)
{
  printf("GET_RAMPTIME: %d us\r\n",radioConfig.RampTimeUs);
}

void cli_setramptime(int argc, char **argv)
{
  uint32_t ramp_time = ciGetUnsigned(argv[1]);
	if(ramp_time < 20) ramp_time = 10;
	else if(ramp_time < 40) ramp_time = 20;
	else if(ramp_time < 80) ramp_time = 40;
	else if(ramp_time < 200) ramp_time = 80;
	else if(ramp_time < 800) ramp_time = 200;
	else if(ramp_time < 1700) ramp_time = 800;
	else if(ramp_time < 3400) ramp_time = 1700;
	else ramp_time = 3400;
  radioConfig.RampTimeUs = ramp_time;
  SX126X_SetTxParams();
  printf("SET_RAMPTIME: %d us\r\n",radioConfig.RampTimeUs);
}

void cli_getxoparams(int argc, char **argv)
{
  printf("GET_XOPARAMS: %d,%d\r\n",radioConfig.XoTempFactor,radioConfig.CalTemp);
}

void cli_setxoparams(int argc, char **argv)
{
  int8_t tf,ct;

  tf = ciGetSigned(argv[1]);
  //check limits
  ct = ciGetSigned(argv[2]);
  //check limits
  radioConfig.XoTempFactor = tf;
  radioConfig.CalTemp = ct;
  printf("SET_XOPARAMS: %d,%d\r\n",radioConfig.XoTempFactor,radioConfig.CalTemp);
}

void cli_getctune(int argc, char **argv)
{
  uint8_t ctunea, ctuneb;

  ctunea = SX126X_readreg(REG_XTATRIM);
  ctuneb = SX126X_readreg(REG_XTBTRIM);
  printf("GET_CTUNE: %d\r\n", ctunea + ctuneb);
}

void cli_setctune(int argc, char **argv)
{
  uint8_t tune;

  tune = ciGetUnsigned(argv[1]);
  if(tune > 94) tune = 94;
  RADIO_setctune(tune);
  printf("SET_CTUNE: %d\r\n",tune);
}
/*
void cli_storectune(int argc, char **argv)
{
  uint8_t ctunea, ctuneb;

  ctunea = SX126X_readreg(REG_XTATRIM);
  ctuneb = SX126X_readreg(REG_XTBTRIM);
  radioConfig.CtuneA = ctunea;
  radioConfig.CtuneB = ctuneb;
  writeconfig();
  printf("STORE_CTUNE: %d\r\n", ctunea + ctuneb);
}
*/

void cli_getRSSI(int argc, char **argv)
{
  float rssi;
  rssi = -((float)SX126X_GetRssiInst()/2);
  printf("GET_RSSI: %.1f dBm\r\n",rssi);
}

void cli_gettimeouts(int argc, char **argv)
{
  if(radioConfig.modem == MODEM_LORA) printf("GET_TIMEOUTS: %d,%d\r\n",radioConfig.LoRaTxTimeout,radioConfig.LoRaRxTimeout);
  else printf("GET_TIMEOUTS: %d,%d\r\n",radioConfig.FskTxTimeout,radioConfig.FskRxTimeout);
}

void cli_settimeouts(int argc, char **argv)
{
  uint32_t txto,rxto;
  txto = ciGetUnsigned(argv[1]);
  rxto = ciGetUnsigned(argv[2]);
  if(radioConfig.modem == MODEM_LORA)
  {
    radioConfig.LoRaTxTimeout = txto;
    radioConfig.LoRaRxTimeout = rxto;
  }
  else
  {
    radioConfig.FskTxTimeout = txto;
    radioConfig.FskRxTimeout = rxto;
  }
  SX126X_config();
  printf("SET_TIMEOUTS: %d,%d\r\n", txto, rxto);
}

void cli_getsleepparams(int argc, char **argv)
{
  printf("GET_SLEEPPARAMS: ERROR\r\n");
}

void cli_setsleepparams(int argc, char **argv)
{
  printf("SET_SLEEPPARAMS: ERROR\r\n");
}

void cli_getstatus(int argc, char **argv)
{
  uint8_t s = sx126x_GetStatus();
  printf("GET_STATUS: 0x%02X\r\n",s);
}

//LoRa

void cli_lr_getmodparams(int argc, char **argv)
{
  float bw_kHz;

  if(radioConfig.LoRaBw == LORA_BW_7p8) bw_kHz = 7.8;
  else if(radioConfig.LoRaBw == LORA_BW_10p4) bw_kHz = 10.4;
  else if(radioConfig.LoRaBw == LORA_BW_15p6) bw_kHz = 15.6;
  else if(radioConfig.LoRaBw == LORA_BW_20p8) bw_kHz = 20.8;
  else if(radioConfig.LoRaBw == LORA_BW_31p3) bw_kHz = 31.3;
  else if(radioConfig.LoRaBw == LORA_BW_41p7) bw_kHz = 41.7;
  else if(radioConfig.LoRaBw == LORA_BW_62p5) bw_kHz = 62.5;
  else if(radioConfig.LoRaBw == LORA_BW_125) bw_kHz = 125.0;
  else if(radioConfig.LoRaBw == LORA_BW_250) bw_kHz = 250.0;
  else if(radioConfig.LoRaBw == LORA_BW_500) bw_kHz = 500.0;
  else bw_kHz = -1.0;
  printf("LR_GET_MODPARAMS:\r\nBW=%.1f\r\nSF=%d\r\n",bw_kHz,radioConfig.LoRaSf);
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

void cli_lr_setmodparams(int argc, char **argv)
{
  if(radioConfig.modem == MODEM_LORA)
  {
    uint8_t bw_val,sf,cr,opt;
    uint16_t bw;
    bw = ciGetUnsigned(argv[1]);
    float bw_kHz;
    //BW in kHz
    if(bw <= 8) {bw_val = LORA_BW_7p8; bw_kHz = 7.8;}
    else if(bw <= 11) {bw_val = LORA_BW_10p4; bw_kHz = 10.4;}
    else if(bw <= 16) {bw_val = LORA_BW_15p6; bw_kHz = 15.6;}
    else if(bw <= 21) {bw_val = LORA_BW_20p8; bw_kHz = 20.8;}
    else if(bw <= 31) {bw_val = LORA_BW_31p3; bw_kHz = 31.3;}
    else if(bw <= 42) {bw_val = LORA_BW_41p7; bw_kHz = 41.7;}
    else if(bw <= 63) {bw_val = LORA_BW_62p5; bw_kHz = 62.5;}
    else if(bw <= 125) {bw_val = LORA_BW_125; bw_kHz = 125.0;}
    else if(bw <= 250) {bw_val = LORA_BW_250; bw_kHz = 250.0;}
    else {bw_val = LORA_BW_500; bw_kHz = 500.0;}

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
    printf("LR_SET_MODPARAMS:\r\nBW=%.1f\r\nSF=%d\r\n",bw_kHz,radioConfig.LoRaSf);
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
  else printf("LR_SET_MODPARAMS: ERROR\r\n");
}

void cli_lr_getpacketparams(int argc, char **argv)
{
  printf("LR_GET_PACKETPARAMS:\r\nPRE_LEN=%d\r\nPAY_LEN=%d\r\nHEADER:",radioConfig.LoRaPreLen,radioConfig.LoRaPayLen);
  if(radioConfig.LoRaImplHeader == true) printf("IMPLICIT\r\n");
  else printf("EXPLICIT\r\n");
  printf("CRC:");
  if(radioConfig.LoRaCrcOn == true) printf("ON\r\n");
  else printf("OFF\r\n");
  printf("INVERTIQ:");
  if(radioConfig.LoRaInvertIQ == true) printf("ON\r\n");
  else printf("OFF\r\n");
}

void cli_lr_setpacketparams(int argc, char **argv)
{
  if(radioConfig.modem == MODEM_LORA)
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
    printf("LR_SET_PACKETPARAMS:\r\nPRE_LEN=%d\r\nPAY_LEN=%d\r\nHEADER:",radioConfig.LoRaPreLen,radioConfig.LoRaPayLen);
    if(radioConfig.LoRaImplHeader == true) printf("IMPLICIT\r\n");
    else printf("EXPLICIT\r\n");
    printf("CRC:");
    if(radioConfig.LoRaCrcOn == true) printf("ON\r\n");
    else printf("OFF\r\n");
    printf("INVERTIQ:");
    if(radioConfig.LoRaInvertIQ == true) printf("ON\r\n");
    else printf("OFF\r\n");
  }
  else printf("LR_SET_PACKETPARAMS: ERROR\r\n");
}

void cli_lr_getsync(int argc, char **argv)
{
  printf("LR_GET_SYNC: 0x%04X\r\n",radioConfig.LoRaSyncWord);
}

void cli_lr_setsync(int argc, char **argv)
{
  if(radioConfig.modem == MODEM_LORA)
  {
    uint16_t sw;
    sw = ciGetUnsigned(argv[1]) & 0xffff;
    radioConfig.LoRaSyncWord = sw;
    printf("LR_SET_SYNC: 0x%04X\r\n",radioConfig.LoRaSyncWord);
  }
  else printf("LR_SET_SYNC: ERROR\r\n");
}

void cli_lr_getsymboltimeout(int argc, char **argv)
{
  printf("LR_GET_SYMTO: IN DEVELOPMENT\r\n");
}

void cli_lr_setsymboltimeout(int argc, char **argv)
{
  if(radioConfig.modem == MODEM_LORA)
  {
    printf("LR_SET_SYMTO: IN DEVELOPMENT\r\n");
  }
  else printf("LR_SET_SYMTO: ERROR\r\n");
}

//FSK
//baudrate,pulse shape,RXBW,deviation
void cli_fs_getmodparams(int argc, char **argv)
{
  float bw_kHz;

  if(radioConfig.FskBw == FSK_BW_4p8) bw_kHz = 4.8;
  else if(radioConfig.FskBw == FSK_BW_5p8) bw_kHz = 5.8;
  else if(radioConfig.FskBw == FSK_BW_7p3) bw_kHz = 7.3;
  else if(radioConfig.FskBw == FSK_BW_11p7) bw_kHz = 11.7;
  else if(radioConfig.FskBw == FSK_BW_19p5) bw_kHz = 19.5;
  else if(radioConfig.FskBw == FSK_BW_23p4) bw_kHz = 23.4;
  else if(radioConfig.FskBw == FSK_BW_29p3) bw_kHz = 29.3;
  else if(radioConfig.FskBw == FSK_BW_39) bw_kHz = 39.0;
  else if(radioConfig.FskBw == FSK_BW_46p9) bw_kHz = 46.9;
  else if(radioConfig.FskBw == FSK_BW_58p6) bw_kHz = 58.6;
  else if(radioConfig.FskBw == FSK_BW_78p2) bw_kHz = 78.2;
  else if(radioConfig.FskBw == FSK_BW_93p8) bw_kHz = 93.8;
  else if(radioConfig.FskBw == FSK_BW_117p3) bw_kHz = 117.3;
  else if(radioConfig.FskBw == FSK_BW_156p2) bw_kHz = 156.2;
  else if(radioConfig.FskBw == FSK_BW_187p2) bw_kHz = 187.2;
  else if(radioConfig.FskBw == FSK_BW_234p3) bw_kHz = 234.3;
  else if(radioConfig.FskBw == FSK_BW_312) bw_kHz = 312.0;
  else if(radioConfig.FskBw == FSK_BW_373p6) bw_kHz = 373.6;
  else if(radioConfig.FskBw == FSK_BW_467) bw_kHz = 467.0;
  else bw_kHz = -1.0;
  printf("FS_GETMODPARAMS:\r\nBR=%d\r\nSH=%d\r\nBW=%.1f\r\nDEV=%d\r\n",radioConfig.FskBr,radioConfig.FskShaping,bw_kHz,radioConfig.FskFdev);
}

void cli_fs_setmodparams(int argc, char **argv)
{
  if(radioConfig.modem == MODEM_FSK)
  {
    uint32_t br;
    uint8_t sh;
    uint32_t bw;
    uint32_t dev;
    float bw_kHz;
    uint8_t bw_val;
    
    
    br = ciGetUnsigned(argv[1]); //1,2,3 - baudrate
    //check
    if(br < 600) br = 600;
    if(br > 300000) br = 300000;
    sh = ciGetUnsigned(argv[2]); //4 - shaping
    //check
    bw = ciGetUnsigned(argv[3]); //5 - RX BW
    if(bw <= 5) {bw_val = FSK_BW_4p8; bw_kHz = 4.8;}
    else if(bw <= 6) {bw_val = FSK_BW_5p8; bw_kHz = 5.8;}
    else if(bw <= 8) {bw_val = FSK_BW_7p3; bw_kHz = 7.3;}
    else if(bw <= 10) {bw_val = FSK_BW_9p7; bw_kHz = 9.7;}
    else if(bw <= 12) {bw_val = FSK_BW_11p7; bw_kHz = 11.7;}
    else if(bw <= 15) {bw_val = FSK_BW_14p6; bw_kHz = 14.6;}
    else if(bw <= 20) {bw_val = FSK_BW_19p5; bw_kHz = 19.5;}
    else if(bw <= 24) {bw_val = FSK_BW_23p4; bw_kHz = 23.4;}
    else if(bw <= 30) {bw_val = FSK_BW_29p3; bw_kHz = 29.3;}
    else if(bw <= 39) {bw_val = FSK_BW_39; bw_kHz = 39.0;}
    else if(bw <= 47) {bw_val = FSK_BW_46p9; bw_kHz = 46.9;}
    else if(bw <= 59) {bw_val = FSK_BW_58p6; bw_kHz = 58.6;}
    else if(bw <= 79) {bw_val = FSK_BW_78p2; bw_kHz = 78.2;}
    else if(bw <= 94) {bw_val = FSK_BW_93p8; bw_kHz = 93.8;}
    else if(bw <= 118) {bw_val = FSK_BW_117p3; bw_kHz = 117.3;}
    else if(bw <= 157) {bw_val = FSK_BW_156p2; bw_kHz = 156.2;}
    else if(bw <= 188) {bw_val = FSK_BW_187p2; bw_kHz = 187.2;}
    else if(bw <= 235) {bw_val = FSK_BW_234p3; bw_kHz = 234.3;}
    else if(bw <= 312) {bw_val = FSK_BW_312; bw_kHz = 312.0;}
    else if(bw <= 374) {bw_val = FSK_BW_373p6; bw_kHz = 373.6;}
    else  {bw_val = FSK_BW_467; bw_kHz = 467.0;}

    dev = ciGetUnsigned(argv[4]); //6,7,8 - Fdev
    //check
    if(dev < 600) dev = 600;
    if(dev > 200000) dev = 200000;
    radioConfig.FskBr = br;
    radioConfig.FskShaping = sh;
    radioConfig.FskBw = bw_val;
    radioConfig.FskFdev = dev;
    SX126X_config();
    printf("FS_SETMODPARAMS:\r\nBR=%d\r\nSH=%d\r\nBW=%.1f\r\nDEV=%d\r\n",radioConfig.FskBr,radioConfig.FskShaping,bw_kHz,radioConfig.FskFdev);
  }
  else printf("FS_SET_MODPARAMS: ERROR\r\n");
	printf("FS_SET_MODPARAMS: TODO\r\n");
}

//prelen, predetlen,synclen,addr.filt,varlen,paylen,crctype,whitening
void cli_fs_getpacketparams(int argc, char **argv) //bool FskVarLen, bool FskWhiteEn,uint8_t FskCrcType;
{
  printf("FS_GET_PACKETPARAMS:\r\nPRE_LEN=%d\r\nPRE_DET=%d\r\nSYNC_LEN=%d\r\n",radioConfig.FskPreLen,radioConfig.FskPreDetLen,radioConfig.FskSyncLen);
  if(radioConfig.FskAddrComp == 0) printf("ADDR_COMP:OFF\r\n");
  else if(radioConfig.FskAddrComp == 1) printf("ADDR_COMP:NODE\r\n");
  else printf("ADDR_COMP:NODE_BR\r\n");
  if(radioConfig.FskVarLen == true) printf("LEN:VAR\r\n"); 
  else printf("LEN:FIXED\r\n");
  printf("PAY_LEN=%d\r\n",radioConfig.FskPayLen);
  switch(radioConfig.FskCrcType)
  {
    case 0:
      printf("CRC:1BYTE\r\n");
      break;
    case 1:
      printf("CRC:OFF\r\n");
      break;
    case 2:
      printf("CRC:2BYTES\r\n");
      break;
    case 4:
      printf("CRC:1BYTE_INV\r\n");
      break;
    case 6:
      printf("CRC:2BYTES_INV\r\n");
      break;
    default:
      printf("CRC:INVALID\r\n");
      break;
  }
  if(radioConfig.FskWhiteEn == true) printf("WHITE:ON\r\n"); 
  else printf("WHITE:OFF\r\n"); 
	printf("FS_GET_PACKETPARAMS: TODO\r\n");
}

void cli_fs_setpacketparams(int argc, char **argv)
{
  if(radioConfig.modem == MODEM_FSK)
  {
    uint16_t prelen;
    uint8_t predetlen;
    uint8_t synclen;
    uint8_t addrfilt;
    bool varlen;
    uint8_t paylen;
    uint8_t crctype;
    bool whiteen;
    
    prelen = ciGetUnsigned(argv[1]);
    //check
    predetlen = ciGetUnsigned(argv[2]);
    //check
    synclen = ciGetUnsigned(argv[3]);
    //check
    addrfilt = ciGetUnsigned(argv[4]);
    //check
    varlen = (bool)ciGetUnsigned(argv[5]);
    //check
    paylen = ciGetUnsigned(argv[6]);
    //check
    crctype = ciGetUnsigned(argv[7]);
    //check
    whiteen = (bool)ciGetUnsigned(argv[8]);
    //check
    //FLASH_Unlock(FLASH_MemType_Data);
    radioConfig.FskPreLen = prelen;
    radioConfig.FskPreDetLen = predetlen;
    radioConfig.FskSyncLen = synclen;
    radioConfig.FskAddrComp = addrfilt;
    radioConfig.FskVarLen = varlen;
    radioConfig.FskPayLen = paylen;
    radioConfig.FskCrcType = crctype;
    radioConfig.FskWhiteEn = whiteen;
    //FLASH_Lock(FLASH_MemType_Data);
    SX126X_config();
    printf("FS_SET_PACKETPARAMS:\r\nPRE_LEN=%d\r\nPRE_DET=%d\r\nSYNC_LEN=%d\r\n",radioConfig.FskPreLen,radioConfig.FskPreDetLen,radioConfig.FskSyncLen);
    if(radioConfig.FskAddrComp == 0) printf("ADDR_COMP:OFF\r\n");
    else if(radioConfig.FskAddrComp == 1) printf("ADDR_COMP:NODE\r\n");
    else printf("ADDR_COMP:NODE_BR\r\n");
    if(radioConfig.FskVarLen == true) printf("LEN:VAR\r\n"); 
    else printf("LEN:FIXED\r\n");
    printf("PAY_LEN=%d\r\n",radioConfig.FskPayLen);
    switch(radioConfig.FskCrcType)
    {
      case 0:
      printf("CRC:1BYTE\r\n");
      break;
      case 1:
      printf("CRC:OFF\r\n");
      break;
      case 2:
      printf("CRC:2BYTES\r\n");
      break;
      case 4:
      printf("CRC:1BYTE_INV\r\n");
      break;
      case 6:
      printf("CRC:2BYTES_INV\r\n");
      break;
      default:
      printf("CRC:INVALID\r\n");
      break;
    }
    if(radioConfig.FskWhiteEn == true) printf("WHITE:ON\r\n"); 
    else printf("WHITE:OFF\r\n"); 
  }
  else printf("FS_SET_PACKETPARAMS: ERROR\r\n");
	printf("FS_SET_PACKETPARAMS: TODO\r\n");
}

//node and broadcast addr.
void cli_fs_getaddr(int argc, char **argv)
{
  printf("FS_GET_ADDR: 0x%02X,0x%02X\r\n",radioConfig.FskNodeAddr,radioConfig.FskBrAddr);
}

void cli_fs_setaddr(int argc, char **argv)
{
  if(radioConfig.modem == MODEM_FSK)
  {
    uint8_t nodeaddr,braddr;
    
    nodeaddr = ciGetUnsigned(argv[1]);
    braddr = ciGetUnsigned(argv[2]);
    radioConfig.FskNodeAddr = nodeaddr;
    radioConfig.FskBrAddr = braddr;
    SX126X_config();
    printf("FS_SET_ADDR: 0x%02X,0x%02X\r\n",radioConfig.FskNodeAddr,radioConfig.FskBrAddr);
  }
  else printf("FS_SET_ADDR: ERROR\r\n");
}

//sync word(s)
void cli_fs_getsync(int argc, char **argv)
{
  uint8_t i;
  
  printf("FS_GET_SYNC: ");
  for(i = 0; i < 7; i++) printf("0x%02X,",radioConfig.FskSync[i]);
  printf("0x%02X\r\n",radioConfig.FskSync[7]);
}

void cli_fs_setsync(int argc, char **argv)
{
  if(radioConfig.modem == MODEM_FSK)
  {
    uint8_t sync[8];
    uint8_t i;
    
    for(i = 0; i < 8; i++) sync[i] = ciGetUnsigned(argv[i+1]);
    for(i = 0; i < 8; i++) radioConfig.FskSync[i] = sync[i];
    SX126X_config();
    printf("FS_SET_SYNC: ");
    for(i = 0; i < 7; i++) printf("0x%02X,",radioConfig.FskSync[i]);
    printf("0x%02X\r\n",radioConfig.FskSync[7]);
  }
  else printf("FS_SET_SYNC: ERROR\r\n");
}
//CRC init, CRC poly, whitening init
void cli_fs_getcrcwhite(int argc, char **argv)
{
  printf("FS_GET_CRCWH:\r\nCRC_INIT=0x%04X\r\nCRC_POLY=0x%04X\r\nWH_INIT=0x%04X\r\n",radioConfig.FskCrcInit,radioConfig.FskCrcPoly,radioConfig.FskWhiteInit);
}

void cli_fs_setcrcwhite(int argc, char **argv)
{
  if(radioConfig.modem == MODEM_FSK)
  {
    uint16_t c_init, c_poly, w_init;
    
    c_init = ciGetUnsigned(argv[1]);
    c_poly = ciGetUnsigned(argv[2]);
    w_init = ciGetUnsigned(argv[3]);
    
    radioConfig.FskCrcInit = c_init;
    radioConfig.FskCrcPoly = c_poly;
    radioConfig.FskWhiteInit = w_init;
    SX126X_config();
    printf("FS_SET_CRCWH:\r\nCRC_INIT=0x%04X\r\nCRC_POLY=0x%04X\r\nWH_INIT=0x%04X\r\n",radioConfig.FskCrcInit,radioConfig.FskCrcPoly,radioConfig.FskWhiteInit);
  }
  else printf("FS_SET_CRCWH: ERROR\r\n");
}

//System tests
void cli_sendburst(int argc, char **argv)
{
  txpacketcount = ciGetUnsigned(argv[1]);
  if(txpacketcount == 0) txpacketcount = 1;
  inter_packet_delay = ciGetUnsigned(argv[2]);
  slaveID = ciGetUnsigned(argv[3]);
  if(inter_packet_delay < 100) inter_packet_delay = 100;
  radio_startburst();
  printf("SEND_PACKET: %d,%d\r\n",txpacketcount,inter_packet_delay);
}

void cli_stopburst (int argc, char **argv)
{
  if(master)
  {
    master = false;
    tx_needed = false;
    printf("STOP_TX: OK\r\n");
  }
  else printf("STOP_TX: ERROR\r\n");
}

void cli_getautosendparams(int argc, char **argv)
{
  printf("GET_AUTOSEND: %d,0x%04X\r\n",radioConfig.InterPacketDelay,radioConfig.SlaveID);
}

void cli_setautosendparams(int argc, char **argv)
{
  uint16_t d,id;
  d = ciGetUnsigned(argv[1]);
  id = ciGetUnsigned(argv[2]);
  if(d < 50) d = 50;
  if(d > 60000) d = 60000;
  radioConfig.InterPacketDelay = d;
  radioConfig.SlaveID = id;
  printf("SET_AUTOSEND: %d,0x%04X\r\n",radioConfig.InterPacketDelay,radioConfig.SlaveID);
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
      if(radioConfig.modem == MODEM_FSK) SX126xx_FskConfig();
      
      SX126X_setopmode(OPMODE_RX); //SX126X_SetRx(radioConfig.LoRaRxTimeout);
      printf("TX_STREAM: OFF\r\n");
      txmode = 0;
      break;

    case 1:
      prevopmode = opmode;
      SX126X_setopmode(OPMODE_TXSTREAMCW);//SX126X_SetCW();
      printf("TX_STREAM: CW\r\n");
      txmode = 1;
      break;

    case 2:
      prevopmode = opmode;
      SX126X_setopmode(OPMODE_TXSTREAMPRE);//SX126X_SetTxInfinitePreamble();
      printf("TX_STREAM: PREAMBLE\r\n");
      txmode = 2;
      break;
  }
}

void cli_sweeptx(int argc, char **argv)
{
  uint8_t stream;
	
  minchan = ciGetUnsigned(argv[1]);
  maxchan = ciGetUnsigned(argv[2]);
  sweepdelay = ciGetUnsigned(argv[3]);
  stream = ciGetUnsigned(argv[4]);
  if(stream > 2) stream = 2;
  if((stream == 0) || (sweepdelay == 0))
  {
    if(txmode != 0)
    {
      sweeptx = false;
			ustimer_stop();
      SX126X_setopmode(prevopmode);
      Channel = radioConfig.Channel;
      SX126X_SetChannel();
      txmode = 0;
      printf("SWEEP_TX: OFF\r\n");
    }
    else printf("SWEEP_TX: ERROR\r\n");
  }
  else
  {
    if(txmode != 0)
    {
      printf("SWEEP_TX: ERROR\r\n");
    }
    else
    {
      printf("SWEEP_TX: START\r\n");
      prevopmode = opmode;
      Channel = minchan;
      SX126X_SetChannel();
      //sweepcnt = sweepdelay;
			ustimer_setinterval(sweepdelay);
      if(stream == 1) 
      {
        SX126X_setopmode(OPMODE_TXSTREAMCW);
				txmode = 1;
        printf("TX_STREAM: CW\r\n");
      }
      else 
      {
        SX126X_setopmode(OPMODE_TXSTREAMPRE);
        txmode = 2;
        printf("TX_STREAM: PREAMBLE\r\n");
      }
      sweeptx = true;
			ustimer_start();
    }
  }
}

void cli_sweeprx(int argc, char **argv)
{
  minchan = ciGetUnsigned(argv[1]);
  maxchan = ciGetUnsigned(argv[2]);
  sweepdelay = ciGetUnsigned(argv[3]);
  if(sweepdelay == 0)
  {
    sweeprx = false;
    Channel = radioConfig.Channel;
    SX126X_SetChannel();
		ustimer_stop();
    printf("SWEEP_RX: STOP\r\n");
  }
  else
  {
    printf("SWEEP_RX: START\r\n");
    Channel = minchan;
    SX126X_SetChannel();
		ustimer_setinterval(sweepdelay);
		ustimer_start();
    sweeprx = true;
  }
}

void cli_startrxcount(int argc, char **argv)
{
  printf("START_RXCOUNT: TODO\r\n");
}

void cli_stoprxcount(int argc, char **argv)
{
  printf("STOP_RXCOUNT: TODO\r\n");
}

void cli_getrxcount(int argc, char **argv)
{
  printf("GET_RXCOUNT: TODO\r\n");
}

void cli_getrssioffset(int argc, char **argv)
{
  printf("GET_RSSIOFFSET: TODO\r\n");
}

void cli_setrssioffset(int argc, char **argv)
{
  printf("SET_RSSIOFFSET: TODO\r\n");
}

//Device dependent commands
void cli_getvt(int argc, char **argv)
{
  printf("GET_VT: T=%.3f,V=%.3f\r\n",T,Vcc);
}

void cli_setem(int argc, char **argv)
{
/*  uint8_t em;
  
  em = ciGetUnsigned(argv[1]);
  if(em > 5) em = 5;
  
  switch(em)
  {
    case 0:
    default:
      printf("SET_EM: 0\r\n");
    break;
    
    case 1: //CPU stall
      printf("SET_EM: 1\r\n");
      setEM1();
    break;
    
    case 2://CPU active, radio off
      printf("SET_EM: 2\r\n");
      setEM2();
    break;
    
    case 3:
      printf("SET_EM: 3\r\n");
      setEM3();
    break;
    
    case 4:
      printf("SET_EM: 4\r\n");
      setEM4();
    break;
    
    case 5:
      printf("SET_EM: 5\r\n");
      setEM5();
    break;
  }
*/	
	printf("SET_EM: TODO\r\n");
}

void cli_readreg(int argc, char **argv)
{
  uint16_t r;
  r = ciGetUnsigned(argv[1]);
  printf("READ_REG: 0x%02X\r\n",SX126X_readreg(r));
}

void cli_writereg(int argc, char **argv)
{
  uint16_t reg;
  uint8_t val;
  reg = ciGetUnsigned(argv[1]) & 0xfff;
  val = ciGetUnsigned(argv[2]) & 0xff;
  SX126X_writereg(reg,val);
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
    printf("0x%04X,0x%02X\r\n",i,SX126X_readreg(i));
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

void cli_getgpsdata(int argc, char **argv)
{
/*  printf("GET_GNSS:\r\n");
  printf("TIME: %d:%d:%d\r\n", GPSdata.hour,GPSdata.min,GPSdata.sec);
  printf("LAT: %f\r\n", GPSdata.lat);
  printf("LON: %f\r\n", GPSdata.lon);
  printf("ALT: %.3f\r\n",GPSdata.alt);
  printf("STATUS: %d\r\n",GPSdata.status);
  printf("SAT: %d\r\n", GPSdata.sat);
  printf("HDOP: %.1f\r\n", GPSdata.hdop);
*/	
	printf("GET_GNSS: TODO\r\n");
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


      


