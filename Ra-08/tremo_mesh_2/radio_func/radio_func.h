#ifndef _RADIO_FUNC_H_
#define _RADIO_FUNC_H_

#include "bsp.h"

//global defines

#define RADIO_OK                0
#define INVALID_CHIP            1
#define RADIO_COMM_FAIL         2
#define RADIO_BUSY              3
#define FEATURE_NOT_SUPPORTED   4
#define RADIO_INVALID_MODE      5
#define RADIO_INVALID_PARAMETER      6
//etc.
#define RADIO_TODO              127

typedef struct radioconfig
{
  uint16_t chip;
  uint32_t id;
  uint32_t freq;
  int8_t txpower;
  //modulation
  uint8_t sf; //spreading factor
  uint8_t bw; //bandwidth
  uint8_t cr; //coding rate
  uint8_t ldropt;
  //packet
  uint16_t sync;
  uint16_t prelen;
  uint8_t header; //0 - explicit,1 - implicit
  uint8_t paylen;
  uint8_t crc;
  uint8_t invertiq;
  uint8_t params[64]; //maybe different
  uint8_t crc;
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

//global functions
int8_t radio_initconfig(uint16_t chip);
int8_t radio_init(void);
int8_t radio_set_freq(uint32_t khz);
int8_t radio_set_power(int8_t dbm);
int8_t radio_setmodparams(uint8_t sf,uint8_t bw,uint8_t cr,uint8_t ldropt);
int8_t radio_setpktparams(uint16_t sync,uint16_t prelen,uint8_t header,uint8_t paylen,uint8_t crc,uint8_t invertiq);
int8_t radio_sendpacket(uint8_t *buf,uint8_t length);
int8_t radio_getpktstatus(rxpacketstatus_t *status);
int8_t radio_getpacket(uint8_t *buf,uint8_t length);
//helpers
int8_t radio_getstats(void);
int8_t radio_clearstats(void);
//irq handler
void radio_irq_handler(void);
//calibrations

//working modes
int8_t radio_rx(void);
int8_t radio_getrssi(float *dbm);
int8_t radio_stream(uint8_t stream);


//global variables
//buffers
extern uint8_t radio_txbuffer[RADIO_TXBUF_SIZE];
extern uint8_t txlen;
extern uint8_t radio_rxbuffer[RADIO_RXBUF_SIZE];
extern uint8_t rxlen;
//radio configuration structure
extern radioconfig_t radioconfig;

#endif
