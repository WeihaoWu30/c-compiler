#pragma once
#include "aast/abstract/operand.hpp"
#include "aast/registers/reg_type.hpp"
#include <ostream>

namespace aast
{
  struct Reg : Operand
  {
  public:
    RegType *reg;
    Reg(RegType *reg_) : reg(reg_) {}
    ~Reg() { delete reg; }

  protected:
    void write(std::ostream &ostr) const override
    {
      ostr << *reg;
    }
  };
}