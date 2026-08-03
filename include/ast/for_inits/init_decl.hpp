#pragma once
#include "ast/abstract/for_init.hpp"
#include "ast/top_level/declaration.hpp"

namespace ast
{
  struct Init_Decl : For_Init
  {
    Declaration *declaration;
    Init_Decl(Declaration *declaration_) : declaration(declaration_) {}
    ~Init_Decl() { delete declaration; }
  };
}