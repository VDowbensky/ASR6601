#ifndef _SX126X_CONFIG_H_
#define _SX126X_CONFIG_H_

#include "bsp.h"

#define SX126X_SYNTH_STEP ((double)FXO / 33554432)
	
typedef struct 
{
  uint8_t modem;

  uint32_t startfreq;
	uint32_t stopfreq;
	uint32_t freqstep;
	int8_t rssithreshold;
	int16_t scantimems;
	bool scan_active;
	
  int8_t txpower;
  uint8_t ramptime;
  //LoRa
  uint8_t sf; //spreading factor
  uint8_t bw; //bandwidth
  uint8_t cr; //coding rate
  bool opt;
  uint16_t loraprelen;
  bool header;
  uint16_t lorasync;
  uint8_t lorapaylen;
  bool loracrc;
  bool invertiq;
  //FSK parameters
  uint32_t br; //baud rate
  uint8_t rbw;
  uint8_t shaping;
  uint32_t fdev;
  uint16_t fskprelen;
	uint8_t predetlen;
  uint8_t fsksynclen;
  uint8_t fsksync[8];
  uint8_t addrcomp;
  uint8_t nodeaddr;
  uint8_t braddr; 
  bool varlen;
  uint8_t fskpaylen;
  uint8_t crctype;
  uint16_t crcinit;
  uint16_t crcpoly;
  bool white;
  uint8_t whiteinit;
}SX126Xconfig_t;

int8_t SX126X_config(void);
void SX126X_LoRaConfig(void);
void SX126X_FskConfig(void);
void SX126X_setopmode(uint8_t mode);

#endif
