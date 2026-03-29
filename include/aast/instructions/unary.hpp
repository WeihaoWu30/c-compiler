#pragma once
#include "aast/abstract/instruction.hpp"
#include "aast/abstract/unary_operator.hpp"
#include "aast/abstract/operand.hpp"
#include <ostream>
#include <memory>
#include <utility>

namespace aast
{
  struct Unary : Instruction
  {
  public:
    std::unique_ptr<Unary_Operator> unary_operator;
    std::shared_ptr<Operand> operand;
    Unary(std::unique_ptr<Unary_Operator> unary_operator_, std::shared_ptr<Operand> operand_) : unary_operator(std::move(unary_operator_)), operand(std::move(operand_)) {}

  protected:
    void write(std::ostream &ostr) const override
    {
      ostr << *unary_operator << "\t" << *operand << std::endl;
    }
  };
}