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
}