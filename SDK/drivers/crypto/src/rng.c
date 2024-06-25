
// WARNING: Globals starting with '_' overlap smaller symbols at the same address

void rng_init(uint8_t div,uint8_t mode)

{
  _DAT_40033008 = div | 0x80;
  _DAT_4003300c = mode | 0x80;
}



// WARNING: Globals starting with '_' overlap smaller symbols at the same address

void rng_close(void)

{
  _DAT_4003300c = _DAT_4003300c & 0xffffff7f;
  _DAT_40033008 = _DAT_40033008 & 0xffffff7f;
}



// WARNING: Globals starting with '_' overlap smaller symbols at the same address

void rng_get_rand(uint8_t *buf,uint16_t len)

{
  uint16_t i;
  uint8_t *ptr;
  
  i = len;
  ptr = buf;
  //while (i != 0)
  for(i = len; i != 0; i--) 	  
  {
    while ((_DAT_40033004 & 1) == 0);
    *ptr = (uint8_t)_DAT_40033014;
    ptr++;
  }
}


