#pragma once
#include "aast/abstract/instruction.hpp"
#include "aast/registers/reg_type.hpp"
#include <ostream>

namespace aast {
  struct Cdq : Instruction {
  public:
    aast::Size size;
    Cdq(aast::Size size_) : size(size_) {}
  protected:
    void write(std::ostream& ostr) const override { ostr  << (size == aast::Size::DWORD ? "cdq" : "cqo") << "\n"; }
  };
} // namespace aast
