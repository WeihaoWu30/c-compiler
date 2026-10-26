#pragma once
#include "aast/abstract/instruction.hpp"
#include "aast/abstract/operand.hpp"
#include "aast/abstract/unary_operator.hpp"
#include "aast/registers/reg_type.hpp"
#include <ostream>

namespace aast {
  struct Unary : Instruction {
  public:
    Unary_Operator* unary_operator;
    Operand* operand;
    aast::Size size;
    Unary(Unary_Operator* unary_operator_, Operand* operand_, aast::Size size_) : unary_operator(unary_operator_), operand(operand_), size(size_) {}
    ~Unary() { delete unary_operator; }

  protected:
    void write(std::ostream& ostr) const override { ostr << *unary_operator << (size == aast::Size::DWORD ? "l" : "q") << "\t" << *operand << "\n"; }
  };
} // namespace aast