#pragma once
#include "aast/abstract/binary_operator.hpp"

namespace aast {
  struct Add : Binary_Operator {
    Add() { instruction = "add"; }
  };

  struct Sub : Binary_Operator {
    Sub() { instruction = "sub"; }
  };

  struct Mult : Binary_Operator {
    Mult() { instruction = "imul"; }
  };

  struct And : Binary_Operator {
    And() { instruction = "and"; }
  };

  struct Or : Binary_Operator {
    Or() { instruction = "or"; }
  };

  struct Xor : Binary_Operator {
    Xor() { instruction = "xor"; }
  };

  struct Shr : Binary_Operator {
    Shr() { instruction = "shr"; }
  };

  struct Shl : Binary_Operator {
    Shl() { instruction = "shl"; }
  };

  struct Sar : Binary_Operator {
    Sar() { instruction = "sar"; }
  };
} // namespace aast