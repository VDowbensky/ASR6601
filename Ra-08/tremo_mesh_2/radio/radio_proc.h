#ifndef _RADIO_PROC_H_
#define _RADIO_PROC_H_

#include <stdio.h>
#include "bsp.h"
#include "sx126x.h"
#include "ustimer.h"
#include "mesh_packet.h"
#include "sx126x_proc.h"
#include "gui.h"

#define SX126X_SYNTH_STEP ((double)FXO / 33554432)
	
#define RADIO_OK                0
#define INVALID_CHIP            1
#define RADIO_COMM_FAIL         2
#define RADIO_BUSY              3
#define FEATURE_NOT_SUPPORTED   4
#define RADIO_INVALID_MODE      5
#define RADIO_INVALID_PARAMETER      6
//etc.
#define RADIO_TODO              127

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

#define RADIO_BUFFER_SIZE 64
#define TX_PACKET_TEMPLATE {0, 0, 0, 0, -120, 0, 'T', 0, 0, 0, 0, 0, 0, 0, 0,'R', 'A', 'D', 'I', 'O',' ','T','E','S','T',' ',' '}

//HARD events
#define HARDEVENT_TXTIMER                       0x01

#define HIGH(x) ((x & 0xFF000000) >> 24)
#define HIGL(x) ((x & 0x00FF0000) >> 16)
#define LOWH(x) ((x & 0x0000FF00) >> 8)
#define LOWL(x)  (x & 0x000000FF)
	
#define MAGICNUMBER 0xc96c236b

typedef struct SX126XX_radioconfig
{
  uint32_t MagicNumber;
  uint32_t deviceID;
  //Common calibration parameters
  uint8_t CtuneA;
  uint8_t CtuneB;

  uint32_t RfFreq;
  int8_t TxPowerDbm;
  //LoRa
  uint8_t LoRaSf; //spreading factor
  uint8_t LoRaBw; //bandwidth
  uint8_t LoRaCr; //coding rate
  bool LoRaLowRateOpt;
  uint16_t LoRaPreLen;
  bool LoRaImplHeader;
  uint16_t LoRaSyncWord;
  uint8_t LoRaPayLen;
  bool LoRaCrcOn;
  bool LoRaInvertIQ;
	
}SX126XX_radioconfig_t;

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


void radio_init(void);
void radio_proc(void);

void printcrcerror(void);
void prepareTxPacket(void);
void radio_startburst(void);
void radio_delayms(uint32_t ms);
void RADIO_DIO0handler(void);

void radio_set_freq(uint32_t freq);
void radio_rx(void);
void process_rx_packet(void);
int8_t radio_get_pkt_status(rxpacketstatus_t *status);
int8_t radio_get_rx_packet(void);


void RADIO_setctune(uint8_t tune);

extern uint8_t radio_txbuffer[RADIO_TXBUF_SIZE];
extern uint8_t txlen;
extern uint8_t radio_rxbuffer[RADIO_RXBUF_SIZE];
extern uint8_t rx_paylen;

extern SX126XX_radioconfig_t radioConfig;

extern volatile uint32_t txpacketnumber;
extern volatile uint32_t rxpacketnumber;
extern volatile uint32_t txpacketcount;
extern volatile uint32_t rxpacketcount;
extern bool master;
extern uint32_t master_id;
extern uint32_t slave_id;
extern uint32_t inter_packet_delay;
extern volatile uint32_t pkt_timecnt;
extern volatile bool tx_request;
extern bool packet_received;
extern bool crc_error;
extern bool packet_sent;

extern uint8_t txmode;

#endif