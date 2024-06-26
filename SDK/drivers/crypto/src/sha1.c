
// WARNING: Globals starting with '_' overlap smaller symbols at the same address

uint8_t sha1_init(SHA1Context *context)

{
  uint8_t retval;
  
  if (context == (SHA1Context *)0x0) {
    retval = 1;
  }
  else {
    memset(context,0,0x4c);
    _DAT_40032004 |= 9;
    _DAT_40032000 = 0x31;
    _DAT_4003200c = 0;
    do {
    } while ((_DAT_40032004 & 1) != 0);
    memcpy((void *)0x40031300,SHA1_V,0x10);
    DAT_40031000 = SHA1_V[0x10];
    DAT_40031000_1._0_1_ = SHA1_V[0x11];
    DAT_40031000_1._1_1_ = SHA1_V[0x12];
    DAT_40031000_1._2_1_ = SHA1_V[0x13];
    DAT_40031310 = SHA1_K[0];
    DAT_40031310_1._0_1_ = SHA1_K[1];
    DAT_40031310_1._1_1_ = SHA1_K[2];
    DAT_40031310_1._2_1_ = SHA1_K[3];
    DAT_40031314 = SHA1_K[4];
    DAT_40031314_1._0_1_ = SHA1_K[5];
    DAT_40031314_1._1_1_ = SHA1_K[6];
    DAT_40031314_1._2_1_ = SHA1_K[7];
    DAT_40031318 = SHA1_K[8];
    DAT_40031318_1._0_1_ = SHA1_K[9];
    DAT_40031318_1._1_1_ = SHA1_K[10];
    DAT_40031318_1._2_1_ = SHA1_K[0xb];
    DAT_4003131c = SHA1_K[0xc];
    DAT_4003131c_1._0_1_ = SHA1_K[0xd];
    DAT_4003131c_1._1_1_ = SHA1_K[0xe];
    DAT_4003131c_1._2_1_ = SHA1_K[0xf];
    retval = 0;
  }
  return retval;
}



bool sha1plus_bytelen(SHA1Context *context,uint32_t len)

{
  context->bitLen[1] = context->bitLen[1] + len;
  if (context->bitLen[1] < len) {
    context->bitLen[0] = context->bitLen[0] + 1;
  }
  return 0x1fffffff < context->bitLen[0];
}



// WARNING: Globals starting with '_' overlap smaller symbols at the same address

void sha1processmessageblock(SHA1Context *context)

{
  memcpy((void *)0x40031010,context->Message_Block,0x40);
  _DAT_4003200c |= 0x80;
  do {
  } while (true);
  context->Message_Block_Index = 0;
  return;
}



// WARNING: Globals starting with '_' overlap smaller symbols at the same address

uint8_t sha1_input(SHA1Context *context,uint8_t *message,uint32_t byteLen)

{
  bool bVar1;
  uint8_t retval;
  uint32_t len;
  astruct *mptr;
  uint32_t i;
  uint32_t cnt;
  uint8_t index;
  
  index = context->Message_Block_Index;
  if (byteLen == 0) {
    retval = 0;
  }
  else if ((context == (SHA1Context *)0x0) || (message == (uint8_t *)0x0)) {
    retval = 1;
  }
  else if (context->Corrupted == 0) {
    bVar1 = sha1plus_bytelen(context,byteLen);
    if (bVar1) {
      context->Corrupted = 1;
      retval = 2;
    }
    else {
      len = index + byteLen;
      cnt = len >> 6;
      if (len < 0x40) {
        memcpy(context->Message_Block + index,message,byteLen);
        context->Message_Block_Index = (byte)len;
      }
      else {
        mptr = (astruct *)message;
        if (index != 0) {
          memcpy(context->Message_Block + index,message,0x40 - index);
          sha1processmessageblock(context);
          cnt -= 1;
          mptr = (astruct *)(message + (0x40 - (uint)index));
        }
        for (i = 0; i < cnt; i += 1) {
          memcpy((void *)0x40031010,mptr,0x40);
          _DAT_4003200c |= 0x80;
          mptr = mptr + 1;
          do {
          } while (true);
        }
        context->Message_Block_Index = (byte)len & 0x3f;
        if (context->Message_Block_Index != 0) {
          memcpy(context->Message_Block,mptr,(uint)context->Message_Block_Index);
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



void sha1padmessage(SHA1Context *context)

{
  U8 *pUVar1;
  undefined4 local_14;
  undefined4 local_10;
  
  pUVar1 = context->Message_Block + context->Message_Block_Index;
  *pUVar1 = 0x80;
  if (context->Message_Block_Index < 0x38) {
    memset(pUVar1 + 1,0,0x37 - context->Message_Block_Index);
  }
  else {
    memset(pUVar1 + 1,0,0x3f - context->Message_Block_Index);
    sha1processmessageblock(context);
    memset(context->Message_Block,0,0x38);
  }
  context->bitLen[0] = context->bitLen[0] << 3;
  context->bitLen[0] = context->bitLen[0] | context->bitLen[1] >> 0x1d;
  context->bitLen[1] = context->bitLen[1] << 3;
  local_14 = CONCAT13((char)context->bitLen[0],
                      CONCAT12((char)(context->bitLen[0] >> 8),
                               CONCAT11((char)(context->bitLen[0] >> 0x10),
                                        (char)(context->bitLen[0] >> 0x18))));
  local_10 = CONCAT13((char)context->bitLen[1],
                      CONCAT12((char)(context->bitLen[1] >> 8),
                               CONCAT11((char)(context->bitLen[1] >> 0x10),
                                        (char)(context->bitLen[1] >> 0x18))));
  *(undefined4 *)(context->Message_Block + 0x38) = local_14;
  *(undefined4 *)(context->Message_Block + 0x3c) = local_10;
  sha1processmessageblock(context);
  return;
}



// WARNING: Globals starting with '_' overlap smaller symbols at the same address

uint8_t sha1_result(SHA1Context *context,uint8_t *digest)

{
  uint8_t retval;
  
  if ((context == (SHA1Context *)0x0) || (digest == (uint8_t *)0x0)) {
    retval = 1;
  }
  else if (context->Corrupted == 0) {
    sha1padmessage(context);
    memset(context,0,8);
    memset(context->Message_Block,0,0x40);
    memcpy(digest,(void *)0x40031300,0x10);
    *(undefined4 *)(digest + 0x10) = _DAT_40031000;
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



uint8_t sha1_hash(uint8_t *in,uint32_t inByteLen,uint8_t *digest)

{
  SHA1Context context;
  uint8_t retval;
  
  retval = sha1_init(&context);
  if (((retval == 0) && (retval = sha1_input(&context,in,inByteLen), retval == 0)) &&
     (retval = sha1_result(&context,digest), retval == 0)) {
    retval = 0;
  }
  return retval;
}


