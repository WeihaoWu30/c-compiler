#pragma once
#include "aast/abstract/instruction.hpp"
#include "aast/abstract/operand.hpp"
#include <ostream>

namespace aast
{
  struct Mov : Instruction
  {
  public:
    Operand *src, *dst;
    Mov(Operand *src_, Operand *dst_) : src(src_), dst(dst_) {}
    ~Mov()
    {
      delete src;
      delete dst;
    }

  protected:
    void write(std::ostream &ostr) const override
    {
      ostr << "movl" << "\t" << *src << ", " << *dst << std::endl;
    }
  };
}