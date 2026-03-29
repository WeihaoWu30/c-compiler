#pragma once
#include "aast/abstract/instruction.hpp"
#include "aast/abstract/operand.hpp"
#include "aast/abstract/cond_code.hpp"
#include "aast/registers/reg_type.hpp"
#include "aast/operands/reg.hpp"
#include <ostream>
#include <memory>
#include <utility>

namespace aast
{
  struct SetCC : Instruction
  {
  public:
    std::unique_ptr<Cond_Code> cond_code;
    std::shared_ptr<Operand> operand;
    SetCC(std::unique_ptr<Cond_Code> cond_code_, std::shared_ptr<Operand> operand_) : cond_code(std::move(cond_code_)), operand(std::move(operand_)) {}

  protected:
    void write(std::ostream &ostr) const override
    {
      ostr << "set" << *cond_code << "\t";
      Reg *reg = dynamic_cast<Reg *>(operand.get());
      if (reg)
      {
        AX *ax = dynamic_cast<AX *>(reg);
        DX *dx = dynamic_cast<DX *>(reg);
        R10 *r10 = dynamic_cast<R10 *>(reg);
        R11 *r11 = dynamic_cast<R11 *>(reg);
        if (ax)
        {
          ostr << "%al";
        }
        else if (dx)
        {
          ostr << "%dl";
        }
        else if (r10)
        {
          ostr << "%r10b";
        }
        else if (r11)
        {
          ostr << "%r11b";
        }
      }
      else
      {
        ostr << *operand;
      }
      ostr << std::endl;
    }
  };
}