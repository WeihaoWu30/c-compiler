#pragma once
#include "aast/abstract/operand.hpp"
#include <ostream>

namespace aast
{
  struct Stack : Operand
  {
  public:
    int offset;
    Stack(int offset_) : offset(offset_) {}

  protected:
    void write(std::ostream &ostr) const override
    {
      ostr << offset << "(%rbp)";
    }
  };
}
