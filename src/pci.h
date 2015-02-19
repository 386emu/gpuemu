#ifndef PCI_H
#define PCI_H

#include "common.h"

namespace PCI
{
struct bar
{
  std::function<u8(u32)> rb;
  std::function<void(u32,u8)> wb;
  u32 base;
  u32 size;
  u8 loc;
  bool io;
};

struct device
{
  u32 id;
  u32 sid;
  bar bars[6];
  bar erba;
  std::function<u8(u8)> config_rb;
  std::function<void(u8,u8)> config_wb;
  bool inta;
};
}

#endif