#pragma once
#include "aast/abstract/instruction.hpp"
#include "aast/abstract/operand.hpp"
#include <ostream>

namespace aast
{
  struct Idiv : Instruction
  {
  public:
    Operand *operand;
    Idiv(Operand *operand_) : operand(operand_) {}
    ~Idiv() { delete operand; }

  protected:
    void write(std::ostream &ostr) const override
    {
      ostr << "idivl" << "\t" << *operand << std::endl;
    }
  };
}
