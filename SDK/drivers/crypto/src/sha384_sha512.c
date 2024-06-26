
// WARNING: Globals starting with '_' overlap smaller symbols at the same address

uint8_t sha384_512_init_1(SHA512Context *context,uint8_t mode)

{
  uint8_t retval;
  
  if (context == (SHA512Context *)0x0) {
    retval = 1;
  }
  else {
    _DAT_40032004 |= 9;
    _DAT_40032000 = 0x31;
    do {
    } while ((_DAT_40032004 & 1) != 0);
    if (mode == 1) {
      _DAT_4003200c = 3;
    }
    else {
      if (mode != 2) {
        _DAT_40032000 = 0x31;
        _DAT_4003200c = 2;
        return 3;
      }
      _DAT_4003200c = 4;
    }
    memcpy((void *)0x40030000,context,0x40);
    memcpy((void *)0x400300c0,SHA512_K,0x280);
    retval = 0;
  }
  return retval;
}



uint8_t sha512_init(SHA512Context *context)

{
  uint8_t retval;
  
  if (context == (SHA512Context *)0x0) {
    retval = 1;
  }
  else {
    retval = sha384_512_init_1((SHA512Context *)SHA512_V,2);
    if (retval == 0) {
      memset(context,0,0x94);
      retval = 0;
    }
    else {
      retval = 3;
    }
  }
  return retval;
}



bool sha512_plus_bytelen(SHA512Context *context,uint32_t len)

{
  int8_t i;
  
  for (i = 3; (-1 < i &&
              (context->byteLen[i] = context->byteLen[i] + len, context->byteLen[i] < len)); i += -1
      ) {
    context->byteLen[i + -1] = context->byteLen[i + -1] + 1;
  }
  return 0xfffffffc < context->byteLen[0];
}



// WARNING: Globals starting with '_' overlap smaller symbols at the same address

void sha512_processmessageblock(SHA512Context *context)

{
  memcpy((void *)0x40030040,context->Message_Block,0x80);
  _DAT_4003200c |= 0x80;
  do {
  } while (true);
  context->Message_Block_Index = 0;
  return;
}



// WARNING: Globals starting with '_' overlap smaller symbols at the same address

uint8_t sha512_input(SHA512Context *context,uint8_t *message,uint32_t byteLen)

{
  bool bVar1;
  uint8_t retval;
  uint32_t uVar2;
  uint32_t uVar3;
  uint8_t *buf;
  uint16_t local_e;
  uint32_t local_c;
  
  uVar2 = (uint32_t)context->Message_Block_Index;
  if (byteLen == 0) {
    retval = 0;
  }
  else if ((context == (SHA512Context *)0x0) || (message == (uint8_t *)0x0)) {
    retval = 1;
  }
  else if (context->Corrupted == '\0') {
    bVar1 = sha512_plus_bytelen(context,byteLen);
    if (bVar1) {
      context->Corrupted = 2;
      retval = 2;
    }
    else {
      uVar3 = byteLen + uVar2;
      local_c = uVar3 >> 7;
      if (uVar3 < 0x80) {
        memcpy(context->Message_Block + uVar2,message,byteLen);
        context->Message_Block_Index = (byte)uVar3;
      }
      else {
        buf = message;
        if (uVar2 != 0) {
          memcpy(context->Message_Block + uVar2,message,0x80 - uVar2);
          sha512_processmessageblock(context);
          local_c -= 1;
          buf = message + (0x80 - uVar2);
        }
        for (local_e = 0; local_e < local_c; local_e += 1) {
          memcpy((void *)0x40030040,buf,0x80);
          _DAT_4003200c |= 0x80;
          buf = buf + 0x80;
          do {
          } while (true);
        }
        context->Message_Block_Index = (byte)uVar3 & 0x7f;
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



void sha512_padmessage(SHA512Context *context)

{
  uint8_t *buf;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  
  buf = context->Message_Block + context->Message_Block_Index;
  *buf = 0x80;
  if (context->Message_Block_Index < 0x70) {
    memset(buf + 1,0,0x6f - context->Message_Block_Index);
  }
  else {
    memset(buf + 1,0,0x7f - context->Message_Block_Index);
    sha512_processmessageblock(context);
    memset(context->Message_Block,0,0x70);
  }
  context->byteLen[0] = context->byteLen[0] << 3;
  context->byteLen[0] = context->byteLen[0] | context->byteLen[1] >> 0x1d;
  context->byteLen[1] = context->byteLen[1] << 3;
  context->byteLen[1] = context->byteLen[1] | context->byteLen[2] >> 0x1d;
  context->byteLen[2] = context->byteLen[2] << 3;
  context->byteLen[2] = context->byteLen[2] | context->byteLen[3] >> 0x1d;
  context->byteLen[3] = context->byteLen[3] << 3;
  local_24 = CONCAT13((char)context->byteLen[0],
                      CONCAT12((char)(context->byteLen[0] >> 8),
                               CONCAT11((char)(context->byteLen[0] >> 0x10),
                                        (char)(context->byteLen[0] >> 0x18))));
  local_20 = CONCAT13((char)context->byteLen[1],
                      CONCAT12((char)(context->byteLen[1] >> 8),
                               CONCAT11((char)(context->byteLen[1] >> 0x10),
                                        (char)(context->byteLen[1] >> 0x18))));
  local_1c = CONCAT13((char)context->byteLen[2],
                      CONCAT12((char)(context->byteLen[2] >> 8),
                               CONCAT11((char)(context->byteLen[2] >> 0x10),
                                        (char)(context->byteLen[2] >> 0x18))));
  local_18 = CONCAT13((char)context->byteLen[3],
                      CONCAT12((char)(context->byteLen[3] >> 8),
                               CONCAT11((char)(context->byteLen[3] >> 0x10),
                                        (char)(context->byteLen[3] >> 0x18))));
  *(undefined4 *)(context->Message_Block + 0x70) = local_24;
  *(undefined4 *)(context->Message_Block + 0x74) = local_20;
  *(undefined4 *)(context->Message_Block + 0x78) = local_1c;
  *(undefined4 *)(context->Message_Block + 0x7c) = local_18;
  sha512_processmessageblock(context);
  return;
}



// WARNING: Globals starting with '_' overlap smaller symbols at the same address

uint8_t sha512_result(SHA512Context *context,uint8_t *digest)

{
  uint8_t retval;
  
  if ((context == (SHA512Context *)0x0) || (digest == (uint8_t *)0x0)) {
    retval = 1;
  }
  else if (context->Corrupted == 0) {
    sha512_padmessage(context);
    memset(context->Message_Block,0,0x80);
    memset(context,0,0x10);
    memcpy(digest,(void *)0x40030000,0x40);
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



uint8_t sha512_hash(uint8_t *in,uint32_t inByteLen,uint8_t *digest)

{
  SHA512Context context;
  uint8_t retval;
  
  if ((in == (uint8_t *)0x0) || (digest == (uint8_t *)0x0)) {
    retval = 1;
  }
  else {
    retval = sha512_init(&context);
    if (((retval == 0) && (retval = sha512_input(&context,in,inByteLen), retval == 0)) &&
       (retval = sha512_result(&context,digest), retval == 0)) {
      retval = 0;
    }
  }
  return retval;
}



uint8_t sha384_init(SHA384Context *context)

{
  uint8_t retval;
  
  if (context == (SHA384Context *)0x0) {
    retval = 1;
  }
  else {
    retval = sha384_512_init_1((SHA512Context *)SHA384_V,1);
    if (retval == 0) {
      memset(context,0,0x94);
      retval = 0;
    }
    else {
      retval = 3;
    }
  }
  return retval;
}



uint8_t sha384_input(SHA384Context *context,uint8_t *message,uint32_t byteLen)

{
  uint8_t retval;
  
  retval = sha512_input(context,message,byteLen);
  return retval;
}



// WARNING: Globals starting with '_' overlap smaller symbols at the same address

uint8_t sha384_result(SHA384Context *context,uint8_t *digest)

{
  uint8_t retval;
  
  if ((context == (SHA384Context *)0x0) || (digest == (uint8_t *)0x0)) {
    retval = 1;
  }
  else if (context->Corrupted == 0) {
    sha512_padmessage(context);
    memset(context,0,0x10);
    memset(context->Message_Block,0,0x80);
    memcpy(digest,(void *)0x40030000,0x30);
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



uint8_t sha384_hash(uint8_t *in,uint32_t inByteLen,uint8_t *digest)

{
  SHA384Context context;
  uint8_t retval;
  
  if ((in == (uint8_t *)0x0) || (digest == (uint8_t *)0x0)) {
    retval = 1;
  }
  else {
    retval = sha384_init(&context);
    if (((retval == 0) && (retval = sha512_input(&context,in,inByteLen), retval == 0)) &&
       (retval = sha384_result(&context,digest), retval == 0)) {
      retval = 0;
    }
  }
  return retval;
}


