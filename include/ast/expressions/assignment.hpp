#pragma once
#include "ast/abstract/expression.hpp"
#include <ostream>

namespace ast
{
   struct Assignment : Expression
   {
   public:
      Expression *lvalue;
      Expression *exp;
      Assignment(Expression *lvalue_, Expression *exp_) : lvalue(lvalue_), exp(exp_) {};
      ~Assignment()
      {
         delete lvalue;
         delete exp;
      }

   protected:
      void print(std::ostream &ostr) const override {};
   };
}