#pragma once
#include "tacky/abstract/instruction.hpp"
#include "tacky/abstract/unary_operator.hpp"
#include "tacky/abstract/val.hpp"

namespace tacky
{
  struct Unary : Instruction
  {
    Unary_Operator *unary_operator;
    Val *src, *dst;
    Unary(Unary_Operator *unary_operator_, Val *src_, Val *dst_) : unary_operator(unary_operator_), src(src_), dst(dst_) {}
    ~Unary()
    {
      delete unary_operator;
      delete src;
      // delete dst;
      /*
      The return struct contains the final dst variable so we only need to
      delete sources since it will clear each dst for us in the upcoming
      instructions starting from the first instruction
      */
    }
  };
}