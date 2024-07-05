#include "aes.h"

uint8_t aes_init(uint8_t *key,uint8_t keymod,uint8_t mode,uint8_t *IVorNonce)
{
  if (((keymod == 16) || (keymod == 24)) || (keymod == 32)) 
  {
    //*(uint32_t*)0x40032004 |= 9;
		SAE->SAEHCR |= 9;
    //*(uint32_t*)0x40032000 = 0x32;
		SAE->SAECR = 0x32;
    //*(uint32_t*)0x40032018 = mode | 0x10;
		SAE->SAEGPR3 = mode | 0x10;
    if (keymod == 32) SAE->SAEGPR0 = AES_KEY256;//*(uint32_t*)0x4003200c = 8;
		else if (keymod < 33) 
	{
      if (keymod == 16) SAE->SAEGPR0 = AES_KEY128; //*(uint32_t*)0x4003200c = 0;
      else if (keymod == 24) SAE->SAEGPR0 = AES_KEY192; //*(uint32_t*)0x4003200c = 4;
    }
    while ((SAE->SAEHCR & 1) != 0); //((*(uint32_t*)0x40032004 & 1) != 0);
    memcpy((void*)0x40031640,key,keymod); //key
    //*(uint32_t*)0x4003200c |= 0x82;
		SAE->SAEGPR0 |= 0x82;
    //while ((*(uint32_t*)0x4003200c & 0x80) != 0);
		while ((SAE->SAEGPR0 & 0x80) != 0);
    //*(uint32_t*)0x4003200c &= 0xfffffffc;
		SAE->SAEGPR0 &= 0xfffffffc;
    //if (((mode == 1) || (mode == 2)) || (mode == 3)) 
		if (((mode == AES_CBC_MODE) || (mode == AES_OFB_MODE)) || (mode == AES_CTR_MODE))
		{
      //memcpy((void*)0x40031610,IVorNonce,0x10); //IV
			memcpy((void*)AES_IVADR,IVorNonce,0x10); //IV
    }
    //if (mode == 3) 
	if (mode == AES_CTR_MODE) 
	{
      //*(uint32_t*)0x40031600 = 0;
			*(uint32_t*)AES_ONEADR = 0;
      *(uint32_t*)0x40031604 = 0;
      *(uint32_t*)0x40031608 = 0;
      *(uint32_t*)0x4003160c = 0x1000000;
		
    }
    return AESSuccess; //0;
  }
  else return AESKeyLenError; //6;
}

uint8_t aes_crypto(uint8_t *in,uint16_t inLen,uint8_t En_De,uint8_t *out)
{
  uint8_t *out_ptr;
  uint16_t i;
  uint8_t *in_ptr;
  
  out_ptr = out;
  in_ptr = in;
  
  //if (En_De == AES_ENC_MODE) 
  //{
  //  Ram4003200c &= 0xfffffffc;
    //uVar2 = Ram4003200c;
    //Ram4003200c = uVar2;
  //}
  //else 
  //{
  //  Ram4003200c &= 0xfffffffc;
  //  Ram4003200c |= 1;
  //}
  //*(uint32_t*)0x4003200c &= 0xfffffffc;
	AESCR &= ~AES_MODE_MASK; //0xfffffffc;
  //if (En_De == AES_DEC_MODE) *(uint32_t*)0x4003200c |= 1;
	if (En_De == AES_DEC_MODE) AESCR |= AES_MODE_DEC; //1;
  for (i = inLen; i != 0; i -= 0x10) 
  {
    //memcpy((void*)0x40031000,in_ptr,0x10); //block to encrypt/decrypt
		memcpy((void*)AES_XADR,in_ptr,0x10); //block to encrypt/decrypt
    //*(uint32_t*)0x4003200c |= 0x80;
		//SAE->SAEGPR0 |= 0x80;
		AESCR |= AES_RUN;
    //while ((SAE->SAEGPR0 & 0x80) != 0); //wait for completing
		while(AESCR & AES_RUN);
		//while ((*(uint32_t*)0x4003200c & 0x80) != 0); //wait for completing
    //memcpy(out_ptr,(void*)0x40031020,0x10); //store result
		memcpy(out_ptr,(void*)AES_YADR,0x10); //store result
    in_ptr = in_ptr + 0x10; //next block
    out_ptr = out_ptr + 0x10;
  }
  return AESSuccess; //0;
}

uint8_t aes_close(void)
{
  //*(uint32_t*)0x40032004 |= 1;
	SAE->SAEHCR |= RAMCLR;
  //while ((*(uint32_t*)0x40032004 & 1) != 0);
	while(SAE->SAEHCR & RAMCLR);
  //*(uint32_t*)0x40032004 &= 0xfffffff7;
	SAE->SAEHCR &= ~SAECLKEN; 
  return AESSuccess; //0;
}


