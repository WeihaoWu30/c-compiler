#pragma once
#include "aast/abstract/binary_operator.hpp"
#include "aast/abstract/instruction.hpp"
#include "aast/abstract/operand.hpp"
#include "aast/registers/reg_type.hpp"
#include <ostream>

namespace aast {
  struct Binary : Instruction {
  public:
    Binary_Operator* binary_operator;
    Operand *operand1, *operand2;
    aast::Size size;
    Binary(Binary_Operator* binary_operator_, Operand* operand1_, Operand* operand2_, aast::Size size_) : binary_operator(binary_operator_), operand1(operand1_), operand2(operand2_), size(size_) {}
    ~Binary() { delete binary_operator; }

  protected:
    void write(std::ostream& ostr) const override { ostr << *binary_operator << (size == aast::Size::DWORD ? "l" : "q") << "\t" << *operand1 << ", " << *operand2 << "\n"; }
  };
} // namespace aast
