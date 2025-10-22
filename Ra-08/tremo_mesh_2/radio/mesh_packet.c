#include "mesh_packet.h"

extern meshtastic_pkt_t rxmessage;
extern rxpacketstatus_t pktstatus;

void decode_meshtastic_packet(void)
{
  uint8_t i;
  //packet copied to rxbuffer[]. Lenght of packet (pkt_len) is known from LoRa header.
  rxmessage.destination_id = ((uint32_t)radio_rxbuffer[3] << 24) | ((uint32_t)radio_rxbuffer[2] << 16) | ((uint32_t)radio_rxbuffer[1] << 8) | radio_rxbuffer[0]; //slave_id
  rxmessage.sender_id = ((uint32_t)radio_rxbuffer[7] << 24) | ((uint32_t)radio_rxbuffer[6] << 16) | ((uint32_t)radio_rxbuffer[5] << 8) | radio_rxbuffer[4]; //master_id
  rxmessage.packet_id = ((uint32_t)radio_rxbuffer[11] << 24) | ((uint32_t)radio_rxbuffer[10] << 16) | ((uint32_t)radio_rxbuffer[9] << 8) | radio_rxbuffer[8]; //packetnumber
  rxmessage.flags = radio_rxbuffer[12];
  rxmessage.channel_hash = radio_rxbuffer[13];
  rxmessage.next_hop = radio_rxbuffer[14];
  rxmessage.relay_node = radio_rxbuffer[15];
  for(i = 0; i < rx_paylen-16; i++)
  {
    rxmessage.payload[i] = radio_rxbuffer[i+16]; //237 max.
  }
}

void print_meshtastic_packet(void)
{
  uint8_t i;
  //print rssi, rssi_pkt, snr_pkt first from radio_get_pkt_status()
	if(crc_error) 
	{
		crc_error = false;
		printf("FERR: ");
	}
	else printf("RPCK: ");
  printf("RSSI_PKT=%.1f,RSSI_SIGN=%.1f,SNR=%.1f\r\n",pktstatus.rssi_pkt,pktstatus.signal_rssi_pkt,pktstatus.snr_pkt);
  printf("Destination ID: 0x%08X\r\n",rxmessage.destination_id);
  printf("Sender ID: 0x%08X\r\n",rxmessage.sender_id);
  printf("Packet ID: 0x%08X\r\n",rxmessage.packet_id);
  //flags
  printf("Hop start: %d\r\n",(rxmessage.flags & HOPSTART_MSK) >> HOPSTART_POS);
  printf("Hop limit: %d\r\n",rxmessage.flags & HOPLIMIT_MSK);
  printf("Channel hash: 0x%02X\r\n",rxmessage.channel_hash);
  printf("Relay node: %d\r\n",rxmessage.relay_node);
  printf("Want ack: ");
  if(rxmessage.flags & WANTACK_MSK) printf("Yes\r\n");
  else printf("No\r\n");
    printf("Via MQTT: ");
  if(rxmessage.flags & VIAMQTT_MSK) printf("Yes\r\n");
  else printf("No\r\n");
  printf("Payload: %d\r\n",rx_paylen - 16);
  for(i = 0; i < rx_paylen-16; i++) printf("0x%02X,",rxmessage.payload[i]);
  printf("\r\n");
}
