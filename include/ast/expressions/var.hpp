#pragma once
#include "ast/abstract/expression.hpp"
#include "ast/top_level/identifier.hpp"

namespace ast
{
   struct Var : Expression
   {
      Identifier *identifier;
      Var(Identifier *identifier_) : identifier(identifier_) {}
      ~Var() { delete identifier; }
   };
}