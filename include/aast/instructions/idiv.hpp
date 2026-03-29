#pragma once
#include "aast/abstract/instruction.hpp"
#include "aast/abstract/operand.hpp"
#include <ostream>
#include <memory>
#include <utility>

namespace aast
{
  struct Idiv : Instruction
  {
  public:
    std::shared_ptr<Operand> operand;
    Idiv(std::shared_ptr<Operand> operand_) : operand(std::move(operand_)) {}

  protected:
    void write(std::ostream &ostr) const override
    {
      ostr << "idivl" << "\t" << *operand << std::endl;
    }
  };
}
