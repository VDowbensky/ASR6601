#ifndef _RADIO_H_
#define _RADIO_H_

#include "bsp.h"
#include "sx126x.h"
#include "sx126x_config.h"
#include "mesh_packet.h"


//Include radiochips definitions here if necessary


#define RADIO_OK                0
#define INVALID_CHIP            1
#define RADIO_COMM_FAIL         2
#define RADIO_BUSY              3
#define FEATURE_NOT_SUPPORTED   4
#define RADIO_INVALID_MODE      5
#define RADIO_INVALID_PARAMETER      6
//etc.
#define RADIO_TODO              127

//Modems definitions
#define MODEM_NONE				0//all (2021 - RAW)
#define MODEM_FSK         1//all (2021 - FSK_LEGACY)
#define MODEM_FSK_GENERIC	2//2021
#define MODEM_GMSK 				3//112x
#define MODEM_BPSK				4//2021,mayse equal to sigfox
#define MODEM_OOK					5//1231,2021
#define MODEM_LORA        6//all
#define MODEM_LR_FHSS     7//1262,2021
#define MODEM_FLRC				8//1280,2021
#define MODEM_OQPSK       9//not used

#define MODEM_RANGING			10//1280,2021
#define MODEM_BLE					11//1280,2021
#define MODEM_SIGFOX			12//112x
#define MODEM_WMBUS				13//2021
#define MODEM_WISUN				14//2021
#define MODEM_ZWAVE				15//2021
#define MODEM_ZIGBEE			16//2021
#define MODEM_INVALID			255

//etc.

#define RADIO_OPMODE_SLEEP						0
#define RADIO_OPMODE_STBYRC						1
#define RADIO_OPMODE_STBYXOSC					2
#define RADIO_OPMODE_FS								3
#define RADIO_OPMODE_TX								4
#define RADIO_OPMODE_RX								5
#define RADIO_OPMODE_TXSTREAMCW				6
#define RADIO_OPMODE_TXSTREAMPRE			7

#define MIN_FREQ_STEP									1
#define MIN_TX_SWEEP_TIME							1
#define MAX_TX_SWEEP_TIME							60000
#define MIN_RX_SWEEP_TIME							10
#define MAX_RX_SWEEP_TIME							60000

#define SX126X_FREQ_DEFAULT						770000000UL
#define SX126X_TXPOWER_DEFAULT				10
#define SX126X_RAMPTIME_DEFAULT				SX126X_RAMP_10U

#define RADIO_TXBUF_SIZE							256
#define RADIO_RXBUF_SIZE							256

typedef struct
{
  uint32_t device_id;//4
  uint8_t chip;//5
  uint8_t modem;//6
  uint8_t work_mode;//7
  uint8_t reserved0;//8
  uint32_t frequency0;//12
  uint32_t frequency1;//16
  uint32_t frequency2;//20
  uint32_t frequency3;//24
  int8_t txpower0;//25
  int8_t txpower1;//26
  int8_t txpower2;//27
  int8_t txpower3;//28
  uint8_t ramptime0;//29
  uint8_t ramptime1;//30
  uint8_t ramptime2;//31
  uint8_t ramptime3;//32
  //calibration and user parameters - add here
  uint8_t user_params[96];//128 XO trim - for SX126x, K_freq for LR1121; AES etc.
  //LORA parameters
  uint8_t bw;//1
  uint8_t sf;//2
  uint8_t cr;//3
  uint8_t ldropt;//4
  uint16_t loraprelen;//5
  uint8_t implheader;//6
  uint16_t lorasync;//8
  uint8_t lorapaylen;//9
  uint8_t loracrc;//10
  uint8_t invertiq;//11
  uint8_t lora_reserved[53];//64
  //FSK parameters
  uint32_t br; //4
  uint8_t rbw;//5
  uint8_t shaping;//6
  uint8_t reserved1[2];//8
  uint32_t fdev;//12
  uint16_t fskprelen;//14
	uint8_t predetlen;//15
  uint8_t fsksynclen;//16
  uint8_t fsksync[16];//32
  uint8_t addrcomp;//33
  uint8_t syncmatch;//34
  uint8_t nodeaddr;//35
  uint8_t braddr;//36 
  uint8_t varlen;//37
  uint8_t fskpaylen;//38
  uint8_t crctype;//39
  uint8_t reserved3;//40
  uint16_t crcinit;//44
  uint16_t crcpoly;//48
  uint8_t white;//49
  uint8_t whiteinit;//50
  uint8_t fsk_reserved[14];//64
  //LR_FHSS
  //OQPSK
  //...
}radioconfig_t;

//incoming packet status
typedef struct
{
	uint8_t rx_status; //FSK
	uint8_t errors; //FSK,1280 only
	float rssi_sync; //FSK
	float rssi_avg; //FSK
	float rssi_pkt; //LoRa
	float snr_pkt; //LoRa
	float signal_rssi_pkt; //LoRa
	//sx1280 packet status
}rxpacketstatus_t;

//statistics
typedef struct
{
	uint16_t pkt_received; 
	uint16_t crc_error; 
	uint16_t len_header_error; 
	uint16_t false_sync;
}rxstats_t;

int8_t radio_initconfig(void);
int8_t radio_init(void);
int8_t radio_config(void);
void radio_proc(void);
int8_t radio_set_frequency(uint32_t freq);
int8_t radio_getrssi(float *rssi);
int8_t radio_getfreqoffset(int32_t *offset);
int8_t radio_txstream(uint8_t stream);
int8_t radio_txsweep(uint32_t start,uint32_t stop,uint32_t step,uint32_t interval,uint8_t stream);
int8_t radio_rxscan(uint32_t startfreq,uint32_t stopfreq,uint32_t step,uint32_t interval,float rssi_threshold);
int8_t radio_setopmode(uint8_t mode);

int8_t radio_setmodem(uint8_t modem);
int8_t radio_setpower(int8_t dbm);
int8_t radio_getxotrim(uint16_t *trim);
int8_t radio_setxotrim(uint16_t trim);

extern radioconfig_t radioconfig;
extern uint8_t txmode;

int8_t radio_fs(void);
int8_t radio_rx(void);

int8_t radio_sendpacket(uint8_t len);
int8_t radio_sendburst(uint32_t dest, uint32_t count,uint32_t interval);
int8_t radio_get_pkt_status(rxpacketstatus_t *status);
int8_t radio_get_rx_packet(void);
int8_t radio_get_rx_stats(rxstats_t *stats);
int8_t radio_clear_rx_stats(void);
void radio_irq_proc(void);

extern volatile bool sweeptx;
extern volatile bool sweeprx;
extern volatile bool sweepflag;
extern volatile uint32_t sweepcnt;
extern uint32_t sweeptime;

extern bool master;
extern volatile bool tx_request;
extern uint32_t inter_packet_delay;
extern volatile uint32_t pkt_timecnt;
extern uint32_t tx_ticks;

extern volatile uint32_t txpacketnumber;
extern volatile uint32_t rxpacketnumber;
extern volatile uint32_t txpacketcount;
extern volatile uint32_t rxpacketcount;

extern uint8_t radio_txbuffer[RADIO_TXBUF_SIZE];
extern uint8_t txlen;
extern uint8_t radio_rxbuffer[RADIO_RXBUF_SIZE];
extern uint8_t rx_paylen;
extern rxpacketstatus_t pktstatus;

#endif
