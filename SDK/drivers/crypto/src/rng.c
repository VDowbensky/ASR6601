
#include "rng.h"

void rng_init(uint8_t div,uint8_t mode)

{
  RNG->RNGCLK = div | RNGCLKEN;
  RNG->RNGCR = mode | RNGEN;
}

void rng_close(void)

{
  RNG->RNGCR &= ~RNGEN;
  RNG->RNGCLK &= ~RNGCLKEN;
}

void rng_get_rand(uint8_t *buf,uint16_t len)

{
  uint16_t i;
  uint8_t *ptr;
  
  i = len;
  ptr = buf;
  for(i = len; i != 0; i--) 	  
  {
	while (RNG->RNGSR & DATARDY);
	*ptr = (uint8_t)RNG->RNGDATA;
    ptr++;
  }
}


