#pragma once
#include "aast/abstract/instruction.hpp"
#include "aast/abstract/operand.hpp"
#include <ostream>

namespace aast
{
  struct Cmp : Instruction
  {
  public:
    Operand *operand1, *operand2;
    Cmp(Operand *operand1_, Operand *operand2_) : operand1(operand1_), operand2(operand2_) {}

  protected:
    void write(std::ostream &ostr) const override
    {
      ostr << "cmpl\t" << *operand1 << ", " << *operand2 << std::endl;
    }
  };
}