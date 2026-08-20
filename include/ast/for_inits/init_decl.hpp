#pragma once
#include "ast/abstract/for_init.hpp"
#include "ast/declarations/var_decl.hpp"

namespace ast {
  struct Init_Decl : For_Init {
    Var_Decl* variable_declaration;
    Init_Decl(Var_Decl* variable_declaration_) : variable_declaration(variable_declaration_) {}
    ~Init_Decl() { delete variable_declaration; }
  };
} // namespace ast