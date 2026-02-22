#pragma once
#include "aast/abstract/operand.hpp"
#include <ostream>

namespace aast
{
  struct Imm : Operand
  {
  public:
    int val;
    Imm(int val_) : val(val_) {}

  protected:
    void write(std::ostream &ostr) const override { ostr << "$" << val; }
  };
}
