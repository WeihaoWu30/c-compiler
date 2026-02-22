#pragma once
#include "aast/abstract/instruction.hpp"
#include "aast/abstract/unary_operator.hpp"
#include "aast/abstract/operand.hpp"
#include <ostream>

namespace aast
{
  struct Unary : Instruction
  {
  public:
    Unary_Operator *unary_operator;
    Operand *operand;
    Unary(Unary_Operator *unary_operator_, Operand *operand_) : unary_operator(unary_operator_), operand(operand_) {}
    ~Unary()
    {
      delete unary_operator;
      delete operand;
    }

  protected:
    void write(std::ostream &ostr) const override
    {
      ostr << *unary_operator << "\t" << *operand << std::endl;
    }
  };
}