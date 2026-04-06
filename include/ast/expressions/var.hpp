#pragma once
#include "ast/abstract/expression.hpp"
#include "ast/top_level/identifier.hpp"
#include <iostream>

namespace ast
{
   struct Var : Expression
   {
   public:
      Identifier *identifier;
      Var(Identifier *identifier_) : identifier(identifier_) {}
      ~Var() { delete identifier; }

   protected:
      void print(std::ostream &ostr) const override { ostr << std::endl; };
   };
}