#pragma once
#include "aast/abstract/instruction.hpp"
#include <ostream>

namespace aast
{
  struct DeallocateStack : Instruction
  {
    int val;
    DeallocateStack(int val_) : val(val_) {}
    void write(std::ostream &ostr) const override { ostr << "addq\t" << "$" << val << ", " << "%rsp\n"; }
  };
}