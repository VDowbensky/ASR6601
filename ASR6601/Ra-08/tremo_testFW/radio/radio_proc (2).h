#ifndef _RADIO_PROC_H_
#define _RADIO_PROC_H_

#include <stdio.h>
#include "stm8l15x.h"
#include "sx126xx.h"
#include "aes.h"
//#include "aes_stm8.h"
#include "gpsparser.h"
#include "timers.h"

#define RADIO_BUFFER_SIZE 64
#define TX_PACKET_TEMPLATE {0, 0, 0, 0, -120, 0, 'T', 0, 0, 0, 0, 0, 0, 0, 0,'R', 'A', 'D', 'I', 'O',' ','T','E','S','T',' ',' '}

//HARD events
#define HARDEVENT_TXTIMER                       0x01

#define HIGH(x) ((x & 0xFF000000) >> 24)
#define HIGL(x) ((x & 0x00FF0000) >> 16)
#define LOWH(x) ((x & 0x0000FF00) >> 8)
#define LOWL(x)  (x & 0x000000FF)


typedef struct rfpacket
{
    uint16_t masterid; //2
    uint16_t slaveid; //4
    uint16_t packetnumber; //6
    uint8_t hour;//7
    uint8_t min;//8
    uint8_t sec;//9
    uint8_t status;//10
    float latitude; //14
    float longitude; //18
    float alt; //22
    uint8_t sat;//23
    int8_t temp;
    uint8_t params[8];
}rfpacket_t;


void init_radio(void);
void process_radio(void);
void printRxPacket(void);
void printcrcerror(void);
void prepareTxPacket(void);
void prepareAnswerPacket(void);
void preparepacketcommon(void);
void radio_startburst(void);
void radio_delayms(uint32_t ms);
void RADIO_DIO0handler(void);

void RADIO_setctune(uint8_t tune);
void RADIO_ctunecorrection(void);

extern uint8_t RADIO_rxBuffer[];
extern uint8_t RADIO_txBuffer[];

extern volatile uint16_t txpacketnumber;
extern volatile uint16_t rxpacketnumber;
extern volatile uint16_t txpacketcount;
extern volatile uint16_t rxpacketcount;

extern bool master;
extern bool tx_needed;
extern uint16_t inter_packet_delay;
extern bool answer_needed;
extern bool packet_received;
extern bool packet_sent;

extern uint8_t txmode;
extern int16_t Channel;

extern uint16_t slaveID;
extern uint16_t masterID;
extern int16_t Rssi;

extern rfpacket_t rfpacket;

extern uint8_t minchan;
extern uint8_t maxchan;
extern volatile bool sweeptx;
extern volatile bool sweeprx;
extern uint32_t sweepdelay;

#endif