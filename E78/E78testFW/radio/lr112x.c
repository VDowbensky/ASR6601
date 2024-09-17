#include "lr112x.h"

void LR112X_SetStandby(uint8_t mode) //0 - RC, 1 - XOSC
{
	if(mode != 0) mode = 1;
	LR112X_writeCmd(LR112X_SET_STANDBY,&mode,1);
}

void LR112X_CalibImage(uint8_t f1,uint8_t f2)
{
	uint8_t buf[2];
	
	buf[0] = f1;
	buf[1] = f2;
	LR112X_writeCmd(LR112X_CALIB_IMAGE,buf,2);
}

void LR112X_SetSleep(uint8_t config,uint32_t time)
{
	uint8_t buf[5];
	
	if(config > 3) config = 3;
	buf[0] = config;
	buf[1] = (time >> 24) & 0xff;
	buf[2] = (time >> 16) & 0xff;
	buf[3] = (time >> 8) & 0xff;
	buf[4] = time & 0xff;
	LR112X_writeCmd(LR112X_CALIB_IMAGE,buf,5);
}

void LR112X_Reboot(bool stayinbootloader)
{
	uint8_t b = 0;
	
	if(stayinbootloader) b = 1;
	LR112X_writeCmd(LR112X_REBOOT,&b,1);
}

void LR112X_SetFs(void)
{
	LR112X_writeCmd(LR112X_SET_FS,NULL,0);
}

void LR112X_GetVersion(uint8_t *hw,uint8_t *use_case,uint8_t *fw_major,uint8_t *fw_minor)
{
	uint8_t buf[5];
	
	LR112X_readCmd(LR112X_GET_VERSION,NULL,0,buf,5);
	*hw = buf[1];
	*use_case = buf[2];
	*fw_major = buf[3];
	*fw_minor = buf[4];
}

void LR112X_EraseFlash(void)
{
	LR112X_writeCmd(LR112X_BTL_ERASE_FLASH,NULL,0);
}

void LR112X_WriteFlashEncrypted(uint32_t offset,uint8_t *src,uint16_t len)
{
	uint8_t buf[4];
	uint16_t i;
	
	buf[0] = (offset >> 24) & 0xff;
	buf[1] = (offset >> 16) & 0xff;
	buf[2] = (offset >> 8) & 0xff;
	buf[3] = offset & 0xff;
	if(len > 32) len = 32;
	LR112X_checkBusy();
	LR112X_select();
	LR112X_spi_transfer(LR112X_BTL_WRITE_FLASH_ENC >> 8);
	LR112X_spi_transfer(LR112X_BTL_WRITE_FLASH_ENC & 0xff); //big endian
	for(i = 0; i < 4; i++) LR112X_spi_transfer(buf[i]);
	for(i = 0; i < len*4; i++) LR112X_spi_transfer(src[i]);
	LR112X_deselect();
	LR112X_checkBusy();
}

void LR112X_EraseInfoPage(void)
{
	uint8_t p = 1;
	
	LR112X_writeCmd(LR112X_ERASE_INFO_PAGE,&p,1);
}

void LR112X_WriteInfoPage(uint16_t wordaddr,uint8_t *src,uint16_t len)
{
	uint8_t buf[3];
	uint16_t i;
	
	buf[0] = 1; // Other values are RFU.
	buf[1] = (wordaddr >> 8) & 0xff;
	buf[2] = wordaddr & 0xff;
	if(len > 16) len = 16; //Data length should be a multiple of 4. The maximum value for N is 64.
	LR112X_checkBusy();
	LR112X_select();
	LR112X_spi_transfer(LR112X_WRITE_INFO_PAGE >> 8);
	LR112X_spi_transfer(LR112X_WRITE_INFO_PAGE & 0xff); //big endian
	for(i = 0; i < 3; i++) LR112X_spi_transfer(buf[i]);
	for(i = 0; i < len*4; i++) LR112X_spi_transfer(src[i]);
	LR112X_deselect();
	LR112X_checkBusy();
}

void LR112X_ReadInfoPage(uint16_t wordaddr,uint8_t *dst,uint16_t len)
{
	uint8_t buf[4];
	uint16_t i;
	
	buf[0] = 1;
	buf[1] = (wordaddr >> 8) & 0xff;
	buf[2] = wordaddr & 0xff;
	if(len > 64) len = 64;
	buf[3] = len; //word count
	LR112X_checkBusy();
	LR112X_select();
	LR112X_spi_transfer(LR112X_READ_INFO_PAGE >> 8);
	LR112X_spi_transfer(LR112X_READ_INFO_PAGE & 0xff); //big endian
	for(i = 0; i < 4; i++) LR112X_spi_transfer(buf[i]);
	LR112X_checkBusy();
	for(i = 0; i < len*4; i++) dst[i] = LR112X_spi_transfer(LR112x_NOP);
	LR112X_deselect();
	LR112X_checkBusy();
}

uint32_t LR112X_GetStatus(void) //must be rewritted
{
	uint8_t buf[6];
	uint8_t i;
	
	LR112X_checkBusy();
	LR112X_select();
	buf[0] = LR112X_spi_transfer(LR112X_GET_STATUS >> 8); //
	buf[1] = LR112X_spi_transfer(LR112X_GET_STATUS & 0xff); //big endian
	for(i = 0; i < 4; i++) buf[i+2] = LR112X_spi_transfer(LR112x_NOP);
	return ((uint32_t)buf[2] << 24) + ((uint32_t)buf[3] << 16) + ((uint32_t)buf[4] >> 8) + buf[5];
}

uint16_t LR112X_GetErrors(void)
{
	uint8_t buf[3];
	
	LR112X_readCmd(LR112X_GET_ERRORS,NULL,0,buf,3);
	return ((uint16_t)buf[1] << 8) + buf[2];
}

void LR112X_ClearErrors(void)
{
	LR112X_writeCmd(LR112X_CLEAR_ERRORS,NULL,0);
}

void LR112X_ClearRxBuffer(void)
{
	LR112X_writeCmd(LR112X_CLEAR_RX_BUFFER,NULL,0);
}

uint32_t LR112X_GetRandomNumber(void)
{
	uint8_t buf[5];
	
	LR112X_readCmd(LR112X_GET_RANDOM_NUMBER,NULL,0,buf[5]);
	return ((uint32_t)buf[1] << 24) + ((uint32_t)buf[2] << 16) + ((uint32_t)buf[3] << 8) + buf[4];
}

void LR112X_EnableSpiCrc(bool enable,uint8_t crc)
{
	uint8_t buf[2] = {0,0};
	
	if(enable) buf[0] = 1;
	buf[1] = crc;
	LR112X_writeCmd(LR112X_ENABLE_SPI_CRC,buf,2);
}

void LR112X_SetDioIrqParams(uint32_t irq1,uint32_t irq2) //irq1 - DIO9, irq2 - DIO11
{
	uint8_t buf[8];
	
	buf[0] = (irq1 >> 24) & 0xff;
	buf[1] = (irq1 >> 16) & 0xff;
	buf[2] = (irq1 >> 8) & 0xff;
	buf[3] = irq1 & 0xff;
	buf[4] = (irq2 >> 24) & 0xff;
	buf[5] = (irq2 >> 16) & 0xff;
	buf[6] = (irq2 >> 8) & 0xff;
	buf[7] = irq2 & 0xff;
	LR112X_writeCmd(LR112X_SET_DIO_IRQ_PARAMS,buf,8);
}

void LR112X_ClearIrq(uint32_t mask)
{
	uint8_t buf[4];
	
	buf[0] = (mask >> 24) & 0xff;
	buf[1] = (mask >> 16) & 0xff;
	buf[2] = (mask >> 8) & 0xff;
	buf[3] = mask & 0xff;
	LR112X_writeCmd(LR112X_CLEAR_IRQ,buf,4);
}

void LR112X_SetDioAsRfSwitch(uint8_t enable,uint8_t stby,uint8_t rx,uint8_t tx,uint8_t txhp,uint8_t txhf)
{
	uint8_t buf[7];
	
	if(enable > 0x1f) enable = 0x1f;
	if(stby > 0x1f) stby = 0x1f;
	if(rx > 0x1f) rx = 0x1f;
	if(tx > 0x1f) tx = 0x1f;
	if(txhp > 0x1f) txhp = 0x1f;
	if(txhf > 0x1f) txhf = 0x1f;
	buf[0] = stby;
	buf[1] = rx;
	buf[2] = tx;
	buf[3] = txhp;
	buf[5] = txhf;
	buf[6] = 0;
	buf[7] = 0;
	LR112X_writeCmd(LR112X_SET_DIO_AS_RF_SWITCH,buf,8);
}

void LR112X_DriveDiosInSleepMode(bool enable)
{
	uint8_t b = 0;
	
	if(enable) b = 1;
	LR112X_writeCmd(LR112X_DRIVE_DIOS_IN_SLEEP,&b,1);
}

float LR112X_GetTemp(void)
{
	uint8_t buf[3];
	uint16_t tmp;
	
	LR112X_readCmd(LR112X_GET_TEMP,NULL,0,buf,3);
	tmp = (buf[1] << 8) + buf[2];
	return 25.0 - 588.235 * ((float)tmp/2047*1.35-0.7295);
}

void LR112X_SetRegMode(bool mode)
{
	uint8_t m = 0;
	
	if(mode) m = 1;
	LR112X_writeCmd(LR112X_SET_REG_MODE,&m,1);
}

float LR112X_GetVbat(void)
{
	uint8_t buf[2];
	
	LR112X_readCmd(LR112X_GET_VBAT,NULL,0,buf,1);
	return ((float)buf[1] * 0.0196 - 1) * 1.35;
}

void LR112X_ConfigLfClock(uint8_t clk,bool wait)
{
	if(clk > 2) clk = 2;
	if(wait) clk |= 4;
	LR112X_writeCmd(LR112X_CONFIG_LF_CLOCK,&clk,1);
}

void LR112X_SetTcxoMode(uint8_t voltage,uint32_t delay)
{
	uint8_t buf[4];
	
	if(voltage > 7) voltage = 7;
	if(delay > 0xffffff) delay = 0xffffff;
	buf[0] = voltage;
	buf[1] = (delay >> 16) & 0xff;
	buf[1] = (delay >> 8) & 0xff;
	buf[1] = delay & 0xff;
	LR112X_writeCmd(LR112X_SET_TCXO_MODE,buf,4);
}

void LR112X_SetRfFrequency(uint32_t hz)
{
	uint8_t buf[4];
	
	buf[0] = (hz >> 24) & 0xff;
	buf[1] = (hz >> 16) & 0xff;
	buf[2] = (hz >> 8) & 0xff;
	buf[3] = hz & 0xff;
	LR112X_writeCmd(LR112X_SET_RF_FREQ,buf,4);
}

void LR112X_SetRx(uint32_t timeout)
{
	uint8_t buf[3];
	
	if(timeout > 0xffffff) timeout = 0xffffff;
	buf[0] = (timeout >> 16) & 0xff;
	buf[1] = (timeout >> 8) & 0xff;
	buf[2] = timeout & 0xff;
	LR112X_writeCmd(LR112X_SET_RX,buf,3);
}

void LR112X_SetTx(uint32_t timeout)
{
	uint8_t buf[3];
	
	if(timeout > 0xffffff) timeout = 0xffffff;
	buf[0] = (timeout >> 16) & 0xff;
	buf[1] = (timeout >> 8) & 0xff;
	buf[2] = timeout & 0xff;
	LR112X_writeCmd(LR112X_SET_TX,buf,3);
}

void LR112X_AutoTxRx(uint32_t delay,uint8_t mode,uint32_t timeout)
{
	uint8_t buf[7];
	
	if(delay > 0xffffff) delay = 0xffffff;
	//0x00: Sleep mode.
	//0x01: Standby RC mode.
	//0x02: Standby Xosc mode.
	//0x03: FS mode.
	if(mode > 3) mode = 3;
	if(timeout > 0xffffff) timeout = 0xffffff;
	buf[0] = (delay >> 16) & 0xff;
	buf[1] = (delay >> 8) & 0xff;
	buf[2] = delay & 0xff;
	buf[3] = mode;
	buf[4] = (timeout >> 16) & 0xff;
	buf[5] = (timeout >> 8) & 0xff;
	buf[6] = timeout & 0xff;
	LR112X_writeCmd(LR112X_AUTO_TX_RX,buf,7);
}

void LR112X_SetRxTxFallbackMode(uint8_t mode)
{
	if(mode > 3) mode = 3;
	LR112X_writeCmd(LR112X_SET_RXTX_FALLBACKMODE,&mode,1);
}

void LR112X_SetRxDutyCycle(uint32_t rxperiod,uint32_t sleepperiod,uint8_t mode)
{
	uint8_t buf[7];
	
	if(rxperiod > 0xffffff) rxperiod = 0xffffff;
	if(sleepperiod > 0xffffff) sleepperiod = 0xffffff;
	if(mode > 1) mode = 1;
	buf[0] = (rxperiod >> 16) & 0xff;
	buf[1] = (rxperiod >> 8) & 0xff;
	buf[2] = rxperiod & 0xff;
	buf[3] = (sleepperiod >> 16) & 0xff;
	buf[4] = (sleepperiod >> 8) & 0xff;
	buf[5] = sleepperiod & 0xff;
	buf[6] = mode;
	LR112X_writeCmd(LR112X_SET_RX_DUTY_CYCLE,buf,7);
}

void LR112X_StopTimeoutOnPreamble(bool enable)
{
	uint8_t b = 0; //Stop on Syncword/Header detection (default value).
	
	if(enable) b = 1;
	LR112X_writeCmd(LR112X_STOP_TIMEOUT_ON_PRE,&b,1);
}

float LR112X_GetRssiInst(void)
{
	uint8_t buf[2];
	
	LR112X_readCmd(LR112X_GET_RSSI_INST,NULL,0,buf,2);
	return -((float)buf[1]/2);
}

void LR112X_GetStats(uint16_t *nbpkt,uint16_t *nbcrcerr,uint16_t *data1,uint16_t *data2)
{
	uint8_t buf[9];
	
	LR112X_readCmd(LR112X_GET_STATS,NULL,0,buf,9);
	*nbpkt = ((uint16_t)buf[1] << 8) + buf[2];
	*nbcrcerr = ((uint16_t)buf[3] << 8) + buf[4];
	*data1 = ((uint16_t)buf[5] << 8) + buf[6];
	*data2 = ((uint16_t)buf[7] << 8) + buf[8];
}

void LR112X_ResetStats(void)
{
	LR112X_writeCmd(LR112X_RESET_STATS,NULL,0);
}

void LR112X_GetRxBufferStatus(uint8_t *rxpaylen,uint8_t *rxstartbufferpointer)
{
	uint8_t buf[3];
	
	LR112X_readCmd(LR112X_GET_RXBUFFER_STATUS,NULL,0,buf,3);
	*rxpaylen = buf[1];
	*rxstartbufferpointer = buf[2];
}

void LR112X_SetRxBoosted(bool enable)
{
	uint8_t b = 0;
	
	if(enable) b = 1;
	LR112X_writeCmd(LR112X_SET_RX_BOOSTED,&b,1);
}

void LR112X_SetLoRaSyncWord(uint8_t sync)
{
	LR112X_writeCmd(LR112X_SET_LORA_SYNC_WORD,&b,1);
}

void LR112X_GetLoRaRxHeaderInfos(uint8_t *infos)
{
	uint8_t buf[2];
	
	LR112X_readCmd(LR112X_GET_LORA_HEADER_INFOS,NULL,0,buf,2);
	*infos = buf[1];
}

void LR112X_SetRssiCalibration(uint8_t *tunes,uint16_t gainoffset) //TODO
{
	uint8_t buf[11];
	
	buf[0] = tunes[0] + (tunes[1] << 4); //G4,G5
	buf[1] = tunes[2] + (tunes[3] << 4); //G6,G7
	buf[2] = tunes[4] + (tunes[5] << 4); //G8,G9
	buf[3] = tunes[6] + (tunes[7] << 4); //G10,G11
	buf[4] = tunes[8] + (tunes[9] << 4); //G12,G13
	buf[5] = tunes[10] + (tunes[11] << 4); //G13hp1,G13hp2
	buf[6] = tunes[12] + (tunes[13] << 4); //G13hp3,G13hp4
	buf[7] = tunes[14] + (tunes[15] << 4); //G13hp5,G13hp6
	buf[8] = tunes[16]; //G13hp6
	buf[9] = gainoffset >> 8;
	buf[10] = gainoffset & 0xff;
	LR112X_writeCmd(LR112X_SET_RSSI_CAL,buf,11);
}

void LR112X_SetPacketType(uint8_t type)
{
	
	//0x00: None (default)
	//0x01: (G)FSK
	//0x02: LoRa
	//0x03: Sigfox Uplink1
	//0x04: GMSK (LR-FHSS)
	if(type > 4) type = 4;
	LR112X_writeCmd(LR112X_SET_PACKET_TYPE,&type,1);
}

uint8_t LR112X_ GetPacketType(void)
{
	uint8_t buf[2];
	
	LR112X_readCmd(LR112X_GET_PACKET_TYPE,NULL,0,buf,2);
	return buf[1];
}

void LR112X_SetLoRaModulationParams(uint8_t sf,uint8_t bw,uint8_t cr,bool ldropt) //!!!
{
	uint8_t buf[4];
	
	if(sf < 5) sf = 5;
	if((bw > 0x06) && (bw < 0x0d)) bw = 0x06;
	if(bw > 0x0f) bw = 0x0f;
	if(cr == 0) cr = 1; //???
	if(cr > 7) cr = 7;
	buf[0] = sf;
	buf[1] = bw;
	buf[2] = cr;
	if(ldopt) buf[3] = 1;
	else buf[3] = 0;
	LR112X_writeCmd(LR112X_SET_MOD_PARAMS,buf,4);
}

void LR112X_SetLoRaPacketParams(uint16_t prelen,bool implheader,uint8_t paylen,bool crcen,bool invertiq) //!!!
{
	uint8_t buf[5] = {0,0,0,0,0};	
	buf[0] = prelen >> 8;
	buf[1] = prelen & 0xff;
	if(implheader) buf[2] = 1;
	if(crcen) buf[3] = 1;
	if(invertiq) buf[4] = 1;
	LR112X_writeCmd(LR112X_SET_PACKET_PARAMS,buf,5);
}

void LR112X_SetCad(void)
{
	LR112X_writeCmd(LR112X_SET_CAD,NULL,0);
}

void LR112X_SetCadParams(uint8_t symnum,uint8_t detpeak,uint8_t detmin,uint8_t exitmode,uint32_t timeout)
{
	uint8_t buf[7];
	
	if(mode > 1) mode = 0x10; //CAD_LBT
	if(timeout > 0xffffff) timeout = 0xffffff;
	buf[0] = symnum;
	buf[1] = detpeak;
	buf[2] = detmin;
	buf[3] = exitmode;
	buf[4] = (timeout >> 16) & 0xff;
	buf[5] = (timeout >> 8) & 0xff;
	buf[6] = timeout & 0xff;
	LR112X_writeCmd(LR112X_SET_CAD_PARAMS,buf,7);
}

void LR112X_SetLoRaSynchTimeout(uint8_t symnum)
{
	LR112X_writeCmd(LR112X_SET_LORA_SYNC_TIMEOUT,&symnum,1);
}

void LR112X_SetLoRaPublicNetwork(bool enable)
{
	uint8_t b = 0;
	
	if(enable) b = 1;
	LR112X_writeCmd(LR112X_SET_PUBLIC_NETWORK,&b,1);
}

void LR112X_GetLoRaPacketStatus(float *rssipkt,float *snrpkt,float *signalrssipkt)
{
	uint8_t buf[4];
	
	LR112X_readCmd(LR112X_GET_PACKET_STATUS,NULL,0,buf,4);
	*rssipkt = -((float)buf[1]/2);
	*snrpkt = (((int8_t)buf[2]) + 2 ) / 4.0f; //check
	*signalrssipkt = -((float)buf[2]/2);
}

void LR112X_SetFskModulationParams(uint32_t br,uint8_t shape,uint8_t bw,uint32_t fdevHz)
{
	uint8_t buf[10];
	
	buf[0] = (br >> 24) & 0xff;
	buf[1] = (br >> 16) & 0xff;
	buf[2] = (br >> 8) & 0xff;
	buf[3] = br & 0xff;
	//0x00: No filter applied
	//0x08: Gaussian BT 0.3
	//0x09: Gaussian BT 0.5
	//0x0A: Gaussian BT 0.7
	//0x0B: Gaussian BT 1
	//0x16: Raise Cosine BPSK BT 0.7
	buf[4] = shape;
	buf[5] = bw;
	buf[6] = (fdevHz >> 24) & 0xff;
	buf[7] = (fdevHz >> 16) & 0xff;
	buf[8] = (fdevHz >> 8) & 0xff;
	buf[9] = fdevHz & 0xff;
	LR112X_writeCmd(LR112X_SET_MOD_PARAMS,buf,10);
}

void LR112X_SetFskPacketParams(uint16_t prelen,uint8_t predet,uint8_t synclen,uint8_t addrcomp,uint8_t pkttype,uint8_t paylen,uint8_t crctype,uint8_t dcfree)
{
	uint8_t buf[9];
	
	//TODO: add parameters checking.
	buf[0] = prelen >> 8;
	buf[1] = prelen & 0xff;
	buf[2] = predet;
	buf[3] = synclen;
	buf[4] = addrcomp;
	buf[5] = pkttype;
	buf[6] = paylen;
	buf[7] = crctype;
	buf[8] = dcfree;
	LR112X_writeCmd(LR112X_SET_PACKET_PARAMS,buf,9);
}

void LR112X_SetGfskSyncWord(uint64_t syncword)
{
	uint8_t buf[8];
	
	buf[0] = (syncword >> 56) & 0xff;
	buf[1] = (syncword >> 48) & 0xff;
	buf[2] = (syncword >> 40) & 0xff;
	buf[3] = (syncword >> 32) & 0xff;
	buf[4] = (syncword >> 24) & 0xff;
	buf[5] = (syncword >> 16) & 0xff;
	buf[6] = (syncword >> 8) & 0xff;
	buf[7] = syncword & 0xff;
	LR112X_writeCmd(LR112X_SET_FSK_SYNC,buf,8);
}

void LR112X_SetPacketAdrs(uint8_t ndaddr,uint8_t braddr)
{
	uint8_t buf[2];
	
	buf[0] = ndaddr;
	buf[1] = braddr;
	LR112X_writeCmd(LR112X_SET_FSK_PACKET_ADDR,buf,2);
}

void LR112X_SetGfskCrcParams(uint32_t init,uint32_t poly)
{
	uint32_t buf[8];
	
	buf[0] = (init >> 24) & 0xff;
	buf[1] = (init >> 16) & 0xff;
	buf[2] = (init >> 8) & 0xff;
	buf[3] = init & 0xff;
	buf[4] = (poly >> 24) & 0xff;
	buf[5] = (poly >> 16) & 0xff;
	buf[6] = (poly >> 8) & 0xff;
	buf[7] = poly & 0xff;
	LR112X_writeCmd(LR112X_SET_FSK_CRC_PARAMS,buf,8);
}

void LR112X_SetGfskWhitParams(uint16_t seed)
{
	uint8_t buf[2];
	
	buf[0] = (seed >> 8) & 0xff;
	buf[1] = seed & 0xff;
	LR112X_writeCmd(LR112X_SET_FSK_WHITE_PARAMS,buf,2);
}

void LR112X_GetFskPacketStatus(float *rssisync,float *rssiavg,uint8_t *rxlen,uint8_t *status)
{
	uint8_t buf[5];
	
	LR112X_readCmd(LR112X_GET_PACKET_STATUS,NULL,0,buf,5);
	*rssisync = -(buf[1]/2.0f);
	*rssiavg = -(buf[2]/2.0f); //check!!!
	*rxlen = buf[3];
	*status = buf[4];
}

void LR112X_SetPaConfig(uint8_t pasel,uint8_t ps,uint8_t dutycycle,uint8_t pahpsel)
{
	uint8_t buf[4];
	
	
	//0x00: Selects the low power PA 
	//0x01: Selects the high power PA
	//0x02: Selects the high frequency PA
	if(pasel > 2) pasel = 2;
	if(ps > 1) ps = 1;
	if(dutycycle > 7) dutycycle = 7;
	buf[0] = pasel;
	buf[1] = ps;
	buf[2] = dutycycle;
	buf[3] = pahpsel;//PaHPSel controls the size of the high power PA.
	LR112X_writeCmd(LR112X_SET_PA_CONFIG,buf,4);
}

void LR112X_SetTxParams(int8_t power,uint8_t ramptime)
{
	uint8_t buf[2];

	if(power > 22) power = 22;
	if(ramptime > 0x0f) ramptime = 0x0f;
	buf[0] = (uint8_t)power;
	buf[1] = ramptime;
	LR112X_writeCmd(LR112X_SET_TX_PARAMS,buf,2);
}

uint64_t LR112X_GetChipEui(void)
{
	uint8_t buf[9];
	uint64_t tmp;

	LR112X_writeCmd(LR112X_GET_CHIP_EUI,NULL,0,buf,9);
	tmp = (uint64_t)buf[1] << 56;
	tmp += (uint64_t)buf[2] << 48;
	tmp += (uint64_t)buf[3] << 40;
	tmp += (uint64_t)buf[4] << 32;
	tmp += (uint64_t)buf[5] << 24;
	tmp += (uint64_t)buf[6] << 16;
	tmp += (uint64_t)buf[7] << 8;
	tmp += buf[8];
	return tmp;
}

uint64_t GetSemtechJoinEui(void)
{
	uint8_t buf[9];
	uint64_t tmp;

	LR112X_writeCmd(LR112X_GET_SEMTECH_JOIN_EUI,NULL,0,buf,9);
	tmp = (uint64_t)buf[1] << 56;
	tmp += (uint64_t)buf[2] << 48;
	tmp += (uint64_t)buf[3] << 40;
	tmp += (uint64_t)buf[4] << 32;
	tmp += (uint64_t)buf[5] << 24;
	tmp += (uint64_t)buf[6] << 16;
	tmp += (uint64_t)buf[7] << 8;
	tmp += buf[8];
	return tmp;
}

void LR112X_SetTxCw(void)
{
	LR112X_writeCmd(LR112X_SET_TX_CW,NULL,0);
}

void LR112X_SetTxInfinitePreamble(void)
{
	LR112X_writeCmd(LR112X_SET_TX_INFINITE_PRE,NULL,0);
}