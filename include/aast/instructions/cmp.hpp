#pragma once
#include "aast/abstract/instruction.hpp"
#include "aast/abstract/operand.hpp"
#include "aast/registers/reg_type.hpp"
#include <ostream>

namespace aast {
  struct Cmp : Instruction {
  public:
    Operand *operand1, *operand2;
    aast::Size size;
    Cmp(Operand* operand1_, Operand* operand2_, aast::Size size_) : operand1(operand1_), operand2(operand2_), size(size_) {}

  protected:
    void write(std::ostream& ostr) const override { ostr << "cmp" << (size == aast::Size::DWORD ? "l" : "q") << "\t" << *operand1 << ", " << *operand2 << "\n"; }
  };
} // namespace aast