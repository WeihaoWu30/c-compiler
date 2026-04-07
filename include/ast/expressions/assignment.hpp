#pragma once
#include "ast/abstract/expression.hpp"

namespace ast
{
   struct Assignment : Expression
   {
      Expression *lvalue;
      Expression *exp;
      Assignment(Expression *lvalue_, Expression *exp_) : lvalue(lvalue_), exp(exp_) {};
   };
}