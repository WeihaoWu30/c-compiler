#pragma once
#include "aast/abstract/binary_operator.hpp"

namespace aast
{
  struct Add : Binary_Operator
  {
    Add() { instruction = "addl"; }
  };

  struct Sub : Binary_Operator
  {
    Sub() { instruction = "subl"; }
  };

  struct Mult : Binary_Operator
  {
    Mult() { instruction = "imull"; }
  };

  struct And : Binary_Operator
  {
    And() { instruction = "andl"; }
  };

  struct Or : Binary_Operator
  {
    Or() { instruction = "orl"; }
  };

  struct Xor : Binary_Operator
  {
    Xor() { instruction = "xorl"; }
  };

  struct Shr : Binary_Operator
  {
    Shr() { instruction = "shrl"; }
  };

  struct Shl : Binary_Operator
  {
    Shl() { instruction = "shll"; }
  };

  struct Sar : Binary_Operator
  {
    Sar() { instruction = "sarl"; }
  };
}