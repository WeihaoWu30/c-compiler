#pragma once
#include "tacky/abstract/instruction.hpp"
#include "tacky/abstract/binary_operator.hpp"
#include "tacky/abstract/val.hpp"

namespace tacky
{
  struct Binary : Instruction
  {
    Binary_Operator *binary_operator;
    Val *src1, *src2, *dst;
    Binary(Binary_Operator *binary_operator_, Val *src1_, Val *src2_, Val *dst_) : binary_operator(binary_operator_), src1(src1_), src2(src2_), dst(dst_) {}
    ~Binary()
    {
      delete binary_operator;
      delete src1;
      delete src2;
      // delete dst; Same reasoning as Unary
    }
  };
}