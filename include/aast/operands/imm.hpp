#pragma once
#include "aast/abstract/operand.hpp"
#include "aast/registers/reg_type.hpp"
#include <ostream>
#include <variant>

namespace aast {
  struct Imm : Operand {
  public:
    std::variant<int, long> val;
    Imm(std::variant<int, long> val_, Size size_) : val(val_) {
      size = size_;
    }

  protected:
    void write(std::ostream& ostr) const override {
      if(std::holds_alternative<int>(val)) {
        ostr << "$" << std::get<int>(val);
      } else if (std::holds_alternative<long>(val)) {
        ostr << "$" << std::get<long>(val);
      }
    }
  };
} // namespace aast
