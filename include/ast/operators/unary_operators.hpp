#pragma once
#include "ast/abstract/unary_operator.hpp"

namespace ast
{
   struct Complement : Unary_Operator
   {
      Complement() { symbol = "~"; }
   };

   struct Negate : Unary_Operator
   {
      Negate() { symbol = "-"; }
   };

   struct Unary_Not : Unary_Operator
   {
      Unary_Not() { symbol = "!"; }
   };
}