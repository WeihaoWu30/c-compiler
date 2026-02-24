#pragma once
#include "aast/abstract/instruction.hpp"
#include "aast/abstract/operand.hpp"
#include "aast/abstract/binary_operator.hpp"
#include <ostream>

namespace aast
{
  struct Binary : Instruction
  {
  public:
    Binary_Operator *binary_operator;
    Operand *operand1, *operand2;
    Binary(Binary_Operator *binary_operator_, Operand *operand1_, Operand *operand2_) : binary_operator(binary_operator_), operand1(operand1_), operand2(operand2_) {}
    ~Binary()
    {
      delete binary_operator;
      delete operand1;
      delete operand2;
    }

  protected:
    void write(std::ostream &ostr) const override
    {
      ostr << *binary_operator << "\t" << *operand1 << ", " << *operand2 << std::endl;
    }
  };
}
