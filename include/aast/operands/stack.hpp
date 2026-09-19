#pragma once
#include "aast/abstract/operand.hpp"
#include <ostream>

namespace aast {
  struct Stack : Operand {
  public:
    int offset;
    Stack(int offset_, Size size_) : offset(offset_) {
      size = size_;
    }

  protected:
    void write(std::ostream& ostr) const override { ostr << offset << "(%rbp)"; }
  };
} // namespace aast
