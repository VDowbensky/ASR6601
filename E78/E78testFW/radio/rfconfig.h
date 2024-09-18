#ifndef _RFCONFIG_H_
#define _RFCONFIG_H_

#include "bsp.h"

typedef struct txconfig
{
  uint8_t modem;

  uint32_t rffreq;
  int8_t txpower;
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
  uint8_t shaping;
  uint32_t fdev;
  uint16_t fskprelen;
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
  uint16_t whiteinit;
}txconfig_t;

typedef struct rxconfig
{
  uint8_t modem;

  uint32_t rffreq;
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
  uint8_t fskpredet;
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
  uint16_t whiteinit;
}rxconfig_t;

void radio_txconfig(void);
void radio_rxconfig(void);

extern txconfig_t txconfig;
extern rxconfig_t rxconfig;

#endif
