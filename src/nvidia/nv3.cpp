#include "nv3.h"

#define BYTESEL_R(addr,val) ((val >> ((addr & 3) << 1)) & 0xFF)
#define BYTESEL_W(dst,addr,val) dst = (dst & (0xFF << ((addr & 3) << 1))) | val
#define BYTESEL_W64L(dst,addr,val) dst = (dst & 0xFFFFFFFF00000000) | BYTESEL_W(dst & 0xFFFFFFFF,addr,val)
#define BYTESEL_W64H(dst,addr,val) dst = (dst & 0XFFFFFFFF) | (BYTESEL_W(dst >> 32,addr,val) << 32)

u8 nv3_device::bar0_rb(u32 addr)
{
  switch(addr)
  {
  case 0x000000 ... 0x000003:
  {
    //PMC - ID
    return BYTESEL_R(addr,0x00030100); //Early NV3
    break;
  }
  case 0x000200 ... 0x000203:
  {
    return BYTESEL_R(addr,pmc.enable);
    break;
  }
  case 0x009200 ... 0x009203: 
  {
    return BYTESEL_R(addr,ptimer.clkdiv);
    break;
  }
  case 0x009210 ... 0x009213: 
  {
    return BYTESEL_R(addr,ptimer.clkmul);
    break;
  }
  case 0x009400 ... 0x009403: 
  {
    return BYTESEL_R(addr,ptimer.time & 0xFFFFFFFF);
    break;
  }
  case 0x009410 ... 0x009413: 
  {
    return BYTESEL_R(addr,ptimer.time >> 32);
    break;
  }
  case 0x009420 ... 0x009423: 
  {
    return BYTESEL_R(addr,ptimer.alarm);
    break;
  }
  case 0x100000 ... 0x100003:
  {
    return BYTESEL_R(addr,0x00000006);
    break;
  }
  case 0x100200 ... 0x100203:
  {
    return BYTESEL_R(addr,pfb.config_0);
    break;
  }
  default:
  {
    return 0;
    break;
  }
  }
}

void nv3_device::bar0_wb(u32 addr, u8 data)
{
  switch(addr)
  {
  case 0x000200 ... 0x000203:
  {
    BYTESEL_W(pmc.enable,addr,data);
    break;
  }
  case 0x002210 ... 0x002213:
  {
    BYTESEL_W(pfifo.ramht,addr,data);
    break;
  }
  case 0x009200 ... 0x009203:
  {
    BYTESEL_W(ptimer.clkdiv,addr,data);
    break;
  }
  case 0x009210 ... 0x009213:
  {
    BYTESEL_W(ptimer.clkmul,addr,data);
    break;
  }
  case 0x009400 ... 0x009403:
  {
    BYTESEL_W64L(ptimer.time,addr,data);
    break;
  }
  case 0x009410 ... 0x009413:
  {
    BYTESEL_W64H(ptimer.time,addr,data);
    break;
  }
  case 0x009420 ... 0x009423:
  {
    BYTESEL_W(ptimer.alarm,addr,data);
    break;
  }
  case 0x100200 ... 0x100203:
  {
    BYTESEL_W(pfb.config_0,addr,data);
    update_video_mode();
    break;
  }
  case 0x800000 ... 0x87ffff:
  {
    u32 subchannel = ((addr - 0x800000) >> 11) & 7;
    u32 channel = ((addr - 0x800000) >> 14) & 31;
    u32 offset = (addr - 0x800000) & 0x7ff;
    BYTESEL_W(channels[channel][subchannel].regs[offset],addr,data);
    if((offset & 0x7fc) == 0x000) load_object(channel,subchannel);
    break;
  }
  }
}

u8 nv3_device::bar1_rb(u32 addr)
{
  return vram[addr & 0x3FFFFF];
}

void nv3_device::bar1_wb(u32 addr, u8 data)
{
  vram[addr & 0x3FFFFF] = data;
}

void nv3_device::update_video_mode()
{
  //TODO: RESOLUTION FORMULA NOT VERIFIED
  int width;
  if((pfb.config_0 & 0x3f) >= 0x10) width = (((pfb.config_0 & 0x3f) - 0x10) << 5) + 512;
  else width = 480 - ((0x0f - (pfb.config_0 & 0x3f)) * 40);
  int height = (width >> 2) * 3; //Aspect ratio used here
  
  screen = SDL_SetVideoMode(width,height,32,SDL_OPENGL);
}

void nv3_device::load_object(u32 channel, u32 subchannel)
{
  u32 handle = channels[channel][subchannel].regs[0];
  for(int i=0;i<=0x100;i++)
  {
    u32 check = vram[0x400000 - (pfifo.ramht & 0xf000) + (i << 4)];
    check |= vram[0x400000 - (pfifo.ramht & 0xf000) + (i << 4) + 1] << 8;
    check |= vram[0x400000 - (pfifo.ramht & 0xf000) + (i << 4) + 2] << 16;
    check |= vram[0x400000 - (pfifo.ramht & 0xf000) + (i << 4) + 3] << 24;
    if(check == handle) break;
  }
  channels[channel][subchannel].objclass = vram[0x400000 - (pfifo.ramht & 0xf000) + (i << 4) + 6];
  channels[channel][subchannel].objoff = vram[0x400000 - (pfifo.ramht & 0xf000) + (i << 4) + 4];
  channels[channel][subchannel].objoff |= vram[0x400000 - (pfifo.ramht & 0xf000) + (i << 4) + 5] << 8;
  channels[channel][subchannel].objoff <<= 4;
}

void nv3_device::update()
{
  
}