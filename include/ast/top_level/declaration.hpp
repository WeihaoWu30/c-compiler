#pragma once
#include "ast/abstract/expression.hpp"
#include "ast/top_level/identifier.hpp"
#include <ostream>

namespace ast
{
   struct Declaration
   {
      Identifier *name;
      Expression *init;
      Declaration(Identifier *name_, Expression *init_ = NULL) : name(name_), init(init_) {};
      ~Declaration()
      {
         delete name;
      }
   };
}