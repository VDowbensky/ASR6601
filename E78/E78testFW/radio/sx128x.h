#ifndef _SX128X_H_
#define _SX128X_H_

#include "bsp.h"

//Commands

#define SX128X_GET_STATUS									0xC0
#define SX128X_WRITE_REGISTER							0x18
#define SX128X_READ_REGISTER							0x19
#define SX128X_WRITE_BUFFER								0x1A
#define SX128X_READ_BUFFER								0x1B
#define SX128X_SET_SLEEP									0x84
#define SX128X_SET_STANDBY								0x80
#define SX128X_SET_FS											0xC1
#define SX128X_SET_TX											0x83
#define SX128X_SET_RX											0x82
#define SX128X_SET_RXDUTYCYCLE						0x94
#define SX128X_SET_CAD										0xC5
#define SX128X_SET_TXCONTINUOUSWAVE				0xD1
#define SX128X_SET_TXCONTINUOUSPREAMBLE		0xD2
#define SX128X_SET_PACKETTYPE							0x8A
#define SX128X_GET_PACKETTYPE							0x03
#define SX128X_SET_RFFREQUENCY						0x86

#define SX128X_SET_TXPARAMS								0x8E
#define SX128X_RAMP_02_US 								0x00 
#define SX128X_RAMP_04_US 								0x20 
#define SX128X_RAMP_06_US 								0x40 
#define SX128X_RAMP_08_US 								0x60 
#define SX128X_RAMP_10_US 								0x80 
#define SX128X_RAMP_12_US 								0xA0 
#define SX128X_RAMP_16_US 								0xC0 
#define SX128X_RAMP_20_US 								0xE0 

#define SX128X_SET_CADPARAMS							0x88
#define SX128X_CAD_01_SYMBOL 							0x00 
#define SX128X_CAD_02_SYMBOLS 						0x20 
#define SX128X_CAD_04_SYMBOLS 						0x40 
#define SX128X_CAD_08_SYMBOLS 						0x60 
#define SX128X_CAD_16_SYMBOLS 						0x80 

#define SX128X_SET_BUFFERBASEADDRESS			0x8F
#define SX128X_SET_MODULATIONPARAMS				0x8B
#define SX128X_SET_PACKETPARAMS						0x8C
#define SX128X_GET_RXBUFFERSTATUS					0x17
#define SX128X_GET_PACKETSTATUS						0x1D
#define SX128X_GET_RSSIINST								0x1F
#define SX128X_SET_DIOIRQPARAMS						0x8D
#define SX128X_GET_IRQSTATUS							0x15
#define SX128X_CLR_IRQSTATUS							0x97
#define SX128X_CALIBRATE									0x89
#define SX128X_SET_REGULATORMODE					0x96
#define SX128X_SET_SAVECONTEXT						0xD5
#define SX128X_SET_AUTOTX									0x98
#define SX128X_SET_AUTOFS									0x9E
#define SX128X_SET_LONGPREAMBLE						0x9B
#define SX128X_SET_UARTSPEED							0x9D
#define SX128X_SET_RANGING_ROLE						0xA3

//registers
#define SX128X_REG_FW_VERSION							0x153 //Firmware versions 0xB7A9 and 0xB5A9 can be read from register 0x0153
#define SX128X_REG_RX_GAIN								0x891 //Register determining the LNA gain regime
#define SX128X_REG_MAN_GAIN								0x895 //Manual Gain
#define SX128X_REG_LNA_GAIN_VAL						0x89e //The LNA gain value
#define SX128X_REG_LNA_GAIN_CTRL					0x89f //Enable/Disable manual LNA gain control
#define SX128X_REG_SYNC_PEAK_ATT					0x8c2 //dB Attenuation of the peak power during synch address.
#define SX128X_REG_PAY_LEN								0x901 //The length of the received LoRa payload
#define SX128X_REG_LR_HDR_MODE						0x903 //LoRa modem header mode
#define SX128X_REG_RRADDR_3								0x912 //Ranging Master: address of the Slave device to which request is sent.
#define SX128X_REG_RRADDR_2								0x913
#define SX128X_REG_RRADDR_1								0x914
#define SX128X_REG_RRADDR_0								0x915
#define SX128X_REG_DEVADDR_3							0x916 //Ranging Address when used in Slave and Advanced Ranging mode.
#define SX128X_REG_DEVADDR_2							0x917
#define SX128X_REG_DEVADDR_1							0x918
#define SX128X_REG_DEVADDR_0							0x919
#define SX128X_REG_RNG_FILT_SIZE					0x91e //The number of ranging samples over which the RSSI evaluated and the results averaged.
#define SX128X_REG_RNG_FLT_RST						0x923 //Clears the samples stored in the ranging filter.
#define SX128X_REG_RNG_RESULT_MUX					0x924 //Ranging result configuration.
#define SX128X_REG_SF_ADD_CONF						0x925 //SF range selection in LoRa mode
#define SX128X_REG_RNG_CAL_2							0x92b //The ranging calibration value
#define SX128X_REG_RNG_CAL_1							0x92c
#define SX128X_REG_RNG_CAL_0							0x92d
#define SX128X_REG_ID_CHK_LEN							0x931 //The number of bytes of the Ranging Slave ID that are checked.
#define SX128X_REG_FREQ_ERR_CORR					0x93c //Crystal frequency error correction mode
#define SX128X_REG_CAD_DET_PEAK						0x942 //Peak-to-noise ratio decision threshold for the CAD
#define SX128X_REG_LR_SYNC_1							0x944 //LoRa sync word value
#define SX128X_REG_LR_SYNC_0							0x945
#define SX128X_REG_CR											0x950 //Coding Rate in LoRa incoming packet
#define SX128X_REG_HEADER_CRC							0x954 //CRC present in LoRa incoming packet
#define SX128X_REG_FEI_2									0x954 //LoRa Frequency error indicator (FEI 16:19)
#define SX128X_REG_FEI_1									0x955 //LoRa Frequency error indicator (FEI 8:15)
#define SX128X_REG_FEI_0									0x956 //LoRa Frequency error indicator (FEI 0:7)
#define SX128X_REG_RNG_RESULT_2						0x961 //The result of the last ranging exchange.
#define SX128X_REG_RNG_RESULT_1						0x962
#define SX128X_REG_RNG_RESULT_0						0x963
#define SX128X_REG_RNG_RSSI								0x964 //The RSSI value of the last ranging exchange
#define SX128X_REG_RNG_RESULT_FREEZE			0x97f //Set bit 1 to preserve the ranging result for reading
#define SX128X_REG_PKT_PRE_SETTINGS				0x9c1 //Preamble length in GFSK and Bluetooth Low Energy compatible.
#define SX128X_REG_WHITE_INIT							0x9c5 //Data whitening seed for GFSK and Bluetooth Low Energy compatible modulation
#define SX128X_REG_CRCPOLY_MSB						0x9c6 //CRC Polynomial Definition for GFSK
#define SX128X_REG_CRCPOLY_LSB						0x9c7
#define SX128X_REG_BT_CRC_SEED_2					0x9c7 //CRC Seed for Bluetooth Low Energy compatible modulation
#define SX128X_REG_BT_CRC_SEED_1					0x9c8 
#define SX128X_REG_BT_CRC_SEED_0					0x9c9
#define SX128X_REG_CRCINIT_MSB						0x9c8 //CRC Seed used for GFSK and FLRC modulation
#define SX128X_REG_CRCINIT_LSB						0x9c9
#define SX128X_REG_SYNC_CTRL							0x9cd //The number of sync word bit errors tolerated in FLRC and GFSK modes
#define SX128X_REG_SYNC_1_4								0x9ce //Sync Word 1 (Also used as the Bluetooth Low Energy compatible Access Address)
#define SX128X_REG_SYNC_1_3								0x9cf
#define SX128X_REG_SYNC_1_2								0x9d0
#define SX128X_REG_SYNC_1_1								0x9d1
#define SX128X_REG_SYNC_1_0								0x9d2
#define SX128X_REG_SYNC_2_4								0x9d3 //Sync Word 2
#define SX128X_REG_SYNC_2_3								0x9d4
#define SX128X_REG_SYNC_2_2								0x9d5
#define SX128X_REG_SYNC_2_1								0x9d6
#define SX128X_REG_SYNC_2_0								0x9d7
#define SX128X_REG_SYNC_3_4								0x9d8 //Sync Word 3
#define SX128X_REG_SYNC_3_3								0x9d9
#define SX128X_REG_SYNC_3_2								0x9da
#define SX128X_REG_SYNC_3_1								0x9db
#define SX128X_REG_SYNC_3_0								0x9dc


//IRQ's
#define SX128X_IRQ_NONE																0x0000
#define SX128X_IRQ_TX_DONE                            0x0001
#define SX128X_IRQ_RX_DONE                            0x0002
#define SX128X_IRQ_SYNCWORD_VALID                     0x0004
#define SX128X_IRQ_SYNCWORD_ERROR                     0x0008
#define SX128X_IRQ_HEADER_VALID                       0x0010
#define SX128X_IRQ_HEADER_ERROR                       0x0020
#define SX128X_IRQ_CRC_ERROR                          0x0040
#define SX128X_IRQ_RANGING_SLAVE_RESPONSE_DONE        0x0080
#define SX128X_IRQ_RANGING_SLAVE_REQUEST_DISCARDED    0x0100
#define SX128X_IRQ_RANGING_MASTER_RESULT_VALID        0x0200
#define SX128X_IRQ_RANGING_MASTER_RESULT_TIMEOUT      0x0400
#define SX128X_IRQ_RANGING_SLAVE_REQUEST_VALID        0x0800
#define SX128X_IRQ_CAD_DONE                           0x1000
#define SX128X_IRQ_CAD_ACTIVITY_DETECTED              0x2000
#define SX128X_IRQ_RX_TX_TIMEOUT                      0x4000
#define SX128X_IRQ_PREAMBLE_DETECTED                  0x8000
#define SX128X_IRQ_ALL																0xFFFF


uint8_t SX128X_GetStatus(void);
void SX128X_SetSleep(bool retention);
void SX128X_SetStandby(uint8_t mode);
void SX128X_SetFs(void);
void SX128X_SetTx(uint8_t period_base,uint16_t period_base_cnt);
void SX128X_SetRx(uint8_t period_base,uint16_t period_base_cnt);
void SX128X_SetRxDutyCycle(uint8_t period_base,uint16_t rx_cnt,uint16_t sleep_cnt);
void SX128X_SetCad(void);
void SX128X_SetCW(void);
void SX128X_SetPRE(void);
void SX128X_SetPacketType(uint8_t pkt);
uint8_t SX128X_GetPacketType(void);
void SX128X_SetRfFrequency(uint32_t f);
void SX128X_SetTxParams(uint8_t power,uint8_t ramptime);
void SX128X_SetCadParams(uint8_t cadSymbolNum);
void SX128X_SetBufferBaseAddress(uint8_t txBaseAdress,uint8_t rxBaseAdress);
void SX128X_SetModulationParams(uint8_t modParam1,uint8_t modParam2,uint8_t modParam3);
void SX128X_LORA_SetModulationParams(uint8_t sf,uint8_t bw,uint8_t cr);
void SX128X_FSK_SetModulationParams(uint8_t bitrate,uint8_t modindex,uint8_t shaping);
void SX128X_FLRC_SetModulationParams(uint8_t bitrate,uint8_t cr,uint8_t shaping);
void SX128X_SetPacketParams(uint8_t *params);
void SX128X_LORA_SetPacketParams(uint8_t PreambleLength,uint8_t HeaderType,uint8_t PayloadLength,uint8_t crc,uint8_t invertiq);
void SX128X_BLE_SetPacketParams(uint8_t ConnectionState,uint8_t CrcLength,uint8_t BleTestPayload,uint8_t Whitening);
void SX128X_FSK_FLRC_SetPacketParams(uint8_t PreambleLength,uint8_t SyncWordLength,uint8_t SyncWordMatch,uint8_t HeaderType,uint8_t PayloadLength,uint8_t CrcLength,uint8_t Whitening);
void SX128X_GetRxBufferStatus(uint8_t* rxPayloadLength,uint8_t* rxStartBufferPointer);
void SX128X_GetPacketStatus(uint8_t *status);
void SX128X_FSK_BLE_FLRC_GetGetPacketStatus(uint8_t *rssiSync,uint8_t *errors,uint8_t *status,uint8_t *sync);
void SX128X_LORA_GetGetPacketStatus(uint8_t *rssiSync,int16_t *snrPkt);
uint8_t SX128X_GetRssiInst(void);
void SX128X_SetDioIrqParams(uint16_t irqMask,uint16_t  dio1Mask,uint16_t  dio2Mask,uint16_t  dio3Mask);
uint16_t SX128X_GetIrqStatus(void);
void SX128X_ClearIrqStatus(uint16_t mask);
void SX128X_Calibrate(uint8_t calibParam);
void SX128X_SetRegulatorMode(uint8_t mode);
void SX128X_SetSaveContext(void);
void SX128X_AutoTx(uint16_t time);
void SX128X_AutoFs(bool enable);
void SX128X_SetLongPreamble(bool enable);
void SX128X_SetRangingRole(uint8_t role);



#endif
