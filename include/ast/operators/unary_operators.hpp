#pragma once
#include "ast/abstract/unary_operator.hpp"

namespace ast
{
   struct Complement : Unary_Operator
   {
   };

   struct Negate : Unary_Operator
   {
   };

   struct Unary_Not : Unary_Operator
   {
   };
}