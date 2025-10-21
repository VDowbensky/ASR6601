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
void cli_getfreq(int argc, char **argv);
void cli_setfreq(int argc, char **argv);

void cli_getpower(int argc, char **argv);
void cli_setpower(int argc, char **argv);

void cli_getctune(int argc, char **argv);
void cli_setctune(int argc, char **argv);

void cli_getRSSI(int argc, char **argv);

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


CommandEntry_t commands[] =
  {
    //general
    COMMAND_ENTRY("RESET", "", cli_reset, "Device reset"),
    COMMAND_ENTRY("GET_UID", "", cli_getuid, "Get CPU ID"),
    COMMAND_ENTRY("GET_VERSION", "", cli_getver, "Get HW/FW version"),
    COMMAND_ENTRY("GET_DEVID", "", cli_getdevid, "Get device ID"),
    COMMAND_ENTRY("SET_DEVID", "w", cli_setdevid, "Get device ID"),
    //RF settings

    COMMAND_ENTRY("GET_FREQ", "", cli_getfreq, "Get frequency settings"),
    COMMAND_ENTRY("SET_FREQ", "w", cli_setfreq, "Set frequency settings"),
    COMMAND_ENTRY("GET_POWER", "", cli_getpower, ""),
    COMMAND_ENTRY("SET_POWER", "w", cli_setpower, ""),
    COMMAND_ENTRY("GET_XOTRIM", "", cli_getctune, ""),
    COMMAND_ENTRY("SET_XOTRIM", "w", cli_setctune, ""),

    COMMAND_ENTRY("GET_RSSI", "", cli_getRSSI, ""),
    
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

    COMMAND_ENTRY("INIT_CONFIG", "", cli_initconfig, ""),
		COMMAND_ENTRY("STORE_CONFIG", "", cli_storeconfig, ""),
		//COMMAND_ENTRY("GET_TIME", "", cli_gettime, ""),
		
		COMMAND_ENTRY("GET_RXSTATS", "", cli_getrxstats, ""),
		COMMAND_ENTRY("CLR_RXSTATS", "", cli_clrrxstats, ""),
		
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

void cli_getfreq(int argc, char **argv)
{
	printf("GET_FREQ: %u\r\n",radioconfig.frequency / 1000);
}

void cli_setfreq(int argc, char **argv)
{
	uint32_t freq = ciGetUnsigned(argv[1]);
	radioconfig.frequency = freq * 1000;
	printf("SET_FREQ: %u\r\n",freq);
}

void cli_getpower(int argc, char **argv)
{
	printf("GET_POWER: %d dBm\r\n",radioconfig.txpower);
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
	uint8_t trim;
	int8_t err = radio_getxotrim(&trim);
	printf("GET_XOTRIM: ");
	if(err == RADIO_OK) printf("%d\r\n",trim);
	else printerror(err); 
}

void cli_setctune(int argc, char **argv)
{
	uint8_t trim = ciGetUnsigned(argv[1]);
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

void cli_getmodparams(int argc, char **argv)
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
 	printf("GET_MODPARAMS:\r\nBW=%.1f\r\nSF=%d\r\n",bw_kHz,sf);
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

void cli_setmodparams(int argc, char **argv)
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
	printf("SET_MODPARAMS: \r\nBW=%.1f\r\nSF=%d\r\n",bw_kHz,sf);
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

void cli_getpacketparams(int argc, char **argv)
{
	printf("GET_PACKETPARAMS: \r\nPRE_LEN=%d\r\nPAY_LEN=%d\r\nHEADER:",radioconfig.loraprelen,radioconfig.lorapaylen);
	if(radioconfig.implheader == 0) printf("EXPLICIT\r\n");
	else printf("IMPLICIT\r\n");
	printf("CRC:");
	if(radioconfig.loracrc == true) printf("ON\r\n");
	else printf("OFF\r\n");
	printf("INVERTIQ:");
	if(radioconfig.invertiq == 0) printf("OFF\r\n");
	else printf("ON\r\n");
}

void cli_setpacketparams(int argc, char **argv)
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
	printf("SET_PACKETPARAMS:\r\nPRE_LEN=%d\r\nPAY_LEN=%d\r\nHEADER:",radioconfig.loraprelen,radioconfig.lorapaylen);
	if(radioconfig.implheader == 1) printf("IMPLICIT\r\n");
	else printf("EXPLICIT\r\n");
	printf("CRC:");
	if(radioconfig.loracrc == 1) printf("ON\r\n");
	else printf("OFF\r\n");
	printf("INVERTIQ:");
	if(radioconfig.invertiq == 1) printf("ON\r\n");
	else printf("OFF\r\n");
}

void cli_getsync(int argc, char **argv)
{
	printf("GET_SYNC: 0x%04X\r\n",radioconfig.lorasync);
}

void cli_setsync(int argc, char **argv)
{
	uint16_t sync = ciGetUnsigned(argv[1]);

	radioconfig.lorasync = sync;
	radio_config();
	printf("SET_SYNC: 0x%04X\r\n",radioconfig.lorasync);
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
	printf("STORE_CONFIG: OK\r\n");
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

void cli_getrxstats(int argc, char **argv)
{
	rxstats_t stats;
	printf("GET_RXSTATS:\r\n");
	uint8_t err = radio_get_rx_stats(&stats);
	if(err == RADIO_OK)
	{
		printf("RECEIVED:%d,\r\nCRC ERROR:%d,\r\nLEN ERROR:%d\r\n",stats.pkt_received,stats.crc_error,stats.header_error);
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

