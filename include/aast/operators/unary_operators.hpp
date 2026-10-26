#pragma once
#include "aast/abstract/unary_operator.hpp"

namespace aast {
  struct Neg : Unary_Operator {
    Neg() { instruction = "neg"; }
  };
  struct Not : Unary_Operator {
    Not() { instruction = "not"; }
  };
} // namespace aast