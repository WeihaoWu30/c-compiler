#pragma once
#include <variant>

namespace ast {
  struct ConstInt {
    int val;
    ConstInt(int val_) : val(val_) {}
  };
  
  struct ConstLong {
    long val;
    ConstLong(long val_) : val(val_) {}
  };

  using Const = std::variant<ConstInt, ConstLong>;
}