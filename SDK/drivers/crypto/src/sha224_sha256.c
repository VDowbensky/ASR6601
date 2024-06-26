
// WARNING: Globals starting with '_' overlap smaller symbols at the same address

uint8_t sha256_init(SHA256Context *context)

{
  uint8_t retval;
  
  if (context == (SHA256Context *)0x0) {
    retval = 1;
  }
  else {
    memset(context,0,0x4c);
    _DAT_40032004 |= 9;
    _DAT_40032000 = 0x31;
    _DAT_4003200c = 2;
    do {
    } while ((_DAT_40032004 & 1) != 0);
    memcpy((void *)0x40031300,SHA256_V,0x10);
    memcpy((void *)0x40031000,SHA256_V + 0x10,0x10);
    memcpy((void *)0x40031310,SHA256_K,0x100);
    retval = 0;
  }
  return retval;
}



// WARNING: Globals starting with '_' overlap smaller symbols at the same address

uint8_t sha224_init(SHA224Context *context)

{
  uint8_t retval;
  
  if (context == (SHA224Context *)0x0) {
    retval = 1;
  }
  else {
    memset(context,0,0x4c);
    _DAT_40032004 |= 9;
    _DAT_40032000 = 0x31;
    _DAT_4003200c = 1;
    do {
    } while ((_DAT_40032004 & 1) != 0);
    memcpy((void *)0x40031300,SHA224_V,0x10);
    memcpy((void *)0x40031000,SHA224_V + 0x10,0x10);
    memcpy((void *)0x40031310,SHA256_K,0x100);
    retval = 0;
  }
  return retval;
}



bool sha256plus_bytelen(SHA256Context *context,uint32_t len)

{
  context->byteLen[1] = context->byteLen[1] + len;
  if (context->byteLen[1] < len) {
    context->byteLen[0] = context->byteLen[0] + 1;
  }
  return 0x1fffffff < context->byteLen[0];
}



// WARNING: Globals starting with '_' overlap smaller symbols at the same address

void sha256processmessageblock(SHA256Context *context)

{
  memcpy((void *)0x40031010,context->Message_Block,0x40);
  _DAT_4003200c |= 0x80;
  do {
  } while (true);
  context->Message_Block_Index = 0;
  return;
}



// WARNING: Globals starting with '_' overlap smaller symbols at the same address

uint8_t sha256_input(SHA256Context *context,uint8_t *message,uint32_t byteLen)

{
  byte bVar1;
  bool bVar2;
  uint8_t retval;
  uint32_t index;
  uint8_t *buf;
  uint local_10;
  uint local_c;
  
  bVar1 = context->Message_Block_Index;
  if (byteLen == 0) {
    retval = 0;
  }
  else if ((context == (SHA256Context *)0x0) || (message == (uint8_t *)0x0)) {
    retval = 1;
  }
  else if (context->Corrupted == 0) {
    bVar2 = sha256plus_bytelen(context,byteLen);
    if (bVar2) {
      context->Corrupted = 1;
      retval = 2;
    }
    else {
      index = bVar1 + byteLen;
      local_c = index >> 6;
      if (index < 0x40) {
        memcpy(context->Message_Block + bVar1,message,byteLen);
        context->Message_Block_Index = (byte)index;
      }
      else {
        buf = message;
        if (bVar1 != 0) {
          memcpy(context->Message_Block + bVar1,message,0x40 - bVar1);
          sha256processmessageblock(context);
          local_c -= 1;
          buf = message + (0x40 - (uint)bVar1);
        }
        for (local_10 = 0; local_10 < local_c; local_10 += 1) {
          memcpy((void *)0x40031010,buf,0x40);
          _DAT_4003200c |= 0x80;
          buf = buf + 0x40;
          do {
          } while (true);
        }
        context->Message_Block_Index = (byte)index & 0x3f;
        if (context->Message_Block_Index != 0) {
          memcpy(context->Message_Block,buf,(uint)context->Message_Block_Index);
        }
      }
      retval = 0;
    }
  }
  else {
    retval = context->Corrupted;
  }
  return retval;
}



void sha256padmessage(SHA256Context *context)

{
  uint8_t *buf;
  uint32_t local_14;
  uint32_t local_10;
  
  buf = context->Message_Block + context->Message_Block_Index;
  *buf = 0x80;
  if (context->Message_Block_Index < 0x38) {
    memset(buf + 1,0,0x37 - context->Message_Block_Index);
  }
  else {
    memset(buf + 1,0,0x3f - context->Message_Block_Index);
    sha256processmessageblock(context);
    memset(context->Message_Block,0,0x38);
  }
  context->byteLen[0] = context->byteLen[0] << 3;
  context->byteLen[0] = context->byteLen[0] | context->byteLen[1] >> 0x1d;
  context->byteLen[1] = context->byteLen[1] << 3;
  local_14 = CONCAT13((char)context->byteLen[0],
                      CONCAT12((char)(context->byteLen[0] >> 8),
                               CONCAT11((char)(context->byteLen[0] >> 0x10),
                                        (char)(context->byteLen[0] >> 0x18))));
  local_10 = CONCAT13((char)context->byteLen[1],
                      CONCAT12((char)(context->byteLen[1] >> 8),
                               CONCAT11((char)(context->byteLen[1] >> 0x10),
                                        (char)(context->byteLen[1] >> 0x18))));
  *(uint32_t *)(context->Message_Block + 0x38) = local_14;
  *(uint32_t *)(context->Message_Block + 0x3c) = local_10;
  sha256processmessageblock(context);
  return;
}



// WARNING: Globals starting with '_' overlap smaller symbols at the same address

uint8_t sha256_result(SHA256Context *context,uint8_t *digest)

{
  uint8_t retval;
  
  if ((context == (SHA256Context *)0x0) || (digest == (uint8_t *)0x0)) {
    retval = 1;
  }
  else if (context->Corrupted == 0) {
    sha256padmessage(context);
    memset(context,0,8);
    memset(context->Message_Block,0,0x40);
    memcpy(digest,(void *)0x40031300,0x10);
    memcpy(digest + 0x10,(void *)0x40031000,0x10);
    do {
    } while (true);
    _DAT_40032004 = _DAT_40032004 & 0xfffffff7 | 1;
    retval = 0;
  }
  else {
    retval = context->Corrupted;
  }
  return retval;
}



uint8_t sha256_hash(uint8_t *in,uint32_t inByteLen,uint8_t *digest)

{
  SHA256Context context;
  uint8_t retval;
  
  retval = sha256_init(&context);
  if (((retval == 0) && (retval = sha256_input(&context,in,inByteLen), retval == 0)) &&
     (retval = sha256_result(&context,digest), retval == 0)) {
    retval = 0;
  }
  return retval;
}



uint8_t sha224_input(SHA224Context *context,uint8_t *message,uint32_t byteLen)

{
  uint8_t retval;
  
  retval = sha256_input(context,message,byteLen);
  return retval;
}



// WARNING: Globals starting with '_' overlap smaller symbols at the same address

uint8_t sha224_result(SHA224Context *context,uint8_t *digest)

{
  uint8_t retval;
  
  if ((context == (SHA224Context *)0x0) || (digest == (uint8_t *)0x0)) {
    retval = 1;
  }
  else if (context->Corrupted == 0) {
    sha256padmessage(context);
    memset(context,0,8);
    memset(context->Message_Block,0,0x40);
    memcpy(digest,(void *)0x40031300,0x10);
    memcpy(digest + 0x10,(void *)0x40031000,0xc);
    do {
    } while (true);
    _DAT_40032004 = _DAT_40032004 & 0xfffffff7 | 1;
    retval = 0;
  }
  else {
    retval = context->Corrupted;
  }
  return retval;
}



uint8_t sha224_hash(uint8_t *in,uint32_t inByteLen,uint8_t *digest)

{
  SHA224Context context;
  uint8_t retval;
  
  retval = sha224_init(&context);
  if (((retval == 0) && (retval = sha256_input(&context,in,inByteLen), retval == 0)) &&
     (retval = sha224_result(&context,digest), retval == 0)) {
    retval = 0;
  }
  return retval;
}


