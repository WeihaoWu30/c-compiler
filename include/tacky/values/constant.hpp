#pragma once
#include "tacky/abstract/val.hpp"

namespace tacky
{
  struct Constant : Val
  {
    int val;
    Constant(int val_) : val(val_) {}
  };
}