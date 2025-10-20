#include <stdio.h>
#include "app_cli.h"
#include "bsp.h"
#include "retargetserial.h"
//#include "adc.h"
#include "flash.h"
//#include "gui.h"
#include "radio.h"
//#include "rtc.h"

CommandState_t state;
char ciBuffer[256];

//uint8_t txmode = 0;

//General
void cli_reset(int argc, char **argv);
void cli_getuid(int argc, char **argv);
void cli_getver(int argc, char **argv);
void cli_getdevid(int argc, char **argv);
void cli_setdevid(int argc, char **argv);
////RF settings
//void cli_getkfreq(int argc, char **argv);
//void cli_setkfreq(int argc, char **argv);
void cli_getchip(int argc, char **argv);
void cli_getmodem(int argc, char **argv);
void cli_setmodem(int argc, char **argv);
void cli_getfreq(int argc, char **argv);
void cli_setfreq(int argc, char **argv);

//void cli_getscanparams(int argc, char **argv);
//void cli_setscanparams(int argc, char **argv);

//void cli_getactivescans(int argc, char **argv);
//void cli_setactivescans(int argc, char **argv);

void cli_getpower(int argc, char **argv);
void cli_setpower(int argc, char **argv);

void cli_getctune(int argc, char **argv);
void cli_setctune(int argc, char **argv);

void cli_getRSSI(int argc, char **argv);
void cli_RSSIcal(int argc, char **argv);

//LoRa
void cli_lr_getmodparams(int argc, char **argv);
void cli_lr_setmodparams(int argc, char **argv);
void cli_lr_getpacketparams(int argc, char **argv);
void cli_lr_setpacketparams(int argc, char **argv);
void cli_lr_getsync(int argc, char **argv);
void cli_lr_setsync(int argc, char **argv);

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
void cli_getstatus(int argc, char **argv);
void cli_setopmode(int argc, char **argv);
void cli_geterrors(int argc, char **argv);
void cli_clearerrors(int argc, char **argv);


void cli_storeconfig(int argc, char **argv);

void cli_sendburst(int argc, char **argv);
void cli_stopburst (int argc, char **argv);
void cli_txstream(int argc, char **argv);

//device dependent
void cli_getvt(int argc, char **argv);
void cli_gettemp(int argc, char **argv);
void cli_getvcc(int argc, char **argv);

//special tests
void cli_sweeptx(int argc, char **argv);
void cli_sweeprx(int argc, char **argv);

void cli_gettime(int argc, char **argv);

//RX functions
void cli_getrxstats(int argc, char **argv);
void cli_clrrxstats(int argc, char **argv);

void cli_poweron(int argc, char **argv);
void cli_poweroff(int argc, char **argv);

CommandEntry_t commands[] =
  {
    //general
    COMMAND_ENTRY("RESET", "", cli_reset, "Device reset"),
    COMMAND_ENTRY("GET_UID", "", cli_getuid, "Get CPU ID"),
    COMMAND_ENTRY("GET_VERSION", "", cli_getver, "Get HW/FW version"),
    COMMAND_ENTRY("GET_DEVID", "", cli_getdevid, "Get device ID"),
    COMMAND_ENTRY("SET_DEVID", "w", cli_setdevid, "Get device ID"),
    //RF settings
		COMMAND_ENTRY("GET_CHIP", "", cli_getchip, ""),
    COMMAND_ENTRY("GET_MODEM", "", cli_getmodem, ""),
    COMMAND_ENTRY("SET_MODEM", "b", cli_setmodem, ""),
		
		//COMMAND_ENTRY("GET_KFREQ", "w", cli_getkfreq, ""),
    //COMMAND_ENTRY("SET_KFREQ", "ww", cli_setkfreq, ""),
		
    COMMAND_ENTRY("GET_FREQ", "", cli_getfreq, "Get frequency settings"),
    COMMAND_ENTRY("SET_FREQ", "w", cli_setfreq, "Set frequency settings"),
//		COMMAND_ENTRY("GET_SCANPARAMS", "", cli_getscanparams, "Get scanning parameters"),
//    COMMAND_ENTRY("SET_SCANPARAMS", "wwwww", cli_setscanparams, "Set scanning parameters"),
//		COMMAND_ENTRY("GET_SCANS","",cli_getactivescans,""),
//		COMMAND_ENTRY("SET_SCANS","w",cli_setactivescans,""),
    COMMAND_ENTRY("GET_POWER", "", cli_getpower, ""),
    COMMAND_ENTRY("SET_POWER", "w", cli_setpower, ""),
    COMMAND_ENTRY("GET_XOTRIM", "", cli_getctune, ""),
    COMMAND_ENTRY("SET_XOTRIM", "w", cli_setctune, ""),

    COMMAND_ENTRY("GET_RSSI", "", cli_getRSSI, ""),
		COMMAND_ENTRY("RSSI_CAL", "w", cli_RSSIcal, ""), //calibrate RSSI (frequency, powerlevel)
    
    //LoRa
    COMMAND_ENTRY("LR_GET_MODPARAMS", "", cli_lr_getmodparams, "Get LoRa modulation parameters"),
    COMMAND_ENTRY("LR_SET_MODPARAMS", "wwww", cli_lr_setmodparams, "Set LoRa modulation parameters"),
    COMMAND_ENTRY("LR_GET_PACKETPARAMS", "", cli_lr_getpacketparams, "Get LoRa packet parameters"),
    COMMAND_ENTRY("LR_SET_PACKETPARAMS", "wwwww", cli_lr_setpacketparams, "Set LoRa packet parameters"),
    COMMAND_ENTRY("LR_GET_SYNC", "", cli_lr_getsync, "Get LoRa sync word"),
    COMMAND_ENTRY("LR_SET_SYNC", "w", cli_lr_setsync, "Set LoRa sync word"),
                        
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
    COMMAND_ENTRY("START_TX", "www", cli_sendburst, "Start packet burst"),
    COMMAND_ENTRY("STOP_TX", "", cli_stopburst, "Stop packet burst"),
    COMMAND_ENTRY("TX_STREAM", "w", cli_txstream, "Start/stop TX stream"),
    //System health - device dependent
    COMMAND_ENTRY("GET_VBAT", "", cli_getvt, "Get ADC data"),
    COMMAND_ENTRY("GET_TEMP", "", cli_gettemp, ""),
		COMMAND_ENTRY("GET_VCC", "", cli_getvcc, ""),

    COMMAND_ENTRY("READ_REG", "w", cli_readreg, ""),
    COMMAND_ENTRY("WRITE_REG", "ww", cli_writereg, ""),
    COMMAND_ENTRY("DUMP_REGS", "ww", cli_dumpregs, ""),
		COMMAND_ENTRY("GET_STATUS", "", cli_getstatus, ""),
		COMMAND_ENTRY("SET_OPMODE", "w", cli_setopmode, ""),
		
		COMMAND_ENTRY("GET_ERRORS", "", cli_geterrors, ""),
		COMMAND_ENTRY("CLEAR_ERRORS", "", cli_clearerrors, ""),
                        
    COMMAND_ENTRY("SWEEP_TX", "wwwww", cli_sweeptx, "TX sweep"),
    COMMAND_ENTRY("SWEEP_RX", "wwwww", cli_sweeprx, "RX scan"),

    COMMAND_ENTRY("INIT_CONFIG", "w", cli_initconfig, ""),
		COMMAND_ENTRY("STORE_CONFIG", "", cli_storeconfig, ""),
		//COMMAND_ENTRY("GET_TIME", "", cli_gettime, ""),
		
		COMMAND_ENTRY("GET_RXSTATS", "", cli_getrxstats, ""),
		COMMAND_ENTRY("CLR_RXSTATS", "", cli_clrrxstats, ""),
		
		COMMAND_ENTRY("RADIO_ON", "", cli_poweron, ""),
		COMMAND_ENTRY("RADIO_OFF", "", cli_poweroff, ""),
		
		COMMAND_ENTRY(NULL, NULL, NULL, NULL),
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
  if (error == CI_UNKNOWN_COMMAND) 
	{
    printf("INVALID COMMAND\r\n");
  } 
	else if (error == CI_MAX_ARGUMENTS) 
	{
    printf("TOO MANY ARGUMENTS\r\n");
  } 
	else if (error == CI_INVALID_ARGUMENTS) 
	{
    printf("INVALID ARGUMENT\r\n");
  }
}


//cli functions implementation
//General
void cli_reset(int argc, char **argv)
{
	printf("RESET: OK\r\n");
	delay_ms(500);
	//NVIC_SystemReset();
	__NVIC_SystemReset();
}

void cli_getuid(int argc, char **argv)
{
  uint64_t id = *(uint64_t*)0x1ffff7e8;
	uint16_t flash_size = *(uint16_t*)0x1ffff7e0;
	printf("GET_UID: 0x%llX\r\n", id);
	printf("FLASH SIZE: %dK\r\n",flash_size);
}

void cli_getver(int argc, char **argv)
{
  printf("GET_VERSION: HW=%d,FW=%d.%d\r\n", HW_VERSION, FW_VERSION, FW_REVISION);
}

void cli_getdevid(int argc, char **argv)
{
	printf("GET_DEVID: 0x%08X\r\n",radioconfig.device_id);
}

void cli_setdevid(int argc, char **argv)
{
  uint32_t id = ciGetUnsigned(argv[1]);
	radioconfig.device_id = id;
//  writeconfig();
	printf("SET_DEVID: 0x%08X\r\n",radioconfig.device_id);
}


//RF settings
void cli_getchip(int argc, char **argv)
{
	printf("GET_CHIP: SX126X\r\n");
}

void cli_getmodem(int argc, char **argv)
{
	printf("GET_MODEM: ");
	switch(radioconfig.modem)
	{
		case MODEM_NONE:
		printf("RAW");
		break;
		
		case MODEM_FSK:
		printf("FSK");
		break;
		
		case MODEM_FSK_GENERIC:
		printf("FSK GENERIC");
		break;
		
		case MODEM_LORA:
		printf("LORA");
		break;
		
		case MODEM_SIGFOX:
		printf("SIGFOX");
		break;
		
		case MODEM_GMSK:
		printf("GMSK");
		break;
		
		case MODEM_LR_FHSS:
		printf("LR-FHSS");
		break;
		
		case MODEM_OQPSK:
		printf("OQPSK");
		break;
		
		case MODEM_RANGING:
		printf("RANGING");
		break;
		
		case MODEM_FLRC:
		printf("FLRC");
		break;
		
		case MODEM_BLE:
		printf("BLE");
		break;
		
		case MODEM_BPSK:
		printf("BPSK");
		break;
		
		case MODEM_OOK:
		printf("RAW");
		break;
		
		case MODEM_WMBUS:
		printf("WMBUS");
		break;
		
		case MODEM_WISUN:
		printf("WISUN");
		break;
		
		case MODEM_ZWAVE:
		printf("ZWAVE");
		break;
		
		case MODEM_ZIGBEE:
		printf("ZIGBEE");
		break;
		
		default:
		printf("INVALID");
		break;
	}
	printf("\r\n");
}

void cli_setmodem(int argc, char **argv)
{
	uint8_t modem;

	if(strcmp((char*)argv[1], "RAW") == 0) modem = MODEM_NONE;
	else if(strcmp((char*)argv[1], "FSK") == 0) modem = MODEM_FSK;
	else if(strcmp((char*)argv[1], "FSK_G") == 0) modem = MODEM_FSK_GENERIC;	
	else if(strcmp((char*)argv[1], "LORA") == 0) modem = MODEM_LORA;	
	else if(strcmp((char*)argv[1], "SIGFOX") == 0) modem = MODEM_SIGFOX;	
	else if(strcmp((char*)argv[1], "GMSK") == 0) modem = MODEM_GMSK;
	else if(strcmp((char*)argv[1], "LR_FHSS") == 0) modem = MODEM_LR_FHSS;
	else if(strcmp((char*)argv[1], "OQPSK") == 0) modem = MODEM_OQPSK;
	else if(strcmp((char*)argv[1], "RANGING") == 0) modem = MODEM_RANGING;
	else if(strcmp((char*)argv[1], "FLRC") == 0) modem = MODEM_FLRC;
	else if(strcmp((char*)argv[1], "BLE") == 0) modem = MODEM_BLE;
	else if(strcmp((char*)argv[1], "BPSK") == 0) modem = MODEM_BPSK;
	else if(strcmp((char*)argv[1], "OOK") == 0) modem = MODEM_OOK;
	else if(strcmp((char*)argv[1], "WMBUS") == 0) modem = MODEM_WMBUS;
	else if(strcmp((char*)argv[1], "WISUN") == 0) modem = MODEM_WISUN;
	else if(strcmp((char*)argv[1], "ZWAVE") == 0) modem = MODEM_ZWAVE;
	else if(strcmp((char*)argv[1], "ZIGBEE") == 0) modem = MODEM_ZIGBEE;
	else modem = MODEM_INVALID;
	if(modem == MODEM_INVALID) printf("SET_MODEM: INVALID MODEM\r\n");
	else
	{
		int8_t err = radio_setmodem(modem);
		printf("SET_MODEM: ");
		if(err == RADIO_OK) printf("OK\r\n");
		else printerror(err);
	}
}


void cli_getfreq(int argc, char **argv)
{
	printf("GET_FREQ: %u\r\n",radioconfig.frequency0 / 1000);
}

void cli_setfreq(int argc, char **argv)
{
	uint32_t freq = ciGetUnsigned(argv[1]);
	radioconfig.frequency0 = freq * 1000;
	printf("SET_FREQ: %u\r\n",freq);
}

//void cli_getscanparams(int argc, char **argv)
//{
////	if(radio == RADIO_L) printf("GET_SCANPARAMS: %u,%u,%u,%u,%u,%d\r\n",radio,globalrfconfig.LBconfig.startfreq / 1000,
////																																				globalrfconfig.LBconfig.stopfreq / 1000,
////																																				globalrfconfig.LBconfig.freqstep / 1000,
////																																				globalrfconfig.LBconfig.scantimems,
////																																				globalrfconfig.LBconfig.rssithreshold);
////	else printf("GET_SCANPARAMS: %u,%u,%u,%u,%u,%d\r\n",radio,globalrfconfig.HBconfig.startfreq / 1000,
////																											globalrfconfig.HBconfig.stopfreq / 1000,
////																											globalrfconfig.HBconfig.freqstep / 1000,
////																											globalrfconfig.HBconfig.scantimems,
////																											globalrfconfig.HBconfig.rssithreshold);
//	printf("GET_SCANPARAMS: TODO\r\n");
//}

//void cli_setscanparams(int argc, char **argv)
//{
////	uint8_t radio = ciGetUnsigned(argv[1]);
////	uint32_t start = ciGetUnsigned(argv[2]);
////	uint32_t stop = ciGetUnsigned(argv[3]);
////	uint32_t step = ciGetUnsigned(argv[4]);
////	uint16_t time = ciGetUnsigned(argv[5]);
////	int8_t tr = ciGetSigned(argv[6]);
////	
////	if(radio > 1)
////	{
////		printf("SET_SCANPARAMS: INVALID RADIO\r\n");
////		return;
////	}
////	if(radio == RADIO_L) 
////	{
////		globalrfconfig.LBconfig.startfreq = start * 1000;
////		globalrfconfig.LBconfig.stopfreq = stop * 1000;
////		globalrfconfig.LBconfig.freqstep= step * 1000;
////		globalrfconfig.LBconfig.scantimems = time;
////		globalrfconfig.LBconfig.rssithreshold = tr;
////	}
////	else 
////	{
////		globalrfconfig.HBconfig.startfreq = start * 1000;
////		globalrfconfig.HBconfig.stopfreq = stop * 1000;
////		globalrfconfig.HBconfig.freqstep= step * 1000;
////		globalrfconfig.HBconfig.scantimems = time;
////		globalrfconfig.HBconfig.rssithreshold = tr;
////	}
////	if(radio_set_frequency(radio,start * 1000) == true) printf("SET_SCANPARAMS: %u,OK\r\n",radio);
////	else printf("SET_SCANPARAMS: %u,ERROR\r\n",radio);
////	//updatescreen();
//	printf("SET_SCANPARAMS: TODO\r\n");
//}


void cli_getpower(int argc, char **argv)
{
	printf("GET_POWER: %d dBm\r\n",radioconfig.txpower0);
}

void cli_setpower(int argc, char **argv)
{
	int8_t pwr = ciGetSigned(argv[1]);
	int8_t err = radio_setpower(pwr);
	printf("SET_POWER: ");
	if(err == RADIO_OK) 
	{
		printf("%d\r\n",pwr);
	}
	else printerror(err);
}

void cli_getctune(int argc, char **argv)
{
	uint16_t trim;
	int8_t err = radio_getxotrim(&trim);
	printf("GET_XOTRIM: ");
	if(err == RADIO_OK) printf("%d\r\n",trim);
	else printerror(err); 
}

void cli_setctune(int argc, char **argv)
{
	uint16_t trim = ciGetUnsigned(argv[1]);
	int8_t err = radio_setxotrim(trim);
	printf("SET_XOTRIM: ");
	if(err == RADIO_OK) printf("%d\r\n",trim);
	else printerror(err); 
}


void cli_getRSSI(int argc, char **argv)
{
	float rssi;
	int8_t err = radio_getrssi(&rssi);
	printf("GET_RSSI: ");
	if(err == RADIO_OK) printf("%.1fdBm\r\n",rssi);
	else printerror(err); 
}


//LoRa

void cli_lr_getmodparams(int argc, char **argv)
{
  float bw_kHz;

	uint8_t bw = radioconfig.bw;
	uint8_t sf = radioconfig.sf;
	uint8_t cr = radioconfig.cr;
	uint8_t opt = radioconfig.ldropt;

	if(bw == LORA_BW_7p8) bw_kHz = 7.8;
	else if(bw == LORA_BW_10p4) bw_kHz = 10.4;
	else if(bw == LORA_BW_15p6) bw_kHz = 15.6;
	else if(bw == LORA_BW_20p8) bw_kHz = 20.8;
	else if(bw == LORA_BW_31p3) bw_kHz = 31.3;
	else if(bw == LORA_BW_41p7) bw_kHz = 41.7;
	else if(bw == LORA_BW_62p5) bw_kHz = 62.5;
	else if(bw == LORA_BW_125) bw_kHz = 125.0;
	else if(bw == LORA_BW_250) bw_kHz = 250.0;
	else if(bw == LORA_BW_500) bw_kHz = 500.0;
	else if(bw == LORA_BW_203) bw_kHz = 203.0;
	else if(bw == LORA_BW_406) bw_kHz = 406.0;
	else if(bw == LORA_BW_812) bw_kHz = 812.0;
	else bw_kHz = -1.0;
 	printf("LR_GET_MODPARAMS:\r\nBW=%.1f\r\nSF=%d\r\n",bw_kHz,sf);
	printf("CR:");
	switch(cr)
	{
		case 0:
    printf("OFF\r\n");
    break;
    case LORA_CR_4_5:
    printf("4_5\r\n");
    break;
    case LORA_CR_4_6:
    printf("4_6\r\n");
    break;
    case LORA_CR_4_7:
    printf("4_7\r\n");
    break;
    case LORA_CR_4_8:
    printf("4_8\r\n");
    break;
		case LORA_CR_4_5_LI:
    printf("LI_4_5\r\n");
    break;
		case LORA_CR_4_6_LI:
    printf("LI_4_6\r\n");
    break;
		case LORA_CR_4_8_LI:
    printf("LI_4_8\r\n");
    break;
    default:
    printf("INVALID\r\n");
    break;
	}
	printf("LROPT:");
	if(opt == 0) printf("OFF\r\n");
	else printf("ON\r\n");
}

void cli_lr_setmodparams(int argc, char **argv)
{
	uint8_t bw_val;
	float bw_kHz;
	
	uint8_t bw = ciGetUnsigned(argv[1]);
	uint8_t sf = ciGetUnsigned(argv[2]);
	uint8_t cr = ciGetUnsigned(argv[3]);
	uint8_t opt = ciGetUnsigned(argv[4]);
	if(opt > 1) opt = 1;
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
	if(sf < 6) sf = 6;
	if(sf > 12) sf = 12;
	if(cr > 4) cr = 4;
	radioconfig.bw = bw_val;
	radioconfig.sf = sf;
	radioconfig.cr = cr;
	radioconfig.ldropt = opt;
	radio_config();
	printf("LR_SET_MODPARAMS: \r\nBW=%.1f\r\nSF=%d\r\n",bw_kHz,sf);
	printf("CR:");
	switch(cr)
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
		case 5:
		printf("LI_4_5\r\n");
		break;
		case 6:
		printf("LI_4_6\r\n");
		break;
		case 7:
		printf("LI_4_8\r\n");
		break;
		default:
		printf("INVALID\r\n");
		break;
	}
	printf("LROPT:");
	if(opt == 0) printf("OFF\r\n");
	else printf("ON\r\n");
}

void cli_lr_getpacketparams(int argc, char **argv)
{
	printf("LR_GET_PACKETPARAMS: \r\nPRE_LEN=%d\r\nPAY_LEN=%d\r\nHEADER:",radioconfig.loraprelen,radioconfig.lorapaylen);
	if(radioconfig.implheader == 0) printf("EXPLICIT\r\n");
	else printf("IMPLICIT\r\n");
	printf("CRC:");
	if(radioconfig.loracrc == true) printf("ON\r\n");
	else printf("OFF\r\n");
	printf("INVERTIQ:");
	if(radioconfig.invertiq == 0) printf("OFF\r\n");
	else printf("ON\r\n");
}

void cli_lr_setpacketparams(int argc, char **argv)
{
  uint16_t pre = ciGetUnsigned(argv[1]) & 0xffff;
  uint8_t pay = ciGetUnsigned(argv[2]) & 0xff;
  uint8_t head = ciGetUnsigned(argv[3]) & 0x1;
  uint8_t crc = ciGetUnsigned(argv[4]) & 0x1;
  uint8_t inviq = ciGetUnsigned(argv[5]) & 0x1;
	
	radioconfig.loraprelen = pre;
	radioconfig.lorapaylen = pay;
	if(head != 0) radioconfig.implheader = 1;
	else radioconfig.implheader = 0;
	if(crc != 0) radioconfig.loracrc = 1;
	else radioconfig.loracrc = 0;
	if(inviq != 0) radioconfig.invertiq = 1;
	else radioconfig.invertiq = 0;
	radio_config();
	printf("LR_SET_PACKETPARAMS:\r\nPRE_LEN=%d\r\nPAY_LEN=%d\r\nHEADER:",radioconfig.loraprelen,radioconfig.lorapaylen);
	if(radioconfig.implheader == 1) printf("IMPLICIT\r\n");
	else printf("EXPLICIT\r\n");
	printf("CRC:");
	if(radioconfig.loracrc == 1) printf("ON\r\n");
	else printf("OFF\r\n");
	printf("INVERTIQ:");
	if(radioconfig.invertiq == 1) printf("ON\r\n");
	else printf("OFF\r\n");
}

void cli_lr_getsync(int argc, char **argv)
{
	printf("LR_GET_SYNC: 0x%04X\r\n",radioconfig.lorasync);
}

void cli_lr_setsync(int argc, char **argv)
{
	uint16_t sync = ciGetUnsigned(argv[1]);

	radioconfig.lorasync = sync;
	radio_config();
	printf("LR_SET_SYNC: 0x%04X\r\n",radioconfig.lorasync);
}


//FSK
//baudrate,pulse shape,RXBW,deviation
void cli_fs_getmodparams(int argc, char **argv)
{
  float bw_kHz;
	if(radioconfig.rbw == FSK_BW_4p8) bw_kHz = 4.8;
	else if(radioconfig.rbw == FSK_BW_5p8) bw_kHz = 5.8;
	else if(radioconfig.rbw == FSK_BW_7p3) bw_kHz = 7.3;
	else if(radioconfig.rbw == FSK_BW_11p7) bw_kHz = 11.7;
	else if(radioconfig.rbw == FSK_BW_19p5) bw_kHz = 19.5;
	else if(radioconfig.rbw == FSK_BW_23p4) bw_kHz = 23.4;
	else if(radioconfig.rbw == FSK_BW_29p3) bw_kHz = 29.3;
	else if(radioconfig.rbw == FSK_BW_39) bw_kHz = 39.0;
	else if(radioconfig.rbw == FSK_BW_46p9) bw_kHz = 46.9;
	else if(radioconfig.rbw == FSK_BW_58p6) bw_kHz = 58.6;
	else if(radioconfig.rbw == FSK_BW_78p2) bw_kHz = 78.2;
	else if(radioconfig.rbw == FSK_BW_93p8) bw_kHz = 93.8;
	else if(radioconfig.rbw == FSK_BW_117p3) bw_kHz = 117.3;
	else if(radioconfig.rbw == FSK_BW_156p2) bw_kHz = 156.2;
	else if(radioconfig.rbw == FSK_BW_187p2) bw_kHz = 187.2;
	else if(radioconfig.rbw == FSK_BW_234p3) bw_kHz = 234.3;
	else if(radioconfig.rbw == FSK_BW_312) bw_kHz = 312.0;
	else if(radioconfig.rbw == FSK_BW_373p6) bw_kHz = 373.6;
	else if(radioconfig.rbw == FSK_BW_467) bw_kHz = 467.0;
	else bw_kHz = -1.0;
	printf("FS_GETMODPARAMS:\r\nBR=%d\r\nSH=%d\r\nBW=%.1f\r\nDEV=%d\r\n",radioconfig.br,radioconfig.shaping,bw_kHz,radioconfig.fdev);
	//printf("FS_GETMODPARAMS: TODO\r\n");
}

void cli_fs_setmodparams(int argc, char **argv)
{
  uint32_t br = ciGetUnsigned(argv[1]);
  uint8_t sh = ciGetUnsigned(argv[2]); //4 - shaping
  uint32_t bw = ciGetUnsigned(argv[3]); //5 - RX BW
  uint32_t dev = ciGetUnsigned(argv[4]);
  float bw_kHz;
  uint8_t bw_val;

	if(br < 600) br = 600;
	if(br > 300000) br = 300000;
	if(dev < 600) dev = 600;
	if(dev > 200000) dev = 200000;
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
	radioconfig.br = br;
	radioconfig.shaping = sh;
	radioconfig.rbw = bw_val;
	radioconfig.fdev = dev;
	radio_config();
	printf("FS_SETMODPARAMS:\r\nBR=%d\r\nSH=%d\r\nBW=%.1f\r\nDEV=%d\r\n",radioconfig.br,radioconfig.shaping,bw_kHz,radioconfig.fdev);
	//printf("FS_SETMODPARAMS: TODO\r\n");
}

//prelen, predetlen,synclen,addr.filt,varlen,paylen,crctype,whitening
void cli_fs_getpacketparams(int argc, char **argv) //bool FskVarLen, bool FskWhiteEn,uint8_t FskCrcType;
{
	uint16_t prelen;
	uint8_t predetlen;
	uint8_t synclen;
	uint8_t paylen;
	uint8_t addrcomp;
	uint8_t varlen;
	uint8_t crc;
	uint8_t white;
	
	prelen = radioconfig.fskprelen;
	predetlen = radioconfig.predetlen;
	synclen = radioconfig.fsksynclen;
	paylen = radioconfig.fskpaylen;
	addrcomp = radioconfig.addrcomp;
	varlen = radioconfig.varlen;
	crc = radioconfig.crctype;
	white = radioconfig.white;

	printf("FS_GET_PACKETPARAMS:\r\nPRE_LEN=%d\r\nPRE_DET=%d\r\nSYNC_LEN=%d\r\n",prelen,predetlen,synclen);
	if(addrcomp == 0) printf("ADDR_COMP:OFF\r\n");
	else if(addrcomp == 1) printf("ADDR_COMP:NODE\r\n");
	else printf("ADDR_COMP:NODE_BR\r\n");
	if(varlen == true) printf("LEN:VAR\r\n"); 
	else printf("LEN:FIXED\r\n");
	printf("PAY_LEN=%d\r\n",paylen);
	switch(crc)
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
if(white == 1) printf("WHITE:ON\r\n"); 
else printf("WHITE:OFF\r\n"); 
//	printf("FS_GET_PACKETPARAMS:TODO\r\n");
}

void cli_fs_setpacketparams(int argc, char **argv)
{
	uint16_t prelen = ciGetUnsigned(argv[1]);
	uint8_t predetlen = ciGetUnsigned(argv[2]);
	uint8_t synclen = ciGetUnsigned(argv[3]);
	uint8_t addrfilt = ciGetUnsigned(argv[4]);
	uint8_t varlen = ciGetUnsigned(argv[5]);
	uint8_t paylen = ciGetUnsigned(argv[6]);
	uint8_t crctype = ciGetUnsigned(argv[7]);
	uint8_t whiteen = ciGetUnsigned(argv[8]);
	
	radioconfig.fskprelen = prelen;
	radioconfig.predetlen = predetlen;
	radioconfig.fsksynclen = synclen;
	radioconfig.addrcomp = addrfilt;
	radioconfig.varlen = varlen;
	radioconfig.fskpaylen = paylen;
	radioconfig.crctype = crctype;
	radioconfig.white = whiteen;
	radio_config();
	printf("FS_SET_PACKETPARAMS:\r\nPRE_LEN=%d\r\nPRE_DET=%d\r\nSYNC_LEN=%d\r\n",prelen,predetlen,synclen);
	if(addrfilt == 0) printf("ADDR_COMP:OFF\r\n");
	else if(addrfilt == 1) printf("ADDR_COMP:NODE\r\n");
	else printf("ADDR_COMP:NODE_BR\r\n");
	if(varlen == 1) printf("LEN:VAR\r\n"); 
	else printf("LEN:FIXED\r\n");
	printf("PAY_LEN=%d\r\n",paylen);
	switch(crctype)
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
	if(whiteen == true) printf("WHITE:ON\r\n"); 
	else printf("WHITE:OFF\r\n"); 
//printf("FS_SET_PACKETPARAMS: TODO\r\n");
}

//node and broadcast addr.
void cli_fs_getaddr(int argc, char **argv)
{
	printf("FS_GET_ADDR: 0x%02X,0x%02X\r\n",radioconfig.nodeaddr,radioconfig.braddr);
//	printf("FS_GET_ADDR: TODO\r\n");
}

void cli_fs_setaddr(int argc, char **argv)
{
	uint8_t nodeaddr = ciGetUnsigned(argv[1]);
	uint8_t braddr = ciGetUnsigned(argv[2]);
	radioconfig.nodeaddr = nodeaddr;
	radioconfig.braddr = braddr;
	radio_config();
	printf("FS_SET_ADDR: 0x%02X,0x%02X\r\n",nodeaddr,braddr);
//	printf("FS_SET_ADDR: TODO\r\n");
}

//sync word(s)
void cli_fs_getsync(int argc, char **argv)
{
//	uint8_t i;
//	printf("FS_GET_SYNC: ");
//		for(i = 0; i < 7; i++) printf("0x%02X,",globalrfconfig.LBconfig.fsksync[i]);
//		printf("0x%02X\r\n",globalrfconfig.LBconfig.fsksync[7]);

  printf("FS_GET_SYNC: TODO\r\n");
}

void cli_fs_setsync(int argc, char **argv)
{
//	uint8_t i;
//	uint8_t radio = ciGetUnsigned(argv[1]);
//	if(radio > 1) 
//	{
//		printf("FS_SET_SYNC: INVALID RADIO\r\n");
//		return;
//	}
//	if(radio == RADIO_L)
//	{
//    for(i = 0; i < 8; i++) globalrfconfig.LBconfig.fsksync[i] = ciGetUnsigned(argv[i+2]);
//    SX126X_config(0,&globalrfconfig.LBconfig);
//    printf("FS_SET_SYNC: ");
//    for(i = 0; i < 7; i++) printf("0x%02X,",globalrfconfig.LBconfig.fsksync[i]);
//    printf("0x%02X\r\n",globalrfconfig.LBconfig.fsksync[7]);
//	}
//	else
//	{
//    for(i = 0; i < 8; i++) globalrfconfig.LBconfig.fsksync[i] = ciGetUnsigned(argv[i+2]);
//    LR112X_config(0,&globalrfconfig.HBconfig);
//    printf("FS_SET_SYNC: ");
//    for(i = 0; i < 7; i++) printf("0x%02X,",globalrfconfig.HBconfig.fsksync[i]);
//    printf("0x%02X\r\n",globalrfconfig.HBconfig.fsksync[7]);
//	}
	printf("FS_SET_SYNC: TODO\r\n");
}
//CRC init, CRC poly, whitening init
void cli_fs_getcrcwhite(int argc, char **argv)
{
	printf("FS_GET_CRCWH:\r\nCRC_INIT=0x%04X\r\nCRC_POLY=0x%04X\r\nWH_INIT=0x%02X\r\n",radioconfig.crcinit,radioconfig.crcpoly,radioconfig.whiteinit);
}

void cli_fs_setcrcwhite(int argc, char **argv)
{
	uint16_t c_init = ciGetUnsigned(argv[1]);
	uint16_t c_poly = ciGetUnsigned(argv[2]);
	uint8_t w_init = ciGetUnsigned(argv[3]);

	radioconfig.crcinit = c_init;
	radioconfig.crcpoly = c_poly;
	radioconfig.whiteinit = w_init;
	radio_config();
	printf("FS_SET_CRCWH: 0\r\nCRC_INIT=0x%04X\r\nCRC_POLY=0x%04X\r\nWH_INIT=0x%02X\r\n",radioconfig.crcinit,radioconfig.crcpoly,radioconfig.whiteinit);
}

//System tests
void cli_sendburst(int argc, char **argv)
{
	uint32_t count = ciGetUnsigned(argv[1]);
	if(count == 0) count = 1;
	uint32_t interval = ciGetUnsigned(argv[2]);
	uint32_t dest = ciGetUnsigned(argv[3]);
	printf("START_TX: ");
	int8_t err = radio_sendburst(dest,count,interval);
	if(err == RADIO_OK) printf("OK\r\n");
	else printerror(err);
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
}


void cli_txstream(int argc, char **argv)
{
	uint8_t stream = ciGetUnsigned(argv[1]);
	int8_t err = radio_txstream(stream);
	printf("TX_STREAM: ");
	if(err == RADIO_OK) printf("%u\r\n",stream);
	else printerror(err);
}

void cli_sweeptx(int argc, char **argv)
{
	uint32_t startfreq = ciGetUnsigned(argv[1]) * 1000;
	uint32_t stopfreq = ciGetUnsigned(argv[2]) * 1000;
	uint32_t step = ciGetUnsigned(argv[3]) * 1000;
	uint32_t us = ciGetUnsigned(argv[4]);
	uint8_t stream = ciGetUnsigned(argv[5]);
	printf("SWEEP_TX: ");
	int8_t err = radio_txsweep(startfreq,stopfreq,step,us,stream);
	if(err == RADIO_OK)
	{
		//check result, on or off
		if(txmode == 0) 
		{
			led_off();
			printf("STOP\r\n");
		}
		else 
		{
			led_on();
			printf("START\r\n");
		}
	}
	else printerror(err);
}

void cli_sweeprx(int argc, char **argv)
{
	uint32_t startfreq = ciGetUnsigned(argv[1]) * 1000;
	uint32_t stopfreq = ciGetUnsigned(argv[2]) * 1000;
	uint32_t step = ciGetUnsigned(argv[3]) * 1000;
	uint32_t ms = ciGetUnsigned(argv[4]);
	float rssitr = (float)ciGetSigned(argv[5]);
	printf("SWEEP_RX: ");
	int8_t err = radio_rxscan(startfreq,stopfreq,step,ms,rssitr);
	if(err == RADIO_OK)
	{
		//check result, on or off
		if(sweeprx == false) 
		{
			led_off();
			printf("STOP\r\n");
		}
		else 
		{
			led_on();
			printf("START\r\n");
		}
	}
	else printerror(err);
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
  //printf("GET_VBAT: %.3f\r\n",Vcc);
	printf("GET_VBAT: TODO\r\n");
}


void cli_readreg(int argc, char **argv)
{
//	uint8_t radio = ciGetUnsigned(argv[1]);
//	uint16_t reg = ciGetUnsigned(argv[2]);
//	printf("READ_REG: 0x%02X\r\n",radio_readregister(radio,reg));
	printf("READ_REG: TODO\r\n");
}

void cli_writereg(int argc, char **argv)
{
//	uint8_t radio = ciGetUnsigned(argv[1]);
//	uint16_t reg = ciGetUnsigned(argv[2]);
//	uint8_t val = ciGetUnsigned(argv[3]) & 0xff;

//  radio_writeregister(radio,reg,val);
//  printf("WRITE_REG: 0x%04X,0x%02X\r\n",reg,val);
	printf("WRITE_REG: TODO\r\n");
}

void cli_dumpregs(int argc, char **argv)
{
//  uint16_t i;
//	uint8_t radio = ciGetUnsigned(argv[1]);
//	uint16_t reg_l = ciGetUnsigned(argv[2]);
//	uint16_t reg_h = ciGetUnsigned(argv[3]);
//	
//  printf("DUMP_REGS:\r\n"); 
//  for(i = reg_l; i <= reg_h; i++)
//  {
//    printf("0x%04X,0x%02X\r\n",i,radio_readregister(radio,i));
//  }
	printf("DUMP_REGS: TODO\r\n");
}

void cli_getstatus(int argc, char **argv)
{
//	uint8_t status = 0;
//	uint8_t mode = 0;
//	uint8_t radio = ciGetUnsigned(argv[1]);
//	
//	if(radio_getstatus(radio,&mode,&status) == true) printf("GET_STATUS: %d,0x%02X,0x%02X\r\n",radio,mode,status);
//	else printf("GET_STATUS: ERROR\r\n");
	printf("GET_STATUS: TODO\r\n");
}

void cli_setopmode(int argc, char **argv)
{
	uint8_t mode = ciGetUnsigned(argv[1]);
	if(radio_setopmode(mode) == RADIO_OK) printf("SET_OPMODE: %d\r\n",mode);
	else printf("SET_OPMODE: ERROR\r\n");
}

void cli_geterrors(int argc, char **argv)
{
//	uint16_t errors = 0;
//	uint8_t status = 0;
//	uint8_t radio = ciGetUnsigned(argv[1]);
//	if(radio_geterrors(radio,&status,&errors) == true) printf("GET_ERRORS: %d,0x%04X\r\n",radio,errors);
//	else printf("GET_ERRORS: ERROR\r\n");
	printf("GET_ERRORS: TODO\r\n");
}

void cli_clearerrors(int argc, char **argv)
{
//	uint8_t radio = ciGetUnsigned(argv[1]);
//	if(radio_clearerrors(radio) == true) printf("CLEAR_ERRORS: OK\r\n");
//	else printf("CLEAR_ERRORS: ERROR\r\n");
	printf("CLEAR_ERRORS: TODO\r\n");
}

void cli_initconfig(int argc, char **argv)
{
	uint16_t chipname = ciGetUnsigned(argv[1]);
	int8_t err = radio_initconfig();
	printf("INIT_CONFIG: ");
	if(err == RADIO_OK) printf("OK\r\n");
	else printerror(err);
}
	

void cli_storeconfig(int argc, char **argv)
{
	writeconfig();
	printf("STORE_CONFIG: ERROR\r\n");
}

void cli_gettemp(int argc, char **argv)
{
//	uint8_t radio = ciGetUnsigned(argv[1]);
//	if(radio != 1)
//	{
//		printf("GET_TEMP: ERROR\r\n");
//	}
//	else
//	{
//		printf("GET_TEMP: 1,%.1f\r\n",LR112X_GetTemp(0));
//	}
	printf("GET_TEMP: TODO\r\n");
}

void cli_getvcc(int argc, char **argv)
{
//	uint8_t radio = ciGetUnsigned(argv[1]);
//	if(radio != 1)
//	{
//		printf("GET_VCC: ERROR\r\n");
//	}
//	else
//	{
//		printf("GET_VCC: 1,%.1f\r\n",LR112X_GetVbat(0));
//	}
	printf("GET_VCC: TODO\r\n");
}

//void cli_getkfreq(int argc, char **argv)
//{
//	uint8_t radio = ciGetUnsigned(argv[1]);
//		
//	if(radio != 1)
//	{
//		printf("GET_KFREQ: ERROR\r\n");
//	}
//	else
//	{
//		printf("GET_KFREQ: 1,%.7f\r\n",globalrfconfig.HBconfig.K_freq);
//	}
//	printf("GET_KFREQ: ERROR\r\n");
//}

//void cli_setkfreq(int argc, char **argv)
//{
//	uint8_t radio = ciGetUnsigned(argv[1]);
//		
//	if(radio != 1)
//	{
//		printf("SET_KFREQ: ERROR\r\n");
//	}
//	else
//	{
//		int16_t ppm = ciGetSigned(argv[2]); //in 0.1 ppm units
//		float k = 1 + ((float)ppm / 10000000.0);
//		globalrfconfig.HBconfig.K_freq = k;
//		radio_config(1);
//		printf("SET_KFREQ: 1,%.7f\r\n",globalrfconfig.HBconfig.K_freq);
//	}
//}

//void cli_gettime(int argc, char **argv)
//{
//	printf("GET_TIME: %d.%02d.%02d,%02d:%02d:%02d\r\n",timestamp.day,timestamp.month,timestamp.year,timestamp.hour,timestamp.minute,timestamp.second);
//}

//void cli_getactivescans(int argc, char **argv)
//{
////	uint8_t s0 = 0;
////	uint8_t s1 = 0;
////	
////	if(globalrfconfig.LBconfig.scan_active == true) s0 = 1;
////	if(globalrfconfig.HBconfig.scan_active == true) s1 = 1;
////	printf("GET_SCANS: %d,%d\r\n",s0,s1);
//	printf("GET_SCANS: TODO\r\n");
//}

//void cli_setactivescans(int argc, char **argv)
//{
////	uint8_t s0 = ciGetUnsigned(argv[1]);
////	uint8_t s1 = ciGetUnsigned(argv[2]);
////	
////	if(s0 != 0)
////	{
////		s0 = 1;
////		globalrfconfig.LBconfig.scan_active = true;
////	}
////	else globalrfconfig.LBconfig.scan_active = false;
////	
////	if(s1 != 0)
////	{
////		s1 = 1;
////		globalrfconfig.HBconfig.scan_active = true;
////	}
////	else globalrfconfig.HBconfig.scan_active = false;
////	printf("SET_SCANS: %d,%d\r\n",s0,s1);
//	printf("SET_SCANS: TODO\r\n");
//}

void cli_RSSIcal(int argc, char **argv)
{
//	float rssi_inst = 0;
//	uint32_t freq = ciGetUnsigned(argv[1]);
//	uint8_t level = ciGetUnsigned(argv[2]);
//	if(level > 16) level = 16;
//	radio_stopscan();
//	
////GainOffset: Global offset added to the Gain Tune values. The offset is a 12-bit signed value, where 1lsb = 0.5dB.
////The power seen by the LR1121 analog front-end is affected by external components such as the matching network, or RF switches. An incorrect RSSI results in a sensitivity degradation in (G)FSK mode and an incorrect gain selection in LoRa and GFSK mode. An incorrect gain can result in a missed detection (packet loss) or decreased resistance to interference.
////By default, the chip is calibrated for the 868-915MHz band on the LR1121 EVK.

////below 600MHz 0  12 12 14 0 1 3 4 4 3 6 6 6 6 6 6 6 6
////from 600MHz to 2GHz 0 2  2  2  3  3  4  5  4  4  6  5  5  6  6  6  7  6
////above 2GHz 2030 6 7 6 4 3 4 14 12 14 12 12 12 12 8 8 9 9

//	uint8_t gain[] = {4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20}; // NB: for the 2G4 path, the max gain is 16 - 17-20 can be ignored.
//	float power[] = {-35.0, -41.0, -45.0, -50.0, -53.5, -60.0, -65.0, -69.5, -75.0, -81.0, -82.5, -83.5, -84.0, -85.0, -86.0, -86.5, -87.0};
//	const LR112X_RssiCalibParams_t caltable = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//	LR112X_SetStandby(0,0);
///* Configure the chip at the system level */ 
//	LR112X_Calibrate(0, 0x3F);  // 0x3F to enable all fields
///* Configure the chip at the modem level */ 
//	LR112X_Calibrate(0,LR112X_PACKET_TYPE_FSK); 
//	LR112X_SetRfFrequency(0, freq * 1000);
//	//LR112X_CalibImage(0,freq/4000000,freq/4000 + 10); //???
//	LR112X_SetFskModulationParams(0,100000,FSK_BT0,FSK_BW_234p3,50000);
//	LR112X_SetFskPacketParams(0,32,FSK_PREDET_32,8,0,0,32,FSK_CRC_OFF,LR112X_FSK_WHITE_OFF);
///* Configure the chip to be controlled manually */
//	LR112X_writeRegMemMask32(0, 0x00F20214, 0x00080000, 0x00080000);
//	LR112X_writeRegMemMask32(0, 0x00F20230, 0x71110000, 0x71100000);
//	LR112X_SetRssiCalibration(0,&caltable);  // All parameters of rssi_cal_table set to 0
//	LR112X_SetRx(0,0xFFFFFF);
//	uint8_t gain_step = gain[level];
//	if(gain_step > 13) gain_step = 13;
//	uint8_t lna_boost = (gain[level] > 13 ) ? gain[level] - 13 : 0;
//	LR112X_writeRegMemMask32(0,0x00F20214, 0x00F00000, gain_step << 20 ); 
//	LR112X_writeRegMemMask32(0,0x00F3008C, 0x00070000, lna_boost << 16 );
/////* Wait for 1 ms */
//	delay_ms(100);
/////* Insert here a control for your test equipment to generate a tone at RF frequency set to freq_in_hz with an output power set to power[i] dBm */
//	rssi_inst = radio_get_rssi(1);
/////* Add a way to log (gain[i], power[i], rssi_in_dbm) triplet to be able to compute offset and
////tunes for the RSSI calibration */
//	printf("RSSI_CAL: %d,%d,%.1f,%.1f\r\n",freq,level,power[level],rssi_inst);
	printf("RSSI_CAL: TODO\r\n");
}

void cli_getrxstats(int argc, char **argv)
{
	rxstats_t stats;
	printf("GET_RXSTATS:\r\n");
	uint8_t err = radio_get_rx_stats(&stats);
	if(err == RADIO_OK)
	{
		printf("RECEIVED:%d,\r\nCRC ERROR:%d,\r\nLEN ERROR:%d\r\n",stats.pkt_received,stats.crc_error,stats.len_header_error);
	}
	else printerror(err);
}

void cli_clrrxstats(int argc, char **argv)
{
	uint8_t err = radio_clear_rx_stats();
	printf("CLR_RXSTATS: ");
	if(err == RADIO_OK) printf("OK\r\n");
	else printerror(err);
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

void cli_poweron(int argc, char **argv)
{
	printf("RADIO_ON: TODO");
}

void cli_poweroff(int argc, char **argv)
{
	printf("RADIO_OFF: TODO\r\n");
}

