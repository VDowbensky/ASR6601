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

#define SX126X_FREQ_DEFAULT						433125000UL
#define SX126X_TXPOWER_DEFAULT				10
#define SX126X_RAMPTIME_DEFAULT				SX126X_RAMP_10U

#define RADIO_TXBUF_SIZE							256
#define RADIO_RXBUF_SIZE							256

typedef struct
{
  uint32_t device_id;//+3
  uint32_t frequency;//+7
  int8_t txpower;//+8
  uint8_t ramptime;//+9
  //calibration and user parameters - add here
	uint8_t xta;//+10
	uint8_t xtb;//+11
  uint8_t bw;//+12
  uint8_t sf;//+13
  uint8_t cr;//+14
  uint8_t ldropt;//+15
  uint16_t loraprelen;//+17
  uint8_t implheader;//18
	uint8_t reserved0;//+19
  uint16_t lorasync;//+21
  uint8_t lorapaylen;//+22
  uint8_t loracrc;//+23
  uint8_t invertiq;//+24
  uint8_t reserved1[231];//255
}radioconfig_t;

//incoming packet status
typedef struct
{
	float rssi_pkt; //LoRa
	float snr_pkt; //LoRa
	float signal_rssi_pkt; //LoRa
}rxpacketstatus_t;

//statistics
typedef struct
{
	uint16_t pkt_received; 
	uint16_t crc_error; 
	uint16_t header_error; 
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
int8_t radio_getxotrim(uint8_t *trim);
int8_t radio_setxotrim(uint8_t trim);

extern radioconfig_t radioconfig;
extern uint8_t opmode;
extern uint8_t prevopmode;
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
