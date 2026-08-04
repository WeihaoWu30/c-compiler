#pragma once
#include "ast/abstract/expression.hpp"
#include "ast/abstract/declaration.hpp"
#include "ast/top_level/identifier.hpp"

namespace ast
{
   struct Var_Decl : Declaration
   {
      Expression *init;
      Var_Decl(Identifier *name_, Expression *init_ = nullptr) : init(init_) {
         name = name_; // name owned by declaration
      };
   };
}