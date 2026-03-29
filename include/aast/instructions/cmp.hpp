#pragma once
#include "aast/abstract/instruction.hpp"
#include "aast/abstract/operand.hpp"
#include <ostream>
#include <memory>
#include <utility>

namespace aast
{
  struct Cmp : Instruction
  {
  public:
    std::shared_ptr<Operand> operand1, operand2;
    Cmp(std::shared_ptr<Operand> operand1_, std::shared_ptr<Operand> operand2_) : operand1(std::move(operand1_)), operand2(std::move(operand2_)) {}

  protected:
    void write(std::ostream &ostr) const override
    {
      ostr << "cmpl\t" << *operand1 << ", " << *operand2 << std::endl;
    }
  };
}