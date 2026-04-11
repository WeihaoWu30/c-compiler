#pragma once
#include "tacky/abstract/instruction.hpp"
#include "tacky/operators/unary_operators.hpp"
#include "tacky/abstract/val.hpp"

namespace tacky
{
  struct Unary : Instruction
  {
    Unary_Operator unary_operator;
    Val *src, *dst;
    Unary(Unary_Operator unary_operator_, Val *src_, Val *dst_) : unary_operator(unary_operator_), src(src_), dst(dst_) {}
  };
}