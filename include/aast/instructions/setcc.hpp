#pragma once
#include "aast/abstract/instruction.hpp"
#include "aast/abstract/operand.hpp"
#include "aast/abstract/cond_code.hpp"
#include "aast/registers/reg_type.hpp"
#include "aast/operands/reg.hpp"
#include <ostream>

namespace aast
{
  struct SetCC : Instruction
  {
  public:
    Cond_Code *cond_code;
    Operand *operand;
    SetCC(Cond_Code *cond_code_, Operand *operand_) : cond_code(cond_code_), operand(operand_) {}
    ~SetCC()
    {
      delete cond_code;
    }

  protected:
    void write(std::ostream &ostr) const override
    {
      ostr << "set" << *cond_code << "\t" << *operand << std::endl;
    }
  };
}