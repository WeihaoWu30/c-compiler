#pragma once
#include "ast/abstract/expression.hpp"
#include "ast/abstract/statement.hpp"
#include <ostream>

namespace ast
{
   struct Expression_Statement : Statement
   {
   public:
      Expression *exp;
      Expression_Statement(Expression *exp_) : exp(exp_) {};
      ~Expression_Statement() { delete exp; }

   protected:
      void print(std::ostream &ostr) const override { ostr << std::endl; };
   };
}