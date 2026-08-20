#pragma once
#include "aast/abstract/instruction.hpp"
#include <ostream>

namespace aast {
  struct Cdq : Instruction {
  protected:
    void write(std::ostream& ostr) const override { ostr << "cdq\n"; }
  };
} // namespace aast
