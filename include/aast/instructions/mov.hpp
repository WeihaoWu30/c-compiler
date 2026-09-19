#pragma once
#include "aast/abstract/instruction.hpp"
#include "aast/abstract/operand.hpp"
#include "aast/registers/reg_type.hpp"
#include <ostream>

namespace aast {
  struct Mov : Instruction {
  public:
    Operand *src, *dst;
    aast::Size size;
    Mov(Operand* src_, Operand* dst_, aast::Size size_) : src(src_), dst(dst_), size(size_) {}

  protected:
    void write(std::ostream& ostr) const override {
      ostr << "mov" << (size == aast::Size::DWORD ? "l" : "q") << "\t"
           << *src << ", " << *dst << "\n";
    }
  };
} // namespace aast