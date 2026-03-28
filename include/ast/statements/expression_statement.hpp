#pragma once
#include "ast/abstract/expression.hpp"
#include "ast/abstract/statement.hpp"
#include <ostream>
#include <memory>
#include <utility>

namespace ast
{
   struct Expression_Statement : Statement
   {
   public:
      std::unique_ptr<Expression> exp;
      Expression_Statement(std::unique_ptr<Expression> exp_) : exp(std::move(exp_)) {};

   protected:
      void print(std::ostream &ostr) const override
      {
         ostr << "Expression Statement(" << *exp << ")" << std::endl;
      };
   };
}