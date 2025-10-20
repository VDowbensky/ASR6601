#ifndef _MESH_PACKET_H_
#define _MESH_PACKET_H_

#include "bsp.h"
#include "radio.h"

//Layer 0: LoRa Radio
//All data is converted into LoRa symbols which are sent to the radio for transmission. The details are described elsewhere, but it is worth noting that 
//in addition to the converted packet bytes described below, there is also a preamble sent at the start of any data packet. 
//This preamble allows receiving radios to synchronize clocks and start framing. We use a preamble length of 16, which is longer than the minimum preamble length of 8,
//to let SX126x LoRa receivers sleep for a while, which lowers power consumption.
//After the preamble comes the LoRa Physical Header, which contains information about the packet length as well as a sync word to distinguish networks. 
//For Meshtastic, it is set to 0x2B.
#define MESHTASTIC_PRE_LEN          16
#define  MESHTASTIC_SYNC_8B         0x2b
#define  MESHTASTIC_16B             0x24b4 //not sure
//Index	# of Bits	Usage
//0	3	HopLimit (see note in Layer 3)
#define HOPLIMIT_MSK            0x07
#define HOPLIMIT_POS            0x00
//3	1	WantAck
#define WANTACK_MSK             0x08
#define WANTACK_POS             3
//4	1	ViaMQTT (packet came via MQTT)
#define VIAMQTT_MSK             0x10
#define VIAMQTT_POS             4
//5	3	HopStart (original HopLimit) */
#define HOPSTART_MSK            0xe0
#define HOPSTART_POS            5

typedef struct meshtastic_packet
{
  //0x00	4 bytes	Integer	Packet Header: Destination. The destination's unique NodeID. 0xFFFFFFFF for broadcast. Little Endian.
  uint32_t destination_id; //slave_id
  //0x04	4 bytes	Integer	Packet Header: Sender. The sender's unique NodeID. Little Endian.
  uint32_t sender_id; //master_id
  //0x08	4 bytes	Integer	Packet Header: The sending node's unique packet ID for this packet. Little Endian.
  uint32_t packet_id; //packetnumber
  //0x0C	1 byte	Bits	Packet Header: Flags. 
  uint8_t flags;
  //0x0D	1 byte	Bits	Packet Header: Channel hash. Used as hint for decryption for the receiver.
  uint8_t channel_hash;
  //0x0E	1 byte	Bytes	Packet Header: Next-hop used for relaying.
  uint8_t next_hop;
  //0x0F	1 byte	Bytes	Packet Header: Relay node of the current transmission.
  uint8_t relay_node;
  //0x10	Max. 237 bytes (excl. protobuf overhead)	Bytes	Actual packet data. Unused bytes are not transmitted. */
  uint8_t payload[240]; //237 max.
}meshtastic_pkt_t;

void decode_meshtastic_packet(void);
void print_meshtastic_packet(void);

#endif
