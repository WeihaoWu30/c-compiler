#pragma once
#include "aast/abstract/instruction.hpp"
#include "aast/abstract/operand.hpp"
#include <ostream>

namespace aast
{
  struct Push : Instruction
  {
    Operand *operand;
    Push(Operand *operand_) : operand(operand_) {}
    void write(std::ostream &ostr) const override { ostr << "pushq\t" << *operand << std::endl; }
  };
}