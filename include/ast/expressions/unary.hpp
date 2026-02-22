#pragma once
#include "ast/abstract/expression.hpp"
#include "ast/abstract/unary_operator.hpp"
#include <ostream>

namespace ast
{
   struct Unary : Expression
   {
   public:
      Unary_Operator *unary_operator;
      Expression *exp;
      Unary(Unary_Operator *unary_operator_, Expression *exp_)
          : unary_operator(unary_operator_), exp(exp_) {}
      ~Unary()
      {
         delete unary_operator;
         delete exp;
      }

   protected:
      void print(std::ostream &ostr) const override
      {
         ostr << "Unary(" << *unary_operator << ", " << *exp << ")";
      }
   };
}