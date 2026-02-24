#pragma once
#include "tacky/abstract/instruction.hpp"
#include "tacky/abstract/val.hpp"

namespace tacky
{
  struct Return : Instruction
  {
    Val *val;
    Return(Val *val_) : val(val_) {}
    ~Return()
    {
      delete val;
    }
  };
}