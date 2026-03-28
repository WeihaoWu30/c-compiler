#pragma once
#include "ast/abstract/expression.hpp"
#include <ostream>
#include <memory>
#include <utility>

namespace ast
{
   struct Assignment : Expression
   {
   public:
      std::unique_ptr<Expression> lvalue;
      std::unique_ptr<Expression> exp;
      Assignment(std::unique_ptr<Expression> lvalue_, std::unique_ptr<Expression> exp_) : lvalue(std::move(lvalue_)), exp(std::move(exp_)) {};

   protected:
      void print(std::ostream &ostr) const override
      {
         ostr << "Assignment(" << *lvalue << ", " << *exp << ")" << std::endl;
      };
   };
}