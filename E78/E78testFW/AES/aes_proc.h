#ifndef _AES_PROC_H_
#define _AES_PROC_H_

#include "bsp.h"
#include "aes.h"
#include "radio_proc.h"

void aes_proc_init(void);
uint8_t aes_encrypt(uint8_t *buf, uint16_t len);
uint8_t aes_decrypt(uint8_t *buf, uint16_t len);

#endif // _AES_PROC_H_

