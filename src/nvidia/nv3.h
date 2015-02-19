#ifndef NV3_H
#define NV3_H

#include "../pci.h"

struct nv3_device : public PCI::device
{
  u8 vram[0x400000]; //4 MB
  struct
  {
    u32 enable;
  } pmc;
  struct
  {
    u32 ramht;
  } pfifo;
  struct
  {
    u64 time;
    u32 clkmul;
    u32 clkdiv;
    u32 alarm;
    double mclk;
  } ptimer;
  struct
  {
    u32 config_0;
  } pfb;
  struct
  {
    u32 regs[0x800];
    u8 objclass;
    u32 objoff;
  } channels[32][8];
  void load_object(u32 channel, u32 subchannel);
  u8 bar0_rb(u32 addr);
  void bar0_wb(u32 addr, u8 data);
  u8 bar1_rb(u32 addr);
  void bar1_wb(u32 addr, u8 data);
  void tick();
  void update();
  void update_video_mode();
  nv3_device() : id(0x10de0018)
  {
    PCI::bar tmp;
    
    //BAR 0: MMIO
    tmp.rb = bar0_rb;
    tmp.wb = bar0_wb;
    tmp.loc = 0;
    tmp.io = false;
    
    bars[0] = tmp;
    
    //BAR 1: VRAM
    tmp.rb = bar1_rb;
    tmp.wb = bar1_wb;
    tmp.size = 0x400000;
    tmp.loc = 0;
    tmp.io = false;
    
    bars[1] = tmp;
  }
}

#endif