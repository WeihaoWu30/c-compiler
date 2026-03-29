#pragma once
#include "aast/abstract/instruction.hpp"
#include "aast/abstract/operand.hpp"
#include "aast/abstract/binary_operator.hpp"
#include <ostream>
#include <memory>
#include <utility>

namespace aast
{
  struct Binary : Instruction
  {
  public:
    std::unique_ptr<Binary_Operator> binary_operator;
    std::shared_ptr<Operand> operand1, operand2;
    Binary(std::unique_ptr<Binary_Operator> binary_operator_, std::shared_ptr<Operand> operand1_, std::shared_ptr<Operand> operand2_) : binary_operator(std::move(binary_operator_)), operand1(std::move(operand1_)), operand2(std::move(operand2_)) {}

  protected:
    void write(std::ostream &ostr) const override
    {
      ostr << *binary_operator << "\t" << *operand1 << ", " << *operand2 << std::endl;
    }
  };
}
