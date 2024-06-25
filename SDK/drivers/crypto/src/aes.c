
uint8_t aes_init(uint8_t *key,uint8_t keymod,uint8_t mode,uint8_t *IVorNonce)

{
  uint8_t retval;
  
  if (((keymod == 16) || (keymod == 24)) || (keymod == 32)) {
    Ram40032004 |= 9;
    Ram40032000 = 0x32;
    Ram40032018 = (uint)(mode | 16);
    if (keymod == 32) Ram4003200c = 8;
    else if (keymod < 33) 
	{
      if (keymod == 16) Ram4003200c = 0;
      else if (keymod == 24) Ram4003200c = 4;
    }
    while ((Ram40032004 & 1) != 0);
    memcpy(&UNK_40031640,key,(uint)keymod);
    Ram4003200c |= 0x82;
    while ((Ram4003200c & 0x80) != 0);
    Ram4003200c &= 0xfffffffc;
    if (((mode == 1) || (mode == 2)) || (mode == 3)) 
	{
      memcpy(&UNK_40031610,IVorNonce,0x10);
    }
    if (mode == 3) 
	{
      Ram40031600 = 0;
      Ram40031604 = 0;
      Ram40031608 = 0;
      Ram4003160c = 0x1000000;
    }
    retval = 0;
  }
  else retval = 6;
  return retval;
}



uint8_t aes_crypto(uint8_t *in,uint16_t inLen,uint8_t En_De,uint8_t *out)

{
  uint uVar1;
  undefined4 uVar2;
  uint8_t *local_14;
  uint16_t local_e;
  uint8_t *local_c;
  
  local_14 = out;
  local_e = inLen;
  local_c = in;
  if (En_De == 0) 
  {
    Ram4003200c &= 0xfffffffc;
    uVar2 = Ram4003200c;
    Ram4003200c = uVar2;
  }
  else 
  {
    Ram4003200c &= 0xfffffffc;
    Ram4003200c |= 1;
  }
  for (; local_e != 0; local_e -= 0x10) 
  {
    memcpy(&UNK_40031000,local_c,0x10);
    Ram4003200c |= 0x80;
    while ((Ram4003200c & 0x80) != 0);
    memcpy(local_14,&UNK_40031020,0x10);
    local_c = local_c + 0x10;
    local_14 = local_14 + 0x10;
  }
  return 0;
}



uint8_t aes_close(void)

{
  Ram40032004 |= 1;
  while ((Ram40032004 & 1) != 0);
  Ram40032004 &= 0xfffffff7;
  return 0;
}


