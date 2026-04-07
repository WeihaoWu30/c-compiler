#pragma once
#include "ast/abstract/expression.hpp"
#include "ast/abstract/unary_operator.hpp"

namespace ast
{
   struct Unary : Expression
   {
      Unary_Operator *unary_operator;
      Expression *exp;
      Unary(Unary_Operator *unary_operator_, Expression *exp_)
          : unary_operator(unary_operator_), exp(exp_) {}
      ~Unary()
      {
         delete unary_operator;
      }
   };
}