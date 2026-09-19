#pragma once
#include "aast/abstract/instruction.hpp"
#include "aast/abstract/operand.hpp"
#include "aast/registers/reg_type.hpp"
#include <ostream>

namespace aast {
  struct Idiv : Instruction {
  public:
    Operand* operand;
    aast::Size size;
    Idiv(Operand* operand_, aast::Size size_) : operand(operand_), size(size_) {}

  protected:
    void write(std::ostream& ostr) const override {
      ostr << "idivl"
           << "\t" << *operand << "\n";
    }
  };
} // namespace aast
