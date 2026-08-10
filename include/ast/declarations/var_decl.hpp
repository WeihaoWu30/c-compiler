#pragma once
#include "ast/abstract/expression.hpp"
#include "ast/abstract/declaration.hpp"
#include "ast/top_level/identifier.hpp"
#include "ast/storage_class/storage_classes.hpp"

namespace ast
{
   struct Var_Decl : Declaration
   {
      Expression *init;
      Storage_Class storage_class;
      Var_Decl(
         Identifier *name_, Expression *init_ = nullptr, 
         Storage_Class storage_class_ = Storage_Class::NONE
      ) : init(init_), storage_class(storage_class_) {
         name = name_; // name owned by declaration
      };
   };
}