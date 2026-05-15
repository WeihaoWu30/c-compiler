#pragma once
#include "tacky/abstract/instruction.hpp"
#include "tacky/operators/binary_operators.hpp"
#include "tacky/abstract/val.hpp"

namespace tacky
{
  struct Compound : Instruction
  {
    Binary_Operator binary_operator;
    Val *src, *dst;
    Compound(Binary_Operator binary_operator_, Val *src, Val *dst_) : binary_operator(binary_operator_), src(src), dst(dst_) {}
  };
}