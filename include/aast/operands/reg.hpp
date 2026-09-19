#pragma once
#include "aast/abstract/operand.hpp"
#include "aast/registers/reg_type.hpp"
#include <ostream>

namespace aast {
  struct Reg : Operand {
  public:
    RegType reg_type;
    Reg(RegType reg_type_, Size size_) : reg_type(reg_type_) {
      size = size_;
    }

  protected:
    void write(std::ostream& ostr) const override { ostr << assembly_match(reg_type, size); }
  };
} // namespace aast