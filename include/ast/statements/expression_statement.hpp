#pragma once
#include "ast/abstract/expression.hpp"
#include "ast/abstract/statement.hpp"

namespace ast
{
   struct Expression_Statement : Statement
   {
      Expression *exp;
      Expression_Statement(Expression *exp_) : exp(exp_) {};
   };
}