#pragma once
#include "ast/abstract/expression.hpp"
#include "ast/abstract/unary_operator.hpp"
#include <ostream>
#include <memory>
#include <utility>

namespace ast
{
   struct Unary : Expression
   {
   public:
      std::unique_ptr<Unary_Operator> unary_operator;
      std::unique_ptr<Expression> exp;
      Unary(std::unique_ptr<Unary_Operator> unary_operator_, std::unique_ptr<Expression> exp_)
          : unary_operator(std::move(unary_operator_)), exp(std::move(exp_)) {}

   protected:
      void print(std::ostream &ostr) const override
      {
         ostr << "Unary(" << *unary_operator << ", " << *exp << ")";
      }
   };
}