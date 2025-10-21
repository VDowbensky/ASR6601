#include "aes_proc.h"


void aes_proc_init(void)
{
	rcc_enable_peripheral_clk(RCC_PERIPHERAL_SAC, true);
	#if AES_MODE == AES_ECB_MODE
	aes_init(radioConfig.AesKey, AES_KEYLEN, AES_ECB_MODE, NULL);
	
	#elif AES_MODE == AES_CBC_MODE
	aes_init(radioConfig.AesKey, AES_KEYLEN,AES_CBC_MODE,radioConfig.AesIv);
	
	//#elif AES_MODE == AES_OFB_MODE
	//aes_init(radioConfig.AesKey, AES_KEYLEN,AES_OFB_MODE,radioConfig.AesIv);
	
	#else
	aes_init(radioConfig.AesKey, AES_KEYLEN,AES_CTR_MODE,radioConfig.AesIv);
	#endif
}

uint8_t aes_encrypt(uint8_t *buf, uint16_t len)
{
	return aes_crypto(buf, len, AES_ENC_MODE, buf);
}

uint8_t aes_decrypt(uint8_t *buf, uint16_t len)
{
	return aes_crypto(buf, len, AES_DEC_MODE, buf);
}