#pragma once
#include "ast/abstract/expression.hpp"
#include "ast/top_level/identifier.hpp"
#include <iostream>
#include <memory>
#include <utility>

namespace ast
{
   struct Var : Expression
   {
   public:
      std::shared_ptr<Identifier> identifier;
      Var(std::shared_ptr<Identifier> identifier_) : identifier(std::move(identifier_)) {}

   protected:
      void print(std::ostream &ostr) const override
      {
         ostr << "Var(" << *identifier << ")" << std::endl;
      };
   };
}