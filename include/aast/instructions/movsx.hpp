#pragma once
#include "aast/abstract/operand.hpp"
#include "aast/abstract/instruction.hpp"

namespace aast {
  struct Movsx : Instruction {
  public:
    Operand *src, *dst;
    Movsx(Operand* src_, Operand* dst_) : src(src_), dst(dst_) {}
  protected:
    void write(std::ostream& ostr) const override { ostr << "movslq\t" << *src << ", " << *dst << "\n"; };
  };
}