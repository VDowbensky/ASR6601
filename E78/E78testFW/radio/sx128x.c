#include "sx128x_interface.h"

// SX128X_GET_STATUS									0xC0
uint8_t SX128X_GetStatus(void)
{
	uint8_t retval;
	
	SX128X_readCmd(SX128X_GET_STATUS,&retval,1);
	return retval;
}
// SX128X_WRITE_REGISTER							0x18
// SX128X_READ_REGISTER							0x19
// SX128X_WRITE_BUFFER								0x1A
// SX128X_READ_BUFFER								0x1B

// SX128X_SET_SLEEP									0x84
void SX128X_SetSleep(bool retention)
{
	uint8_t buf = 0; 
	if(retention) buf = 1;
	SX128X_writeCmd(SX128X_SET_SLEEP,&buf,1);
}
// SX128X_SET_STANDBY								0x80
void SX128X_SetStandby(uint8_t mode)
{
	uint8_t buf = 0; 
	if(mode) buf = 1;
	SX128X_writeCmd(SX128X_SET_STANDBY,&buf,1);
}
// SX128X_SET_FS											0xC1
void SX128X_SetFs(void)
{
	SX128X_writeCmd(SX128X_SET_FS,NULL,0);
}
// SX128X_SET_TX											0x83
void SX128X_SetTx(uint8_t period_base,uint16_t period_base_cnt)
{
	uint8_t buf[3];
	
	buf[0] = period_base;
	buf[1] = period_base_cnt >> 8;
	buf[2] = period_base_cnt & 0xff;
	SX128X_writeCmd(SX128X_SET_TX,buf,3);
}
// SX128X_SET_RX											0x82
void SX128X_SetRx(uint8_t period_base,uint16_t period_base_cnt)
{
	uint8_t buf[3];
	
	buf[0] = period_base;
	buf[1] = period_base_cnt >> 8;
	buf[2] = period_base_cnt & 0xff;
	SX128X_writeCmd(SX128X_SET_RX,buf,3);
}
// SX128X_SET_RXDUTYCYCLE						0x94
void SX128X_SetRxDutyCycle(uint8_t period_base,uint16_t rx_cnt,uint16_t sleep_cnt)
{
	uint8_t buf[5];
	
	buf[0] = period_base;
	buf[1] = rx_cnt >> 8;
	buf[2] = rx_cnt & 0xff;
	buf[3] = sleep_cnt >> 8;
	buf[4] = sleep_cnt & 0xff;
	SX128X_writeCmd(SX128X_SET_RXDUTYCYCLE,buf,5);
}
// SX128X_SET_CAD										0xC5
void SX128X_SetCad(void)
{
	SX128X_writeCmd(SX128X_SET_CAD,NULL,0);
}
// SX128X_SET_TXCONTINUOUSWAVE				0xD1
void SX128X_SetCW(void)
{
	SX128X_writeCmd(SX128X_SET_TXCONTINUOUSWAVE,NULL,0);
}
// SX128X_SET_TXCONTINUOUSPREAMBLE		0xD2
void SX128X_SetPRE(void)
{
	SX128X_writeCmd(SX128X_SET_TXCONTINUOUSPREAMBLE,NULL,0);
}

// SX128X_SET_PACKETTYPE							0x8A
void SX128X_SetPacketType(uint8_t pkt)
{
	if(pkt > 4) pkt = 4;
	SX128X_writeCmd(SX128X_SET_PACKETTYPE,&pkt,1);
}
// SX128X_GET_PACKETTYPE							0x03
uint8_t SX128X_GetPacketType(void)
{
	uint8_t buf[2];
	
	SX128X_readCmd(SX128X_GET_PACKETTYPE,buf,2);
	return buf[1];
}
// SX128X_SET_RFFREQUENCY						0x86
void SX128X_SetRfFrequency(uint32_t f)
{
	uint8_t buf[3];
	
	buf[0] = (f >> 16) & 0xff;
	buf[1] = (f >> 8) & 0xff;
	buf[2] = f & 0xff;
	SX128X_writeCmd(SX128X_SET_RFFREQUENCY,buf,3);
}
// SX128X_SET_TXPARAMS								0x8E
void SX128X_SetTxParams(uint8_t power,uint8_t ramptime)
{
	uint8_t buf[2];
	
	buf[0] = power;
	buf[1] = ramptime;
	SX128X_writeCmd(SX128X_SET_TXPARAMS,buf,2);
}
// SX128X_SET_CADPARAMS							0x88
void SX128X_SetCadParams(uint8_t cadSymbolNum)
{
	SX128X_writeCmd(SX128X_SET_CADPARAMS,&cadSymbolNum,1);
}
// SX128X_SET_BUFFERBASEADDRESS			0x8F
void SX128X_SetBufferBaseAddress(uint8_t txBaseAdress,uint8_t rxBaseAdress)
{
	uint8_t buf[2];
	
	buf[0] = txBaseAdress;
	buf[1] = rxBaseAdress;
	SX128X_writeCmd(SX128X_SET_BUFFERBASEADDRESS,buf,2);
}
// SX128X_SET_MODULATIONPARAMS				0x8B
void SX128X_SetModulationParams(uint8_t modParam1,uint8_t modParam2,uint8_t modParam3)
{
	uint8_t buf[3];
	
	buf[0] = modParam1;
	buf[1] = modParam2;
	buf[1] = modParam3;
	SX128X_writeCmd(SX128X_SET_MODULATIONPARAMS,buf,3);
}

void SX128X_LORA_SetModulationParams(uint8_t sf,uint8_t bw,uint8_t cr)
{
	//check values
	SX128X_SetModulationParams(sf,bw,cr);
}

void SX128X_FSK_SetModulationParams(uint8_t bitrate,uint8_t modindex,uint8_t shaping)
{
	//check values
	SX128X_SetModulationParams(bitrate,modindex,shaping);
}
	
void SX128X_FLRC_SetModulationParams(uint8_t bitrate,uint8_t cr,uint8_t shaping)
{
	//check values
	SX128X_SetModulationParams(bitrate,cr,shaping);
}

// SX128X_SET_PACKETPARAMS						0x8C
void SX128X_SetPacketParams(uint8_t *params)
{
	SX128X_writeCmd(SX128X_SET_PACKETPARAMS,params,7);
}

void SX128X_LORA_SetPacketParams(uint8_t PreambleLength,uint8_t HeaderType,uint8_t PayloadLength,uint8_t crc,uint8_t invertiq)
{
	uint8_t buf[7];
	//check parameters
	buf[0] = PreambleLength;
	buf[1] = HeaderType;
	buf[2] = PayloadLength;
	buf[3] = crc;
	buf[4] = invertiq;
	buf[5] = buf[6] = 0;
	SX128X_SetPacketParams(buf);
}

void SX128X_BLE_SetPacketParams(uint8_t ConnectionState,uint8_t CrcLength,uint8_t BleTestPayload,uint8_t Whitening)
{
	uint8_t buf[7];
	//check parameters
	buf[0] = ConnectionState;
	buf[1] = CrcLength;
	buf[2] = BleTestPayload;
	buf[3] = Whitening;
	buf[4] = buf[5] = buf[6] = 0;
	SX128X_SetPacketParams(buf);
}

void SX128X_FSK_FLRC_SetPacketParams(uint8_t PreambleLength,uint8_t SyncWordLength,uint8_t SyncWordMatch,uint8_t HeaderType,uint8_t PayloadLength,uint8_t CrcLength,uint8_t Whitening)
{
	uint8_t buf[7];
	//check parameters
	buf[0] = PreambleLength;
	buf[1] = SyncWordLength;
	buf[2] = SyncWordMatch;
	buf[3] = HeaderType;
	buf[4] = PayloadLength;
	buf[5] = CrcLength;
	buf[6] = Whitening;
	SX128X_SetPacketParams(buf);
}
	
// SX128X_GET_RXBUFFERSTATUS					0x17
void SX128X_GetRxBufferStatus(uint8_t* rxPayloadLength,uint8_t* rxStartBufferPointer)
{
	uint8_t buf[3];
	
	SX128X_readCmd(SX128X_GET_RXBUFFERSTATUS,buf,3);
	*rxPayloadLength = buf[1];
	*rxStartBufferPointer = buf[2];
}
// SX128X_GET_PACKETSTATUS						0x1D
void SX128X_GetPacketStatus(uint8_t *status)
{
	SX128X_readCmd(SX128X_GET_PACKETSTATUS,status,6);
}

void SX128X_FSK_BLE_FLRC_GetGetPacketStatus(uint8_t *rssiSync,uint8_t *errors,uint8_t *status,uint8_t *sync)
{
	uint8_t buf[6];
	
	SX128X_GetPacketStatus(buf);
	*rssiSync = buf[2];
	*errors = buf[3] & 0x7f;
	*status = buf[4] & 0x3f;
	*sync = buf[5] & 0x07;
}

void SX128X_LORA_GetGetPacketStatus(uint8_t *rssiSync,int16_t *snrPkt)
{
	uint8_t buf[6];
	
	SX128X_GetPacketStatus(buf);
	*rssiSync = buf[1];
	*snrPkt = (int16_t)buf[2];
}
	
// SX128X_GET_RSSIINST								0x1F
uint8_t SX128X_GetRssiInst(void)
{
	uint8_t buf[2];
	
	SX128X_readCmd(SX128X_GET_RSSIINST,buf,2);
	return buf[1];
}
// SX128X_SET_DIOIRQPARAMS						0x8D
void SX128X_SetDioIrqParams(uint16_t irqMask,uint16_t  dio1Mask,uint16_t  dio2Mask,uint16_t  dio3Mask)
{
	uint8_t buf[8];
	
	buf[0] = irqMask >> 8;
	buf[1] = irqMask & 0xff;
	buf[2] = dio1Mask >> 8;
	buf[3] = dio1Mask & 0xff;
	buf[4] = dio2Mask >> 8;
	buf[5] = dio2Mask & 0xff;
	buf[6] = dio3Mask >> 8;
	buf[7] = dio3Mask & 0xff;
	SX128X_writeCmd(SX128X_SET_DIOIRQPARAMS,buf,8);
}
// SX128X_GET_IRQSTATUS							0x15
uint16_t SX128X_GetIrqStatus(void)
{
	uint8_t buf[3];
	
	SX128X_readCmd(SX128X_GET_IRQSTATUS,buf,3);
	return ((uint16_t)buf[1] << 8) + buf[2];
}
// SX128X_CLR_IRQSTATUS							0x97
void SX128X_ClearIrqStatus(uint16_t mask)
{
	uint8_t buf[2];
	
	buf[0] = mask >> 8;
	buf[1] = mask & 0xff;
	SX128X_writeCmd(SX128X_CLR_IRQSTATUS,buf,2);
}
// SX128X_CALIBRATE									0x89
void SX128X_Calibrate(uint8_t calibParam)
{
	SX128X_writeCmd(SX128X_CALIBRATE,&calibParam,1);
}
// SX128X_SET_REGULATORMODE					0x96
void SX128X_SetRegulatorMode(uint8_t mode)
{
	mode &= 0x01;
	SX128X_writeCmd(SX128X_SET_REGULATORMODE,&mode,1);
}
// SX128X_SET_SAVECONTEXT						0xD5
void SX128X_SetSaveContext(void)
{
	SX128X_writeCmd(SX128X_SET_SAVECONTEXT,NULL,0);
}
// SX128X_SET_AUTOTX									0x98
void SX128X_AutoTx(uint16_t time)
{
	uint8_t buf[2];
	
	buf[0] = time >> 8;
	buf[1] = time & 0xff;
	SX128X_writeCmd(SX128X_SET_RX,buf,2);
}
// SX128X_SET_AUTOFS									0x9E
void SX128X_AutoFs(bool enable)
{
	uint8_t buf = 0;
	if(enable) buf = 1;
	SX128X_writeCmd(SX128X_SET_AUTOFS,&buf,1);
}
// SX128X_SET_LONGPREAMBLE						0x9B
void SX128X_SetLongPreamble(bool enable)
{
	uint8_t value = 0;
	if(enable) value = 1;
	SX128X_writeCmd(SX128X_SET_LONGPREAMBLE,&value,1);
}
// SX128X_SET_UARTSPEED							0x9D
// SX128X_SET_RANGING_ROLE						0xA3
void SX128X_SetRangingRole(uint8_t role)
{
	role &= 0x01;
	SX128X_writeCmd(SX128X_SET_RANGING_ROLE,&role,1);
}
